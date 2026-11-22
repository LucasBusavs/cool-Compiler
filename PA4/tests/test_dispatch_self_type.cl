class A {
    identity() : SELF_TYPE {
        self
    };
};

class B inherits A {};

class Main inherits B {
    child_return() : B {
        (new B).identity()
    };

    self_return() : SELF_TYPE {
        self.identity()
    };

    main() : Object { 0 };
};
