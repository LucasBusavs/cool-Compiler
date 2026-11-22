class A {
    calculate(x : Int, y : Int) : Int { x + y };
};

class B inherits A {
    calculate(x : Int) : Int { x };
};

class Main inherits B {
    main() : Object { 0 };
};
