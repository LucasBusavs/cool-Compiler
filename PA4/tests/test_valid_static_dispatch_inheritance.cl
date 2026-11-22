class Parent {};

class Child inherits Parent {};

class Service {
    identity(value : Parent) : Parent {
        value
    };
};

class DerivedService inherits Service {};

class Main {
    main() : Parent {
        (new DerivedService)@Service.identity(new Child)
    };
};
