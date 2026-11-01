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

class UnaryAssignTest {
    x : Int;
    y : Int;

    assignSimple() : Int {
        x <- 10
    };

    assignChain() : Int {
        x <- y <- 10
    };

    unaryNeg() : Int {
        ~5
    };

    logicalNot() : Bool {
        not true
    };

    checkVoid() : Bool {
        isvoid x
    };

    negPrecedence() : Int {
        ~2 * 3
    };

    notPrecedence() : Bool {
        not 2 < 3
    };
};

class ControlFlowTest {
    x : Int;

    simpleIf(flag : Bool) : Int {
        if flag then 10 else 20 fi
    };

    comparisonIf() : Int {
        if 2 < 3 then 1 else 0 fi
    };

    nestedIf() : Int {
        if true then
            if false then 1 else 2 fi
        else
            3
        fi
    };

    simpleWhile() : Object {
        while x < 10 loop x <- x + 1 pool
    };

    nestedWhile() : Object {
        while x < 10 loop
            if x < 5 then
                x <- x + 1
            else
                x <- x + 2
            fi
        pool
    };
};

class BlockTest {
    x : Int;
    y : Int;

    singleBlock() : Int {
        {
            42;
        }
    };

    multipleBlock() : Int {
        {
            x <- 10;
            y <- 20;
            x + y;
        }
    };

    nestedBlock() : Int {
        {
            x <- 1;
            {
                y <- 2;
                x + y;
            };
            x + y;
        }
    };

    blockInIf() : Int {
        if true then
            {
                x <- 10;
                x + 1;
            }
        else
            {
                x <- 20;
                x + 2;
            }
        fi
    };

    blockInWhile() : Object {
        while x < 10 loop
            {
                x <- x + 1;
                x;
            }
        pool
    };
};

class DispatchTest {
    sum(a : Int, b : Int) : Int {
        a + b
    };

    zero() : Int {
        0
    };

    identity(x : Int) : Int {
        x
    };

    callZero() : Int {
        zero()
    };

    callOne() : Int {
        identity(7)
    };

    callMany() : Int {
        sum(2, 3)
    };

    callExpression() : Int {
        sum(1 + 2, 3 * 4)
    };

    callNested() : Int {
        sum(identity(5), sum(2, 3))
    };
};

class ExplicitDispatchTest {
    zero() : Int {
        0
    };

    identity(x : Int) : Int {
        x
    };

    sum(a : Int, b : Int) : Int {
        a + b
    };

    getSelf() : SELF_TYPE {
        self
    };

    callEmpty() : Int {
        self.zero()
    };

    callOne() : Int {
        self.identity(7)
    };

    callOther(other : ExplicitDispatchTest) : Int {
        other.sum(2, 3)
    };

    callChain() : Int {
        self.getSelf().identity(5)
    };

    callExpression() : Int {
        (if true then self else self fi).sum(1 + 2, 3 * 4)
    };
};

class StaticDispatchTest inherits ExplicitDispatchTest {

    staticNoArgs() : String {
        self@Object.type_name()
    };

    staticWithArgs() : Int {
        self@ExplicitDispatchTest.sum(2, 3)
    };

    staticWithExpressions() : Int {
        self@ExplicitDispatchTest.sum(1 + 2, 3 * 4)
    };

    staticChain() : Int {
        self@ExplicitDispatchTest.getSelf().identity(5)
    };

    staticComplexReceiver() : Int {
        (if true then self else self fi)@ExplicitDispatchTest.sum(2, 3)
    };
};

class NewInitTest {
    number : Int <- 10;
    flag : Bool <- true;
    message : String <- "Hello";

    result : Int <- 2 + 3 * 4;

    instance : NewInitTest <- new NewInitTest;

    makeObject() : Object {
        new Object
    };

    makeSelf() : SELF_TYPE {
        new SELF_TYPE
    };

    makeInstance() : NewInitTest {
        new NewInitTest
    };
};

class LetTest {

    noInit() : Int {
        let x : Int in x
    };

    simpleInit() : Int {
        let x : Int <- 10 in x
    };

    multipleInit() : Int {
        let x : Int <- 10,
            y : Int <- x + 5
        in x + y
    };

    mixedInit() : Int {
        let x : Int,
            y : Int <- 20
        in y
    };

    nestedLet() : Int {
        let x : Int <- 3 in
            let y : Int <- 4 in
                x * y
    };

    bodyPrecedence() : Int {
        let x : Int <- 2 in x + 3 * 4
    };
};
