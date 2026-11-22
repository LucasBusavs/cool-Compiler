class A {
    answer() : Int {
        42
    };
};

class B inherits A {};

class Main {
    main() : Int {
        (new B).answer()
    };
};
