class A {
    identity() : SELF_TYPE {
        self
    };
};

class B inherits A {};

class Main {
    main() : A {
        (new B)@A.identity()
    };
};
