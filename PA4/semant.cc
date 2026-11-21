

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include <set>

using std::cerr;
using std::endl;

extern int semant_debug;
extern char *curr_filename;

int curr_lineno = 0;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

static bool same_method_signature(Feature child, Feature parent) {
    method_class* child_method = static_cast<method_class*>(child);
    method_class* parent_method = static_cast<method_class*>(parent);

    if (child_method->get_return_type() !=
        parent_method->get_return_type()) {
        return false;
    }

    Formals child_formals = child_method->get_formals();
    Formals parent_formals = parent_method->get_formals();

    if (child_formals->len() != parent_formals->len()) {
        return false;
    }

    for (int i = child_formals->first();
        child_formals->more(i);
        i = child_formals->next(i)) {

        formal_class* child_formal =
            static_cast<formal_class*>(child_formals->nth(i));

        formal_class* parent_formal =
            static_cast<formal_class*>(parent_formals->nth(i));

        if (child_formal->get_type_decl() !=
            parent_formal->get_type_decl()) {
            return false;
        }
    }

    return true;
}

ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    install_basic_classes();

    // Register user-defined classes.
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // SELF_TYPE cannot be used as a class name.
        if (class_name == SELF_TYPE) {
            semant_error(current_class)
                << "SELF_TYPE cannot be used as a class name." << endl;
            continue;
        }

        // Reject duplicate class definitions.
        if (class_map.find(class_name) != class_map.end()) {
            semant_error(current_class)
                << "Class " << class_name
                << " was previously defined." << endl;
            continue;
        }

        class_map[class_name] = current_class;
    }
    // Validate parent classes after registering all user classes.
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();
        Symbol parent_name = current_class->get_parent();

        // Skip invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator it = class_map.find(class_name);
        if (it == class_map.end() || it->second != current_class) {
            continue;
        }

        // Basic types and SELF_TYPE cannot be inherited.
        if (parent_name == Int || parent_name == Bool ||
            parent_name == Str || parent_name == SELF_TYPE) {
            semant_error(current_class)
                << "Class " << class_name
                << " cannot inherit class " << parent_name
                << "." << endl;
            continue;
        }

        // The parent class must exist.
        if (class_map.find(parent_name) == class_map.end()) {
            semant_error(current_class)
                << "Class " << class_name
                << " inherits from an undefined class "
                << parent_name << "." << endl;
        }
    }
    // Detect cycles in the inheritance graph.
    std::set<Symbol> checked;

    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ start_class = classes->nth(i);
        Symbol start_name = start_class->get_name();

        // Ignore invalid or duplicate declarations.
        std::map<Symbol, Class_>::iterator start_it =
            class_map.find(start_name);

        if (start_it == class_map.end() ||
            start_it->second != start_class) {
            continue;
        }

        std::set<Symbol> path;
        Symbol current = start_name;

        while (true) {
            if (checked.count(current)) {
                break;
            }

            std::map<Symbol, Class_>::iterator it =
                class_map.find(current);

            if (it == class_map.end()) {
                break;
            }

            // Revisiting a class in the current path indicates a cycle.
            if (!path.insert(current).second) {
                semant_error(it->second)
                    << "Class " << current
                    << " is involved in an inheritance cycle."
                    << endl;
                break;
            }

            Symbol parent = it->second->get_parent();

            if (parent == No_class) {
                break;
            }

            current = parent;
        }

        checked.insert(path.begin(), path.end());
    }
    // Collect attributes and methods declared in each class.
    for (std::map<Symbol, Class_>::iterator it = class_map.begin();
         it != class_map.end(); ++it) {

        Symbol class_name = it->first;
        Class_ current_class = it->second;
        Features features = current_class->get_features();

        for (int i = features->first(); features->more(i);
             i = features->next(i)) {

            Feature feature = features->nth(i);
            Symbol feature_name = feature->get_name();

            if (feature->is_method()) {
                // Detect methods declared more than once.
                if (class_methods[class_name].count(feature_name)) {
                    semant_error(current_class->get_filename(), feature)
                        << "Method " << feature_name
                        << " is multiply defined in class "
                        << class_name << "." << endl;
                    continue;
                }

                class_methods[class_name][feature_name] = feature;

            } else {
                // Detect attributes declared more than once.
                if (class_attributes[class_name].count(feature_name)) {
                    semant_error(current_class->get_filename(), feature)
                        << "Attribute " << feature_name
                        << " is multiply defined in class "
                        << class_name << "." << endl;
                    continue;
                }

                class_attributes[class_name][feature_name] = feature;
            }
        }
    }
    // Check attributes inherited from ancestor classes.
    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // Ignore invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(class_name);

        if (class_it == class_map.end() ||
            class_it->second != current_class) {
            continue;
        }

        std::map<Symbol, Feature>& attributes =
            class_attributes[class_name];

        for (std::map<Symbol, Feature>::iterator attr_it =
                 attributes.begin();
             attr_it != attributes.end(); ++attr_it) {

            Symbol attr_name = attr_it->first;
            Symbol parent = current_class->get_parent();
            std::set<Symbol> visited;

            while (parent != No_class &&
                   visited.insert(parent).second) {

                std::map<Symbol, Class_>::iterator parent_it =
                    class_map.find(parent);

                if (parent_it == class_map.end()) {
                    break;
                }

                std::map<Symbol, std::map<Symbol, Feature> >::iterator
                    attrs_it = class_attributes.find(parent);

                if (attrs_it != class_attributes.end() &&
                    attrs_it->second.count(attr_name)) {

                    semant_error(current_class->get_filename(),
                                 attr_it->second)
                        << "Attribute " << attr_name
                        << " is an attribute of an inherited class."
                        << endl;
                    break;
                }

                parent = parent_it->second->get_parent();
            }
        }
    }
    // Validate overridden methods against inherited declarations.
    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // Ignore invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(class_name);

        if (class_it == class_map.end() ||
            class_it->second != current_class) {
            continue;
        }

        std::map<Symbol, Feature>& methods =
            class_methods[class_name];

        for (std::map<Symbol, Feature>::iterator method_it =
                 methods.begin();
             method_it != methods.end(); ++method_it) {

            Symbol method_name = method_it->first;
            Feature current_method = method_it->second;

            Symbol parent = current_class->get_parent();
            std::set<Symbol> visited;

            while (parent != No_class &&
                   visited.insert(parent).second) {

                std::map<Symbol, Class_>::iterator parent_it =
                    class_map.find(parent);

                if (parent_it == class_map.end()) {
                    break;
                }

                std::map<Symbol, std::map<Symbol, Feature> >::iterator
                    methods_it = class_methods.find(parent);

                if (methods_it != class_methods.end()) {
                    std::map<Symbol, Feature>::iterator inherited_it =
                        methods_it->second.find(method_name);

                    if (inherited_it != methods_it->second.end()) {

                        if (!same_method_signature(
                                current_method, inherited_it->second)) {

                            semant_error(
                                current_class->get_filename(),
                                current_method)
                                << "Method " << method_name
                                << " is redefined with an incompatible signature."
                                << endl;
                        }

                        // Compare with the nearest inherited declaration.
                        break;
                    }
                }

                parent = parent_it->second->get_parent();
            }
        }
    }
    // Validate declared attribute types.
    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // Skip invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(class_name);

        if (class_it == class_map.end() ||
            class_it->second != current_class) {
            continue;
        }

        std::map<Symbol, Feature>& attributes =
            class_attributes[class_name];

        for (std::map<Symbol, Feature>::iterator it =
                 attributes.begin();
             it != attributes.end(); ++it) {

            attr_class* attribute =
                static_cast<attr_class*>(it->second);

            Symbol declared_type = attribute->get_type_decl();

            if (declared_type != SELF_TYPE &&
                class_map.find(declared_type) == class_map.end()) {

                semant_error(current_class->get_filename(), attribute)
                    << "Class " << declared_type
                    << " of attribute " << it->first
                    << " is undefined." << endl;
            }
        }
    }
    // Validate declared method return types.
    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // Skip invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(class_name);

        if (class_it == class_map.end() ||
            class_it->second != current_class) {
            continue;
        }

        std::map<Symbol, Feature>& methods =
            class_methods[class_name];

        for (std::map<Symbol, Feature>::iterator it =
                 methods.begin();
             it != methods.end(); ++it) {

            method_class* method_node =
                static_cast<method_class*>(it->second);

            Symbol return_type = method_node->get_return_type();

            if (return_type != SELF_TYPE &&
                class_map.find(return_type) == class_map.end()) {

                semant_error(current_class->get_filename(), method_node)
                    << "Undefined return type " << return_type
                    << " in method " << it->first << "." << endl;
            }
        }
    }
    // Validate formal parameters of user-defined methods.
    for (int i = classes->first(); classes->more(i);
         i = classes->next(i)) {

        Class_ current_class = classes->nth(i);
        Symbol class_name = current_class->get_name();

        // Skip invalid or duplicate class declarations.
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(class_name);

        if (class_it == class_map.end() ||
            class_it->second != current_class) {
            continue;
        }

        std::map<Symbol, Feature>& methods =
            class_methods[class_name];

        for (std::map<Symbol, Feature>::iterator method_it =
                 methods.begin();
             method_it != methods.end(); ++method_it) {

            method_class* method_node =
                static_cast<method_class*>(method_it->second);

            Formals formals = method_node->get_formals();
            std::set<Symbol> declared_names;

            for (int j = formals->first(); formals->more(j);
                 j = formals->next(j)) {

                formal_class* formal_node =
                    static_cast<formal_class*>(formals->nth(j));

                Symbol formal_name = formal_node->get_name();
                Symbol formal_type = formal_node->get_type_decl();

                // The identifier self cannot be a formal parameter.
                if (formal_name == self) {
                    semant_error(current_class->get_filename(), formal_node)
                        << "'self' cannot be the name of a formal parameter."
                        << endl;
                }

                // Formal parameter names must be unique.
                if (!declared_names.insert(formal_name).second) {
                    semant_error(current_class->get_filename(), formal_node)
                        << "Formal parameter " << formal_name
                        << " is multiply defined." << endl;
                }

                // SELF_TYPE is not allowed as a formal parameter type.
                if (formal_type == SELF_TYPE) {
                    semant_error(current_class->get_filename(), formal_node)
                        << "Formal parameter " << formal_name
                        << " cannot have type SELF_TYPE." << endl;
                }
                else if (class_map.find(formal_type) == class_map.end()) {
                    semant_error(current_class->get_filename(), formal_node)
                        << "Class " << formal_type
                        << " of formal parameter " << formal_name
                        << " is undefined." << endl;
                }
            }
        }
    }
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    // Register the basic COOL classes.
    class_map[Object] = Object_class;
    class_map[IO] = IO_class;
    class_map[Int] = Int_class;
    class_map[Bool] = Bool_class;
    class_map[Str] = Str_class;
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}


