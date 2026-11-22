class A {};

class B inherits A {};

class C inherits A {};

class Main inherits B {
    ancestor_to_child(value : A) : B {
        value
    };

    sibling_to_child(value : C) : B {
        value
    };

    regular_to_self_type(value : Main) : SELF_TYPE {
        value
    };

    main() : Object {
        self
    };
};
