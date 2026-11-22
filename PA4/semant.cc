
#include <vector>
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
                // An attribute cannot be named self.
                if (feature_name == self) {
                    semant_error(current_class->get_filename(), feature)
                        << "'self' cannot be the name of an attribute."
                        << endl;
                    continue;
                }
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

// Internal marker for recovering from a previous type error.
static Symbol semantic_error_type() {
    return idtable.add_string("<error_type>");
}

bool ClassTable::conforms(
    Symbol actual_type,
    Symbol expected_type,
    Symbol current_class)
{
    // Avoid cascading errors from an invalid declared type.
    if (actual_type == semantic_error_type() ||
        expected_type == semantic_error_type()) {
        return true;
    }

    // No_type conforms to every type.
    // This is useful for expressions without an initializer.
    if (actual_type == No_type) {
        return true;
    }

    // SELF_TYPE conforms to itself.
    if (actual_type == SELF_TYPE &&
        expected_type == SELF_TYPE) {
        return true;
    }

    // A regular class type does not conform to SELF_TYPE.
    if (expected_type == SELF_TYPE) {
        return false;
    }

    // Resolve SELF_TYPE using the current class.
    if (actual_type == SELF_TYPE) {
        actual_type = current_class;
    }

    // Walk through the inheritance hierarchy.
    std::set<Symbol> visited;
    Symbol current = actual_type;

    while (current != No_class &&
           visited.insert(current).second) {

        if (current == expected_type) {
            return true;
        }

        std::map<Symbol, Class_>::iterator it =
            class_map.find(current);

        if (it == class_map.end()) {
            break;
        }

        current = it->second->get_parent();
    }

    return false;
}

bool ClassTable::is_defined_type(Symbol type)
{
    return type == SELF_TYPE ||
           class_map.find(type) != class_map.end();
}

bool ClassTable::has_main_class() {
    Symbol main_symbol = idtable.add_string("Main");

    return class_map.find(main_symbol) != class_map.end();
}

bool ClassTable::has_main_method() {
    Symbol main_class = idtable.add_string("Main");
    Symbol main_method = idtable.add_string("main");

    // Search only methods declared directly in class Main.
    std::map<Symbol, std::map<Symbol, Feature> >::iterator
        class_it = class_methods.find(main_class);

    if (class_it == class_methods.end()) {
        return false;
    }

    return class_it->second.find(main_method) !=
           class_it->second.end();
}

bool ClassTable::main_method_has_no_parameters() {
    Symbol main_class = idtable.add_string("Main");
    Symbol main_method = idtable.add_string("main");

    std::map<Symbol, std::map<Symbol, Feature> >::iterator
        class_it = class_methods.find(main_class);

    if (class_it == class_methods.end()) {
        return false;
    }

    std::map<Symbol, Feature>::iterator method_it =
        class_it->second.find(main_method);

    if (method_it == class_it->second.end()) {
        return false;
    }

    method_class* method_node =
        static_cast<method_class*>(method_it->second);

    Formals formals = method_node->get_formals();

    // An empty formal list has no first valid element.
    return !formals->more(formals->first());
}

Feature ClassTable::lookup_method(
    Symbol receiver_type,
    Symbol method_name,
    Symbol current_class
) {
    // SELF_TYPE refers to the current class.
    Symbol search_class =
        receiver_type == SELF_TYPE
            ? current_class
            : receiver_type;

    std::set<Symbol> visited;

    // Search the receiver class and then its ancestors.
    while (visited.insert(search_class).second) {
        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(search_class);

        if (class_it == class_map.end()) {
            break;
        }

        std::map<Symbol, std::map<Symbol, Feature> >::iterator
            methods_it = class_methods.find(search_class);

        if (methods_it != class_methods.end()) {
            std::map<Symbol, Feature>::iterator method_it =
                methods_it->second.find(method_name);

            if (method_it != methods_it->second.end()) {
                return method_it->second;
            }
        }

        // Continue searching in the parent class.
        class__class* class_node =
            static_cast<class__class*>(class_it->second);

        search_class = class_node->get_parent();
    }

    // Method not found in the inheritance hierarchy.
    return NULL;
}

