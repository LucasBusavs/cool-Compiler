#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"
#include <map>

using std::ostream;

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  std::map<Symbol, Class_> class_map;

  std::map<Symbol, std::map<Symbol, Feature> > class_attributes;
  std::map<Symbol, std::map<Symbol, Feature> > class_methods;
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  void populate_object_environment(
    Symbol class_name,
    SymbolTable<Symbol, Symbol>& env
  );

  bool conforms(
    Symbol actual_type,
    Symbol expected_type,
    Symbol current_class
  );

  bool is_defined_type(Symbol type);

  bool has_main_class();

  bool has_main_method();

  bool main_method_has_no_parameters();

  Feature lookup_method(
    Symbol receiver_type,
    Symbol method_name,
    Symbol current_class
  );

  Symbol least_upper_bound(
    Symbol first_type,
    Symbol second_type,
    Symbol current_class
  );
};


#endif

