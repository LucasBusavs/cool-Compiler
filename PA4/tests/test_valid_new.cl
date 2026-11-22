class A {};

class Main {
    create_a() : A {
        new A
    };

    create_self() : SELF_TYPE {
        new SELF_TYPE
    };

    main() : Object { 0 };
};
