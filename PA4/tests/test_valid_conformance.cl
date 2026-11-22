class A {};

class B inherits A {};

class C inherits B {};

class Main inherits C {
    to_a(value : C) : A {
        value
    };

    to_b(value : C) : B {
        value
    };

    parent_self() : A {
        self
    };

    return_self() : SELF_TYPE {
        self
    };

    main() : Object {
        self
    };
};
