class A {};

class B inherits A {};

class Main {
    assign_parent(child : B, parent : A) : Object {
        child <- parent
    };

    main() : Object { 0 };
};
