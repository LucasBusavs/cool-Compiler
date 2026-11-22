class A {
    calculate(x : Int, y : String) : Int { x };
};

class B inherits A {
    calculate(x : String, y : Int) : Int { y };
};

class Main inherits B {
    main() : Object { 0 };
};
