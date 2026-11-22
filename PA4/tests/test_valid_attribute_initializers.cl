class Base {
    base : Int <- 7;
};

class Main inherits Base {
    derived : Int <- base + 3;
    message : String;
    enabled : Bool <- true;

    main() : Object {
        derived
    };
};
