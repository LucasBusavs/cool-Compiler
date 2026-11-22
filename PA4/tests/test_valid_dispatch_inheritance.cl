class Parent {};

class Child inherits Parent {};

class Service {
    identity(value : Parent) : Parent {
        value
    };
};

class Main {
    main() : Parent {
        (new Service).identity(new Child)
    };
};
