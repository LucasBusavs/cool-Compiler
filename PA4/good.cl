class C {
    a : Int <- 0;
    b : Bool <- false;

    init(x : Int, y : Bool) : SELF_TYPE {
        {
            a <- x;
            b <- y;
            self;
        }
    };

    calc(n : Int) : Int {
        (n + a) * 2 - 1
    };

    choose(flag : Bool) : Object {
        if flag then self else new C fi
    };
};

class D inherits C {
    label : String <- "ok";

    calc(n : Int) : Int {
        n + a
    };
};

class Main inherits IO {
    main() : Object {
        {
            let obj : C <- new D, n : Int <- 0 in {
                obj.init(2, true);
                obj.calc(3);
                (new D)@C.calc(2);

                if isvoid obj then
                    out_int(0)
                else
                    out_int(1)
                fi;

                case obj of
                    d : D => d.calc(1);
                    c : C => c.calc(2);
                esac;

                while n < 3 loop
                    n <- n + 1
                pool;

                not false;
                ~n;
                n <= 4;
                n = 3;
                "a" = "b";
                12 / 3;

                obj.choose(true);
            };

            self;
        }
    };
};