Symbol ClassTable::least_upper_bound(
    Symbol first_type,
    Symbol second_type,
    Symbol current_class)
{
    // No_type is the bottom type.
    if (first_type == No_type) {
        return second_type;
    }

    if (second_type == No_type) {
        return first_type;
    }

    // Two SELF_TYPE values preserve SELF_TYPE.
    if (first_type == SELF_TYPE &&
        second_type == SELF_TYPE) {
        return SELF_TYPE;
    }

    // Otherwise, use the actual current class.
    if (first_type == SELF_TYPE) {
        first_type = current_class;
    }

    if (second_type == SELF_TYPE) {
        second_type = current_class;
    }

    // Collect ancestors of the first type.
    std::set<Symbol> ancestors;
    Symbol current = first_type;

    while (current != No_class &&
           ancestors.insert(current).second) {

        std::map<Symbol, Class_>::iterator it =
            class_map.find(current);

        if (it == class_map.end()) {
            break;
        }

        current = it->second->get_parent();
    }

    // Find the closest common ancestor.
    std::set<Symbol> visited;
    current = second_type;

    while (current != No_class &&
           visited.insert(current).second) {

        if (ancestors.count(current)) {
            return current;
        }

        std::map<Symbol, Class_>::iterator it =
            class_map.find(current);

        if (it == class_map.end()) {
            break;
        }

        current = it->second->get_parent();
    }

    // Recovery when no common ancestor is found.
    return Object;
}

