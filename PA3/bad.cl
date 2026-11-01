
(*
 *  execute "coolc bad.cl" to see the error messages that the coolc parser
 *  generates
 *
 *  execute "myparser bad.cl" to see the error messages that your parser
 *  generates
 *)

(* no error *)
class A {
};

(* error:  b is not a type identifier *)
Class b inherits A {
};

(* error:  a is not a type identifier *)
Class C inherits a {
};

(* error:  keyword inherits is misspelled *)
Class D inherts A {
};

(* Recovery test: malformed attributes *)
class FeatureRecoveryTest {
    a : Int;

    brokenOne : Int <- ;

    b : Int;

    brokenTwo : String <- ;

    c : Int;

    validMethod() : Int { 10 };
};

(* Recovery test: malformed methods *)
class MethodRecoveryTest {
    before : Int;

    brokenSignature(x : ) : Int { 10 };

    validMethod() : Int { 20 };

    brokenBody() : Int { 2 + };

    after : Int;
};

(* Recovery test: malformed let declarations *)
class LetRecoveryTest {

    validBefore() : Int {
        let x : Int <- 10 in x
    };

    missingType() : Int {
        let x : Int <- 10,
            y : <- 20
        in x + 5
    };

    missingIdentifier() : Int {
        let : Int <- 3 in 7
    };

    validAfter() : Int {
        let z : Int <- 4 in z
    };
};

(* Recovery test: malformed block expressions *)
class BlockRecoveryTest {

    firstError() : Int {
        {
            1 + ;
            10;
        }
    };

    middleError() : Int {
        {
            10;
            2 * ;
            20;
        }
    };

    nestedError() : Int {
        {
            1;
            {
                3 + ;
                4;
            };
            5;
        }
    };

    validAfter() : Int {
        {
            7;
            8;
        }
    };
};

(* error: closing brace is missing *)
Class E inherits A {
;

