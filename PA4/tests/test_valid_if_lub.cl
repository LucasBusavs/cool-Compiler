class A {};

class B inherits A {};

class C inherits A {};

class Main {
    choose(flag : Bool, left : B, right : C) : A {
        if flag then left else right fi
    };

    main() : Object { 0 };
};
