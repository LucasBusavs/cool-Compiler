class A {
};

Class BB__ inherits A {
};

class AttributeTest {
    counter : Int;
    title : String;
};


class MethodTest {
    getValue() : Int {
        42
    };

    identity(value : Int) : Int {
        value
    };

    chooseFirst(a : Int, b : Int) : Int {
        a
    };
};


class LiteralTest {
    getTrue() : Bool {
        true
    };

    getFalse() : Bool {
        false
    };

    getText() : String {
        "Hello"
    };

    getEscaped() : String {
        "Line1\nLine2"
    };
};
