(* Empty class with implicit inheritance from Object. *)
class A {
};

(* Explicit inheritance and case-insensitive class keyword. *)
Class BB__ inherits A {
};

(* Attributes without initialization use no_expr() in the AST. *)
class AttributeTest {
    counter : Int;
    title : String;
};


(* Methods with zero, one, and multiple formal parameters. *)
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


(* Integer, boolean, and string constants, including escaped newlines. *)
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


(* Arithmetic precedence, parentheses, left associativity, and comparisons. *)
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

(* Simple and chained assignments; unary operators and their precedence. *)
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

(* Simple and nested conditionals and loops. *)
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

(* Single-expression, multiple-expression, and nested blocks in control flow. *)
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

(* Implicit dispatch with empty, multiple, and nested arguments. *)
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

(* Explicit dispatch with chained calls and compound receivers. *)
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

(* Static dispatch with arguments, chained calls, and compound receivers. *)
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

(* Initialized attributes and object creation, including SELF_TYPE. *)
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

(* Let bindings with optional initialization, nesting, and body precedence. *)
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

(* Case expressions with one or multiple branches and nested cases. *)
class CaseTest {
    value : Int <- 7;

    oneBranch() : Int {
        case value of
            n : Int => n + 1;
        esac
    };

    multipleBranches() : Int {
        case value of
            n : Int => n + 1;
            s : String => 0;
            o : Object => 9;
        esac
    };

    complexExpression() : Int {
        case (if true then 5 else 8 fi) of
            n : Int => n * 2;
        esac
    };

    nestedCase() : Int {
        case value of
            a : Int =>
                case a of
                    b : Int => b + 1;
                esac;
        esac
    };
};
