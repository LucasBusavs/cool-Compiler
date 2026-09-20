(*
 * COOL Lexer Test File
 *
 * Representative tests for:
 * - keywords and case-insensitivity
 * - boolean constants
 * - TYPEID and OBJECTID
 * - integer constants
 * - operators and punctuation
 * - whitespace and line counting
 * - line and nested block comments
 * - string constants and escapes
 * - recoverable lexical errors
 *
 * Additional edge cases such as EOF inside strings/comments,
 * real NUL bytes and exact string-length boundaries are tested
 * separately in tests/.
 *)


-- ============================================================
-- Keywords
-- ============================================================

class else fi if in inherits isvoid let loop pool then while
case esac new of not

CLASS Else FI If IN InHeRiTs IsVoId LET Loop Pool Then WHILE
Case ESAC New OF Not


-- ============================================================
-- Boolean constants
-- ============================================================

true
false
tRuE
fAlSe

-- These are identifiers, not BOOL_CONST, because the first
-- character is uppercase.
True
FALSE


-- ============================================================
-- Identifiers
-- ============================================================

Object
Int
Bool
String
SELF_TYPE

Main
MyClass
Type_123

self
main
my_variable
object123


-- ============================================================
-- Integer constants
-- ============================================================

0
1
007
123
999999

-- No overflow checking is performed by the scanner.
1234567890123456789012345678901234567890


-- ============================================================
-- Operators and punctuation
-- ============================================================

<- <= =>

+ - * / ~ <
= ( ) { } ; : , . @


-- ============================================================
-- Longest-match / token boundaries
-- ============================================================

class classy Class ClassName
true trueValue false false_1
classx ifx whilex newer notebook


-- ============================================================
-- Line comments
-- ============================================================

-- Everything here must be ignored: class Main "string" (* comment *)

class Main


-- ============================================================
-- Nested block comments
-- ============================================================

(*
   Outer block comment.

   class Fake {
       text : String <- "not a real string";
   };

   (* nested block comment
      (* nested again *)
   *)

   -- still inside the block comment
*)

Main


-- ============================================================
-- String constants
-- ============================================================

""
"a"
"hello"
"hello world"
"123 !? @"

"class -- (* not a comment *) true <- <= =>"

"quote: \""
"backslash: \\"
"zero textual escape: \0"
"generic escape: \x"

"tab:\tend"
"newline:\nend"
"backspace:\bend"
"formfeed:\fend"


-- ============================================================
-- Escaped physical newline
-- ============================================================

"line one\
line two"

Main


-- ============================================================
-- Recoverable errors
-- ============================================================

-- Invalid character: scanner must return ERROR and continue.
#
main

-- Unmatched block-comment terminator.
*)
Main

-- Physical unescaped newline inside string.
"unterminated
Main


-- ============================================================
-- Interaction between constructs
-- ============================================================

class Integration {
    text : String <- "class -- (* still string *)";

    -- "not a string"
    value : Int <- 123;

    (* comment containing:
       "fake string"
       -- fake line comment
       (* nested comment *)
    *)

    flag : Bool <- true;

    message : String <- "first line\
second line";

    main() : SELF_TYPE {
        self
    };
};
