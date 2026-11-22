class A {
    answer() : Int { 42 };
};

class Main {
    main() : Object {
        (new A)@A.missing()
    };
};
