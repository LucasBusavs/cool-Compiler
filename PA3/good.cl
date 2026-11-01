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
