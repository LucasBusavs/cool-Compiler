class A {
    sum(x : Int, y : Int) : Int {
        x + y
    };
};

class Main {
    main() : Object {
        (new A).sum(10, "hello")
    };
};
