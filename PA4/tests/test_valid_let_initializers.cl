class Main {
    with_init() : Int {
        let x : Int <- 5 in x
    };

    without_init() : Int {
        let x : Int in x
    };

    self_init() : Object {
        let x : Object <- self in x
    };

    main() : Object { 0 };
};
