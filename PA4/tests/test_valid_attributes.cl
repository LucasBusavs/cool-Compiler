class A {
    x : Int;
};

class B inherits A {
    y : String;
};

class C inherits B {
    z : Bool;
};

class Main inherits C {
    main() : Object { 0 };
};
