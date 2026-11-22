class A {};

class B inherits A {};

class Main {
    assign_child(parent : A, child : B) : B {
        parent <- child
    };

    main() : Object { 0 };
};
