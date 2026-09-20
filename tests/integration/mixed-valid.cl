class Main {
    text : String <- "class -- (* not a comment *)";
    -- "this is not a string" (* neither is this a block comment *)
    number : Int <- 123;

    (* outer comment
       "this is not a string"
       -- this is still part of the block comment
       (* nested comment *)
    *)

    flag : Bool <- true;
    message : String <- "line one\
line two";

    main() : SELF_TYPE {
        self
    };
};
