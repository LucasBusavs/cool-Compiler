class C {
    a : Int <- 0;

    init(x : Int, y : Bool) : SELF_TYPE {
        {
            a <- x;
            self;
        }
    };

    calc(n : Int) : Int {
        n + a
    };
};

class Main inherits IO {
    main() : Object {
        {
            -- Incorrect argument type
            (new C).init(1, 1);

            -- Incorrect number of arguments
            (new C).init(1, true, 3);

            -- Undefined method
            (new C).missing(1);

            -- Undeclared identifier
            unknown;

            -- Invalid arithmetic operands
            1 + true;

            -- Invalid logical negation
            not 1;

            -- Invalid arithmetic negation
            ~false;

            -- Invalid if predicate
            if 1 then 0 else 1 fi;

            -- Invalid while predicate
            while 1 loop 0 pool;

            -- Incompatible let initializer
            let n : Int <- "wrong" in n;

            -- Duplicate case branch type
            case 0 of
                x : Int => x;
                y : Int => y + 1;
            esac;

            -- Invalid static dispatch receiver
            (new C)@IO.out_int(1);

            -- Instantiation of an undefined class
            new MissingType;

            -- Assignment to self
            self <- new Main;

            self;
        }
    };
};
