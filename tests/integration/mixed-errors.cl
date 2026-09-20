class Main {
    a : Int <- 1;

    # b : Int <- 2;

    *) c : Int <- 3;

    bad : String <- "unterminated
    d : Int <- 4;

    text : String <- "still -- not a comment";

    (* "not a string"
       (* nested comment *)
    *)

    flag : Bool <- false;
};
