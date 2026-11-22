class A {};

class B inherits A {};

class C inherits A {};

class Main {
    choose(value : A) : A {
        case value of
            b : B => b;
            c : C => c;
        esac
    };

    main() : Object { 0 };
};