void ClassTable::populate_object_environment(
    Symbol class_name,
    SymbolTable<Symbol, Symbol>& env)
{
    std::vector<Symbol> hierarchy;
    std::set<Symbol> visited;

    Symbol current = class_name;

    // Collect the inheritance chain.
    while (current != No_class &&
           visited.insert(current).second) {

        std::map<Symbol, Class_>::iterator class_it =
            class_map.find(current);

        if (class_it == class_map.end()) {
            break;
        }

        hierarchy.push_back(current);
        current = class_it->second->get_parent();
    }

    // Create the class scope.
    env.enterscope();

    // self is always available.
    env.addid(self, &SELF_TYPE);

    // Register inherited attributes before local attributes.
    for (std::vector<Symbol>::reverse_iterator it =
             hierarchy.rbegin();
         it != hierarchy.rend(); ++it) {

        std::map<Symbol, std::map<Symbol, Feature> >::iterator
            attrs_it = class_attributes.find(*it);

        if (attrs_it == class_attributes.end()) {
            continue;
        }

        for (std::map<Symbol, Feature>::iterator attr_it =
                 attrs_it->second.begin();
             attr_it != attrs_it->second.end(); ++attr_it) {

            attr_class* attribute =
                static_cast<attr_class*>(attr_it->second);

            // self cannot be replaced by an attribute.
            if (attr_it->first == self) {
                continue;
            }

            Symbol declared_type = attribute->get_type_decl();

            env.addid(
                attr_it->first,
                attribute->get_type_decl_ptr()
            );
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

static Symbol infer_expression_type(
    Expression expr,
    SymbolTable<Symbol, Symbol>& env,
    ClassTable* classtable,
    Symbol filename,
    Symbol current_class)
{
    // Identifier reference.
    object_class* object_node =
        dynamic_cast<object_class*>(expr);

    if (object_node != NULL) {
        Symbol identifier = object_node->get_name();
        Symbol* resolved_type = env.lookup(identifier);

        if (resolved_type == NULL) {
            classtable->semant_error(filename, object_node)
                << "Undeclared identifier "
                << identifier << "." << endl;

            expr->set_type(Object);
            return Object;
        }

        expr->set_type(*resolved_type);
        return *resolved_type;
    }

    // Integer constant.
    if (dynamic_cast<int_const_class*>(expr) != NULL) {
        expr->set_type(Int);
        return Int;
    }

    // Boolean constant.
    if (dynamic_cast<bool_const_class*>(expr) != NULL) {
        expr->set_type(Bool);
        return Bool;
    }

    // String constant.
    if (dynamic_cast<string_const_class*>(expr) != NULL) {
        expr->set_type(Str);
        return Str;
    }

    // Empty expression.
    if (dynamic_cast<no_expr_class*>(expr) != NULL) {
        expr->set_type(No_type);
        return No_type;
    }

    // Addition expression.
    plus_class* plus_node =
        dynamic_cast<plus_class*>(expr);

    if (plus_node != NULL) {
        Symbol left_type = infer_expression_type(
            plus_node->get_e1(),
            env,
            classtable,
            filename,
            current_class
        );

        Symbol right_type = infer_expression_type(
            plus_node->get_e2(),
            env,
            classtable,
            filename,
            current_class
        );

        // Both operands must be Int.
        if ((left_type != Int &&
            left_type != semantic_error_type()) ||
            (right_type != Int &&
            right_type != semantic_error_type())) {
            classtable->semant_error(filename, plus_node)
                << "Non-Int arguments to +: "
                << left_type << " + " << right_type
                << "." << endl;
        }

        // The result type of an addition is Int.
        expr->set_type(Int);
        return Int;
    }

    // Subtraction, multiplication and division.
    Expression left_expr = NULL;
    Expression right_expr = NULL;
    const char* operator_name = NULL;

    sub_class* sub_node =
        dynamic_cast<sub_class*>(expr);

    mul_class* mul_node =
        dynamic_cast<mul_class*>(expr);

    divide_class* divide_node =
        dynamic_cast<divide_class*>(expr);

    if (sub_node != NULL) {
        left_expr = sub_node->get_e1();
        right_expr = sub_node->get_e2();
        operator_name = "-";
    }
    else if (mul_node != NULL) {
        left_expr = mul_node->get_e1();
        right_expr = mul_node->get_e2();
        operator_name = "*";
    }
    else if (divide_node != NULL) {
        left_expr = divide_node->get_e1();
        right_expr = divide_node->get_e2();
        operator_name = "/";
    }

    if (operator_name != NULL) {
        Symbol left_type = infer_expression_type(
            left_expr, env, classtable, filename, current_class
        );

        Symbol right_type = infer_expression_type(
            right_expr, env, classtable, filename, current_class
        );

        if ((left_type != Int &&
            left_type != semantic_error_type()) ||
            (right_type != Int &&
            right_type != semantic_error_type())) {
            classtable->semant_error(filename, expr)
                << "Non-Int arguments to "
                << operator_name << ": "
                << left_type << " "
                << operator_name << " "
                << right_type << "." << endl;
        }

        expr->set_type(Int);
        return Int;
    }
    // Conditional expression: if ... then ... else ... fi.
    cond_class* cond_node =
        dynamic_cast<cond_class*>(expr);

    if (cond_node != NULL) {
        Symbol pred_type = infer_expression_type(
            cond_node->get_pred(), env, classtable, filename, current_class
        );

        Symbol then_type = infer_expression_type(
            cond_node->get_then_exp(), env, classtable, filename, current_class
        );

        Symbol else_type = infer_expression_type(
            cond_node->get_else_exp(), env, classtable, filename, current_class
        );

        if (pred_type != Bool) {
            classtable->semant_error(filename, cond_node)
                << "Predicate of if does not have type Bool."
                << endl;
        }

        Symbol result_type = classtable->least_upper_bound(
            then_type,
            else_type,
            current_class
        );

        expr->set_type(result_type);
        return result_type;
    }
    // While expression: while ... loop ... pool.
    loop_class* loop_node =
        dynamic_cast<loop_class*>(expr);

    if (loop_node != NULL) {
        Symbol pred_type = infer_expression_type(
            loop_node->get_pred(),
            env,
            classtable,
            filename,
            current_class
        );

        // Analyze the loop body, even if the predicate is invalid.
        infer_expression_type(
            loop_node->get_body(),
            env,
            classtable,
            filename,
            current_class
        );

        if (pred_type != Bool) {
            classtable->semant_error(filename, loop_node)
                << "Predicate of while does not have type Bool."
                << endl;
        }

        // A while expression always has type Object.
        expr->set_type(Object);
        return Object;
    }
    // Block expression: { expr1; expr2; ...; exprN; }
    block_class* block_node =
        dynamic_cast<block_class*>(expr);

    if (block_node != NULL) {
        Expressions expressions = block_node->get_body();

        Symbol result_type = Object;

        // Analyze all expressions in sequence.
        for (int i = expressions->first();
             expressions->more(i);
             i = expressions->next(i)) {

            result_type = infer_expression_type(
                expressions->nth(i),
                env,
                classtable,
                filename,
                current_class
            );
        }

        // The block type is the type of its last expression.
        expr->set_type(result_type);
        return result_type;
    }
    // Let expression: let identifier : Type <- init in body.
    let_class* let_node =
        dynamic_cast<let_class*>(expr);

    if (let_node != NULL) {
        Symbol identifier = let_node->get_identifier();

        // The initializer is evaluated in the outer scope.
        Symbol init_type = infer_expression_type(
            let_node->get_init(),
            env,
            classtable,
            filename,
            current_class
        );

        Symbol declared_type = let_node->get_type_decl();

        Symbol scoped_type = declared_type;

        // Validate the declared type.
        if (!classtable->is_defined_type(declared_type)) {
            classtable->semant_error(filename, let_node)
                << "Class " << declared_type
                << " of let-bound identifier " << identifier
                << " is undefined." << endl;
            scoped_type = semantic_error_type();
        }
        // Validate initializer type conformance.
        else if (!classtable->conforms(
                init_type,
                declared_type,
                current_class
            )) {

            classtable->semant_error(filename, let_node)
                << "Inferred type " << init_type
                << " of initialization of " << identifier
                << " does not conform to declared type "
                << declared_type << "." << endl;
        }

        // Create the local scope.
        env.enterscope();

        // self cannot be bound in a let expression.
        if (identifier == self) {
            classtable->semant_error(filename, let_node)
                << "'self' cannot be bound in a let expression."
                << endl;
        } else {
            env.addid(
                identifier,
                &scoped_type
            );
        }

        // Analyze the body with the local variable visible.
        Symbol body_type = infer_expression_type(
            let_node->get_body(),
            env,
            classtable,
            filename,
            current_class
        );

        // Close the local scope.
        env.exitscope();

        // A let expression has the type of its body.
        expr->set_type(body_type);
        return body_type;
    }
    // Case expression.
    typcase_class* case_node =
        dynamic_cast<typcase_class*>(expr);

    if (case_node != NULL) {
        // Analyze the expression being evaluated.
        infer_expression_type(
            case_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        Cases branches = case_node->get_cases();

        Symbol result_type = No_type;

        // Track declared branch types to detect duplicates.
        std::set<Symbol> declared_branch_types;

        // Analyze each branch in its own scope.
        for (int i = branches->first();
             branches->more(i);
             i = branches->next(i)) {

            branch_class* branch_node =
                static_cast<branch_class*>(branches->nth(i));

            // Each branch type must be unique within the case.
            Symbol branch_type = branch_node->get_type_decl();

            Symbol scoped_branch_type = branch_type;

            if (!declared_branch_types.insert(branch_type).second) {
                classtable->semant_error(filename, branch_node)
                    << "Duplicate branch type " << branch_type
                    << " in case expression." << endl;
            }

            // Validate the declared branch type.
            if (!classtable->is_defined_type(branch_type)) {
                classtable->semant_error(filename, branch_node)
                    << "Class " << branch_type
                    << " of case branch is undefined."
                    << endl;
                scoped_branch_type = semantic_error_type();
            }

            // SELF_TYPE cannot be used as a case branch type.
            if (branch_type == SELF_TYPE) {
                classtable->semant_error(filename, branch_node)
                    << "SELF_TYPE cannot be used as a case branch type."
                    << endl;

                scoped_branch_type = semantic_error_type();
            }

            env.enterscope();

            // self cannot be bound in a case branch.
            if (branch_node->get_name() == self) {
                classtable->semant_error(filename, branch_node)
                    << "'self' cannot be bound in a case branch."
                    << endl;
            } else {
                // Register the branch identifier.
                env.addid(
                    branch_node->get_name(),
                    &scoped_branch_type
                );
            }

            // Infer the type of the branch body.
            Symbol branch_result = infer_expression_type(
                branch_node->get_expr(),
                env,
                classtable,
                filename,
                current_class
            );

            env.exitscope();

            // Combine the types of all branches.
            result_type = classtable->least_upper_bound(
                result_type,
                branch_result,
                current_class
            );
        }

        expr->set_type(result_type);
        return result_type;
    }
    // Assignment expression: identifier <- expression.
    assign_class* assign_node =
        dynamic_cast<assign_class*>(expr);

    if (assign_node != NULL) {
        Symbol identifier = assign_node->get_name();

        // Analyze the expression on the right-hand side.
        Symbol assigned_type = infer_expression_type(
            assign_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        // Assignment to self is forbidden.
        if (identifier == self) {
            classtable->semant_error(filename, assign_node)
                << "Cannot assign to 'self'."
                << endl;
        } else {
            // Find the declared type of the identifier.
            Symbol* declared_type = env.lookup(identifier);

            if (declared_type == NULL) {
                classtable->semant_error(filename, assign_node)
                    << "Assignment to undeclared identifier "
                    << identifier << "." << endl;
            }
            else if (!classtable->conforms(
                    assigned_type,
                    *declared_type,
                    current_class
                )) {

                classtable->semant_error(filename, assign_node)
                    << "Type " << assigned_type
                    << " of assigned expression does not conform "
                    << "to declared type " << *declared_type
                    << " of identifier " << identifier
                    << "." << endl;
            }
        }

        // The assignment type is the right-hand expression type.
        expr->set_type(assigned_type);
        return assigned_type;
    }
    // Arithmetic negation: ~expression.
    neg_class* neg_node =
        dynamic_cast<neg_class*>(expr);

    if (neg_node != NULL) {
        Symbol operand_type = infer_expression_type(
            neg_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        if (operand_type != Int &&
            operand_type != semantic_error_type()) {
            classtable->semant_error(filename, neg_node)
                << "Argument of '~' has type "
                << operand_type << " instead of Int."
                << endl;
        }

        expr->set_type(Int);
        return Int;
    }
    // Logical negation: not expression.
    comp_class* comp_node =
        dynamic_cast<comp_class*>(expr);

    if (comp_node != NULL) {
        Symbol operand_type = infer_expression_type(
            comp_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        // Ignore a type error already reported in the operand.
        if (operand_type != Bool &&
            operand_type != semantic_error_type()) {

            classtable->semant_error(filename, comp_node)
                << "Argument of 'not' has type "
                << operand_type << " instead of Bool."
                << endl;
        }

        expr->set_type(Bool);
        return Bool;
    }
    // Isvoid expression: isvoid expression.
    isvoid_class* isvoid_node =
        dynamic_cast<isvoid_class*>(expr);

    if (isvoid_node != NULL) {
        // Analyze the operand, regardless of its type.
        infer_expression_type(
            isvoid_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        // The result of isvoid is always Bool.
        expr->set_type(Bool);
        return Bool;
    }
    // Less-than comparison: expression < expression.
    lt_class* lt_node =
        dynamic_cast<lt_class*>(expr);

    if (lt_node != NULL) {
        Symbol left_type = infer_expression_type(
            lt_node->get_e1(),
            env,
            classtable,
            filename,
            current_class
        );

        Symbol right_type = infer_expression_type(
            lt_node->get_e2(),
            env,
            classtable,
            filename,
            current_class
        );

        // Both operands must have type Int.
        if ((left_type != Int &&
            left_type != semantic_error_type()) ||
            (right_type != Int &&
            right_type != semantic_error_type())) {
            classtable->semant_error(filename, lt_node)
                << "Non-Int arguments: "
                << left_type << " < " << right_type
                << "." << endl;
        }

        // Comparisons always produce Bool.
        expr->set_type(Bool);
        return Bool;
    }
    // Less-than-or-equal comparison: expression <= expression.
    leq_class* leq_node =
        dynamic_cast<leq_class*>(expr);

    if (leq_node != NULL) {
        Symbol left_type = infer_expression_type(
            leq_node->get_e1(),
            env,
            classtable,
            filename,
            current_class
        );

        Symbol right_type = infer_expression_type(
            leq_node->get_e2(),
            env,
            classtable,
            filename,
            current_class
        );

        // Both operands must have type Int.
        if ((left_type != Int &&
            left_type != semantic_error_type()) ||
            (right_type != Int &&
            right_type != semantic_error_type())) {
            classtable->semant_error(filename, leq_node)
                << "Non-Int arguments: "
                << left_type << " <= " << right_type
                << "." << endl;
        }

        // Comparisons always produce Bool.
        expr->set_type(Bool);
        return Bool;
    }
    // Equality comparison: expression = expression.
    eq_class* eq_node =
        dynamic_cast<eq_class*>(expr);

    if (eq_node != NULL) {
        Symbol left_type = infer_expression_type(
            eq_node->get_e1(),
            env,
            classtable,
            filename,
            current_class
        );

        Symbol right_type = infer_expression_type(
            eq_node->get_e2(),
            env,
            classtable,
            filename,
            current_class
        );

        // Basic types require both operands to have the same type.
        bool has_basic_type =
            left_type == Int || left_type == Bool ||
            left_type == Str || right_type == Int ||
            right_type == Bool || right_type == Str;

        if (has_basic_type &&
            left_type != right_type &&
            left_type != semantic_error_type() &&
            right_type != semantic_error_type()) {
            classtable->semant_error(filename, eq_node)
                << "Illegal comparison with a basic type: "
                << left_type << " = " << right_type
                << "." << endl;
        }

        // Equality comparisons always produce Bool.
        expr->set_type(Bool);
        return Bool;
    }
    // Object creation: new Type.
    new__class* new_node =
        dynamic_cast<new__class*>(expr);

    if (new_node != NULL) {
        Symbol created_type = new_node->get_type_name();

        // The requested type must exist or be SELF_TYPE.
        if (!classtable->is_defined_type(created_type)) {
            classtable->semant_error(filename, new_node)
                << "'new' used with undefined class "
                << created_type << "." << endl;

            // Recovery type for subsequent semantic analysis.
            expr->set_type(Object);
            return Object;
        }

        expr->set_type(created_type);
        return created_type;
    }
    // Dynamic method dispatch: receiver.method(arguments).
    dispatch_class* dispatch_node =
        dynamic_cast<dispatch_class*>(expr);

    if (dispatch_node != NULL) {
        // Infer the receiver type.
        Symbol receiver_type = infer_expression_type(
            dispatch_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        // Analyze all arguments and preserve their inferred types.
        Expressions arguments = dispatch_node->get_actual();

        std::vector<Symbol> actual_types;

        for (int i = arguments->first();
            arguments->more(i);
            i = arguments->next(i)) {

            Symbol argument_type = infer_expression_type(
                arguments->nth(i),
                env,
                classtable,
                filename,
                current_class
            );

            actual_types.push_back(argument_type);
        }

        // The receiver already has an invalid type.
        // Avoid cascading errors from method lookup.
        if (receiver_type == semantic_error_type()) {
            expr->set_type(semantic_error_type());
            return semantic_error_type();
        }

        Symbol method_name = dispatch_node->get_name();

        // Search for the method in the inheritance hierarchy.
        Feature method_feature = classtable->lookup_method(
            receiver_type,
            method_name,
            current_class
        );

        if (method_feature == NULL) {
            classtable->semant_error(filename, dispatch_node)
                << "Dispatch to undefined method "
                << method_name << "." << endl;

            expr->set_type(Object);
            return Object;
        }

        method_class* method_node =
            static_cast<method_class*>(method_feature);

        // Validate the number of actual arguments.
        Formals formals = method_node->get_formals();

        int actual_count = 0;
        int formal_count = 0;

        for (int i = arguments->first();
             arguments->more(i);
             i = arguments->next(i)) {
            actual_count++;
        }

        for (int i = formals->first();
             formals->more(i);
             i = formals->next(i)) {
            formal_count++;
        }

        if (actual_count != formal_count) {
            classtable->semant_error(filename, dispatch_node)
                << "Method " << method_name
                << " called with wrong number of arguments."
                << endl;
        }

        // Validate argument types when the arity is correct.
        if (actual_count == formal_count) {
            int argument_index = 0;

            for (int i = formals->first();
                 formals->more(i);
                 i = formals->next(i)) {

                formal_class* formal_node =
                    static_cast<formal_class*>(formals->nth(i));

                Symbol expected_type =
                    formal_node->get_type_decl();

                Symbol actual_type =
                    actual_types[argument_index];

                if (!classtable->conforms(
                        actual_type,
                        expected_type,
                        current_class
                    )) {

                    classtable->semant_error(
                        filename,
                        dispatch_node
                    ) << "In call of method " << method_name
                      << ", type " << actual_type
                      << " of parameter " << argument_index + 1
                      << " does not conform to declared type "
                      << expected_type << "." << endl;
                }

                argument_index++;
            }
        }

        Symbol return_type = method_node->get_return_type();

        // SELF_TYPE follows the receiver type in dynamic dispatch.
        if (return_type == SELF_TYPE) {
            return_type = receiver_type;
        }

        expr->set_type(return_type);
        return return_type;
    }
    // Static method dispatch: receiver@Type.method(arguments).
    static_dispatch_class* static_node =
        dynamic_cast<static_dispatch_class*>(expr);

    if (static_node != NULL) {
        // Infer the receiver type.
        Symbol receiver_type = infer_expression_type(
            static_node->get_expr(),
            env,
            classtable,
            filename,
            current_class
        );

        // Analyze arguments and preserve their inferred types.
        Expressions arguments = static_node->get_actual();

        std::vector<Symbol> actual_types;

        for (int i = arguments->first();
             arguments->more(i);
             i = arguments->next(i)) {

            Symbol argument_type = infer_expression_type(
                arguments->nth(i),
                env,
                classtable,
                filename,
                current_class
            );

            actual_types.push_back(argument_type);
        }

        Symbol static_type = static_node->get_type_name();
        Symbol method_name = static_node->get_name();

        // SELF_TYPE cannot be an explicit dispatch type.
        if (static_type == SELF_TYPE) {
            classtable->semant_error(filename, static_node)
                << "Static dispatch to SELF_TYPE is not allowed."
                << endl;

            expr->set_type(Object);
            return Object;
        }

        // The explicit dispatch type must exist.
        if (!classtable->is_defined_type(static_type)) {
            classtable->semant_error(filename, static_node)
                << "Static dispatch to undefined class "
                << static_type << "." << endl;

            expr->set_type(Object);
            return Object;
        }

        // Receiver type must conform to the explicit type.
        if (!classtable->conforms(
                receiver_type,
                static_type,
                current_class
            )) {

            classtable->semant_error(filename, static_node)
                << "Expression type " << receiver_type
                << " does not conform to declared static "
                << "dispatch type " << static_type
                << "." << endl;
        }

        // Search from the explicitly specified class.
        Feature method_feature = classtable->lookup_method(
            static_type,
            method_name,
            current_class
        );

        if (method_feature == NULL) {
            classtable->semant_error(filename, static_node)
                << "Static dispatch to undefined method "
                << method_name << "." << endl;

            expr->set_type(Object);
            return Object;
        }

        method_class* method_node =
            static_cast<method_class*>(method_feature);

        // Validate the number of actual arguments.
        Formals formals = method_node->get_formals();

        int actual_count = 0;
        int formal_count = 0;

        for (int i = arguments->first();
             arguments->more(i);
             i = arguments->next(i)) {
            actual_count++;
        }

        for (int i = formals->first();
             formals->more(i);
             i = formals->next(i)) {
            formal_count++;
        }

        if (actual_count != formal_count) {
            classtable->semant_error(filename, static_node)
                << "Method " << method_name
                << " called with wrong number of arguments."
                << endl;
        }

        // Validate argument types when the arity is correct.
        if (actual_count == formal_count) {
            int argument_index = 0;

            for (int i = formals->first();
                 formals->more(i);
                 i = formals->next(i)) {

                formal_class* formal_node =
                    static_cast<formal_class*>(formals->nth(i));

                Symbol expected_type =
                    formal_node->get_type_decl();

                Symbol actual_type =
                    actual_types[argument_index];

                if (!classtable->conforms(
                        actual_type,
                        expected_type,
                        current_class
                    )) {

                    classtable->semant_error(
                        filename,
                        static_node
                    ) << "In call of method " << method_name
                      << ", type " << actual_type
                      << " of parameter " << argument_index + 1
                      << " does not conform to declared type "
                      << expected_type << "." << endl;
                }

                argument_index++;
            }
        }

        Symbol return_type = method_node->get_return_type();

        // SELF_TYPE resolves to the explicit dispatch type.
        if (return_type == SELF_TYPE) {
            return_type = static_type;
        }

        expr->set_type(return_type);
        return return_type;
    }
    // Every COOL expression class should be handled above.
    classtable->semant_error(filename, expr)
        << "Internal semantic error: unhandled expression node."
        << endl;

    // Recovery type to avoid propagating an untyped expression.
    expr->set_type(Object);
    return Object;
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

    ClassTable *classtable = new ClassTable(classes);

    // Every COOL program must define class Main
    // with its own main() method.
    if (!classtable->has_main_class()) {
        classtable->semant_error()
            << "Class Main is not defined." << endl;

    } else if (!classtable->has_main_method()) {
        classtable->semant_error()
            << "No 'main' method in class Main."
            << endl;

    } else if (!classtable->main_method_has_no_parameters()) {
        classtable->semant_error()
            << "'main' method in class Main should have no arguments."
            << endl;
    }

    // Stop if the class hierarchy or declarations are invalid.
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors."
             << endl;
        exit(1);
    }

    // Build the object environment for each user-defined class.
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ current_class = classes->nth(i);

        SymbolTable<Symbol, Symbol> object_env;

        classtable->populate_object_environment(
            current_class->get_name(),
            object_env
        );

        // Create a separate scope for each method.
        Features features = current_class->get_features();

        for (int j = features->first(); features->more(j);
             j = features->next(j)) {

            Feature feature = features->nth(j);

            // Analyze attribute initializers in the class environment.
            if (!feature->is_method()) {
                attr_class* attr_node =
                    static_cast<attr_class*>(feature);

                Symbol init_type = infer_expression_type(
                    attr_node->get_init(),
                    object_env,
                    classtable,
                    current_class->get_filename(),
                    current_class->get_name()
                );

                // No_type represents an attribute without initializer.
                if (init_type != No_type &&
                    !classtable->conforms(
                        init_type,
                        attr_node->get_type_decl(),
                        current_class->get_name()
                    )) {

                    classtable->semant_error(
                        current_class->get_filename(),
                        attr_node
                    ) << "Inferred type " << init_type
                      << " of initialization of attribute "
                      << attr_node->get_name()
                      << " does not conform to declared type "
                      << attr_node->get_type_decl()
                      << "." << endl;
                }

                continue;
            }

            method_class* method_node =
                static_cast<method_class*>(feature);

            // Method scope: formal parameters.
            object_env.enterscope();

            Formals formals = method_node->get_formals();

            for (int k = formals->first(); formals->more(k);
                 k = formals->next(k)) {

                formal_class* formal_node =
                    static_cast<formal_class*>(formals->nth(k));

                object_env.addid(
                    formal_node->get_name(),
                    formal_node->get_type_decl_ptr()
                );
            }

            // Infer the type of the method body.
            Symbol body_type = infer_expression_type(
                method_node->get_expr(),
                object_env,
                classtable,
                current_class->get_filename(),
                current_class->get_name()
            );

            // Check compatibility with the declared return type.
            if (body_type == No_type) {
                classtable->semant_error(
                    current_class->get_filename(),
                    method_node
                ) << "Method " << method_node->get_name()
                  << " has no inferred body type."
                  << endl;

            } else if (!classtable->conforms(
                    body_type,
                    method_node->get_return_type(),
                    current_class->get_name()
                )) {

                classtable->semant_error(
                    current_class->get_filename(),
                    method_node
                ) << "Inferred return type " << body_type
                  << " of method " << method_node->get_name()
                  << " does not conform to declared return type "
                  << method_node->get_return_type()
                  << "." << endl;
            }

            object_env.exitscope();
        }

        // Close the class scope.
        object_env.exitscope();
    }

    // Report errors detected while analyzing method bodies.
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors."
            << endl;
        exit(1);
    }
}


