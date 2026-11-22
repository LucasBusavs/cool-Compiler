class A {
    calculate(x : Int) : Int { x };
};

class B inherits A {
    calculate(x : Int) : String { "invalid" };
};

class Main inherits B {
    main() : Object { 0 };
};
