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


class OperatorTest {
    precedence() : Int {
        2 + 3 * 4
    };

    parentheses() : Int {
        (2 + 3) * 4
    };

    leftAssoc() : Int {
        10 - 4 - 2
    };

    division() : Int {
        20 / 2 / 2
    };

    comparison() : Bool {
        2 + 3 <= 10
    };

    equality() : Bool {
        5 = 5
    };

    lessThan() : Bool {
        3 < 5
    };
};
