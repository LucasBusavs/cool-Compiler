class A {
    calculate(x : Int, y : Int) : Int {
        x + y
    };
};

class B inherits A {
    calculate(a : Int, b : Int) : Int {
        a * b
    };
};

class Main inherits B {
    main() : Object { 0 };
};
