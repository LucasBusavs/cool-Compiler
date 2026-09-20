class Main inherits IO {
    -- Método principal que inicia a execução do programa
    main() : Object {
        let n : Int <- 7 in {
            out_string("O fatorial de ");
            out_int(n);
            out_string(" eh: ");
            out_int(fatorial(n));
            out_string("\n");
        }
    };

    -- Método recursivo para calcular o fatorial
    fatorial(num : Int) : Int {
        if num <= 1 then
            1
        else
            num * fatorial(num - 1)
        fi
    };
};
