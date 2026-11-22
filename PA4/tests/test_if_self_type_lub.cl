class A {};

class Main inherits A {
    same_self() : SELF_TYPE {
        if true then self else self fi
    };

    mixed_self(parent : A) : A {
        if true then self else parent fi
    };

    main() : Object {
        self
    };
};
