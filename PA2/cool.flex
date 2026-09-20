/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

/*
 * Buffer persistente usado para reportar um caractere inválido.
 * cool_yylval.error_msg armazena um ponteiro para esta memória.
 */
char invalid_char_error[2];

/*
 * Current nesting depth of block comments.
 */
int comment_depth = 0;

/*
 * Appends one character while preserving space for the final '\0'.
 * COOL strings may contain at most 1024 characters.
 */
static bool append_string_char(char c)
{
        if (string_buf_ptr - string_buf >= MAX_STR_CONST - 1) {
                return false;
        }

        *string_buf_ptr++ = c;
        return true;
}

%}

/*
 * Define names for regular expressions here.
 */

/* Multiple-character operators */
ASSIGN          <-
LE              <=
DARROW          =>

/* Boolean constants */
TRUE            t[rR][uU][eE]
FALSE           f[aA][lL][sS][eE]

/* Keywords: case-insensitive */
KW_CLASS        [cC][lL][aA][sS][sS]
KW_ELSE         [eE][lL][sS][eE]
KW_FI           [fF][iI]
KW_IF           [iI][fF]
KW_IN           [iI][nN]
KW_INHERITS     [iI][nN][hH][eE][rR][iI][tT][sS]
KW_ISVOID       [iI][sS][vV][oO][iI][dD]
KW_LET          [lL][eE][tT]
KW_LOOP         [lL][oO][oO][pP]
KW_POOL         [pP][oO][oO][lL]
KW_THEN         [tT][hH][eE][nN]
KW_WHILE        [wW][hH][iI][lL][eE]
KW_CASE         [cC][aA][sS][eE]
KW_ESAC         [eE][sS][aA][cC]
KW_NEW          [nN][eE][wW]
KW_OF           [oO][fF]
KW_NOT          [nN][oO][tT]

/* Constants and identifiers */
INT_CONST       [0-9]+
TYPEID          [A-Z][A-Za-z0-9_]*
OBJECTID        [a-z][A-Za-z0-9_]*

/* Whitespace */
NEWLINE         \n
WHITESPACE      [ \t\f\r\v]+

/* One-character tokens */
SIMPLE_TOKEN    [-+*/~<={}();:,.@]

/*
 * Exclusive state used while scanning block comments.
 */
%x COMMENT
%x STRING
%x STRING_RECOVERY

%%

 /*
  *  Nested comments
  */

 /*
  * Line comments.
  *
  * The newline is intentionally left for the normal NEWLINE rule,
  * which is responsible for updating curr_lineno.
  */

"--"[^\n]* {
        /* Ignore line comment contents. */
}


 /*
  * Block comments.
  */

"(*" {
        comment_depth = 1;
        BEGIN(COMMENT);
}

<COMMENT>"(*" {
        comment_depth++;
}

<COMMENT>"*)" {
        comment_depth--;

        if (comment_depth == 0) {
                BEGIN(INITIAL);
        }
}

<COMMENT>\n {
        curr_lineno++;
}

<COMMENT><<EOF>> {
        BEGIN(INITIAL);
        cool_yylval.error_msg = (char *)"EOF in comment";
        return (ERROR);
}

<COMMENT>. {
        /* Ignore ordinary characters inside a block comment. */
}


 /*
  * Unmatched block-comment terminator.
  */

"*)" {
        cool_yylval.error_msg = (char *)"Unmatched *)";
        return (ERROR);
}

 /*
  *  The multiple-character operators.
  */

{ASSIGN}        { return (ASSIGN); }
{LE}            { return (LE); }
{DARROW}        { return (DARROW); }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */

 /*
  * Boolean constants.
  *
  * true and false require a lowercase first letter.
  */

{TRUE} {
        cool_yylval.boolean = true;
        return (BOOL_CONST);
}

{FALSE} {
        cool_yylval.boolean = false;
        return (BOOL_CONST);
}


 /*
  * Keywords.
  */

{KW_CLASS}      { return (CLASS); }
{KW_ELSE}       { return (ELSE); }
{KW_FI}         { return (FI); }
{KW_IF}         { return (IF); }
{KW_IN}         { return (IN); }
{KW_INHERITS}   { return (INHERITS); }
{KW_ISVOID}     { return (ISVOID); }
{KW_LET}        { return (LET); }
{KW_LOOP}       { return (LOOP); }
{KW_POOL}       { return (POOL); }
{KW_THEN}       { return (THEN); }
{KW_WHILE}      { return (WHILE); }
{KW_CASE}       { return (CASE); }
{KW_ESAC}       { return (ESAC); }
{KW_NEW}        { return (NEW); }
{KW_OF}         { return (OF); }
{KW_NOT}        { return (NOT); }

 /*
  * Integer constants.
  */

{INT_CONST} {
        cool_yylval.symbol = inttable.add_string(yytext);
        return (INT_CONST);
}


 /*
  * Type and object identifiers.
  */

{TYPEID} {
        cool_yylval.symbol = idtable.add_string(yytext);
        return (TYPEID);
}

{OBJECTID} {
        cool_yylval.symbol = idtable.add_string(yytext);
        return (OBJECTID);
}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */

 /*
  * Begin a string constant.
  */

\" {
        string_buf_ptr = string_buf;
        BEGIN(STRING);
}


 /*
  * End a valid string constant.
  */

<STRING>\" {
        *string_buf_ptr = '\0';

        cool_yylval.symbol = stringtable.add_string(string_buf);

        BEGIN(INITIAL);

        return (STR_CONST);
}


 /*
  * Ordinary characters inside a string.
  *
  * Escapes, newlines and error cases will be implemented separately.
  */

 /*
  * Standard escape sequences.
  */

 /*
  * Escaped physical newline.
  *
  * A backslash followed by an actual newline keeps the string open.
  * The newline becomes part of the string value.
  */

<STRING>\\\n {
        curr_lineno++;

        if (!append_string_char('\n')) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

<STRING>\\b {
        if (!append_string_char('\b')) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

<STRING>\\t {
        if (!append_string_char('\t')) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

<STRING>\\n {
        if (!append_string_char('\n')) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

<STRING>\\f {
        if (!append_string_char('\f')) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

 /*
  * Generic escape.
  *
  * Any escaped character other than b, t, n and f evaluates
  * to the character itself.
  */

 /*
  * A real NUL byte is invalid inside a string.
  * This is different from the textual escape \0.
  */

<STRING>\\\x00 {
        BEGIN(STRING_RECOVERY);
        cool_yylval.error_msg =
                (char *)"String contains null character";
        return (ERROR);
}

<STRING>\x00 {
        BEGIN(STRING_RECOVERY);
        cool_yylval.error_msg =
                (char *)"String contains null character";
        return (ERROR);
}

<STRING>\\. {
        if (!append_string_char(yytext[1])) {
                BEGIN(STRING_RECOVERY);
                cool_yylval.error_msg =
                        (char *)"String constant too long";
                return (ERROR);
        }
}

<STRING>[^"\\\n\x00]+ {
        for (int i = 0; i < (int)yyleng; i++) {
                if (!append_string_char(yytext[i])) {
                        BEGIN(STRING_RECOVERY);
                        cool_yylval.error_msg =
                                (char *)"String constant too long";
                        return (ERROR);
                }
        }
}

 /*
  * Unescaped physical newline.
  *
  * This terminates the invalid string and resumes scanning
  * at the beginning of the next source line.
  */

<STRING>\n {
        curr_lineno++;

        BEGIN(INITIAL);

        cool_yylval.error_msg = (char *)"Unterminated string constant";

        return (ERROR);
}

 /*
  * EOF reached before the closing quote.
  */

<STRING><<EOF>> {
        BEGIN(INITIAL);

        cool_yylval.error_msg = (char *)"EOF in string constant";

        return (ERROR);
}

 /*
  * Recovery after an invalid string.
  *
  * The original error has already been returned. From this point on,
  * consume the remaining contents until the logical end of the string.
  */

<STRING_RECOVERY>\\\n {
        curr_lineno++;
}

<STRING_RECOVERY>\\. {
        /* Ignore escaped character. */
}

<STRING_RECOVERY>\" {
        BEGIN(INITIAL);
}

<STRING_RECOVERY>\n {
        curr_lineno++;
        BEGIN(INITIAL);
}

<STRING_RECOVERY>\x00 {
        /* Ignore additional NUL bytes during recovery. */
}

<STRING_RECOVERY><<EOF>> {
        BEGIN(INITIAL);
        return 0;
}

<STRING_RECOVERY>. {
        /* Ignore remaining characters of the invalid string. */
}

 /*
  * Line counting and whitespace.
  */

{NEWLINE} {
        curr_lineno++;
}

{WHITESPACE} {
        /* Ignore whitespace other than newline. */
}

 /*
  * Single-character tokens.
  */

{SIMPLE_TOKEN} {
        return (yytext[0]);
}


 /*
  * Invalid character.
  *
  * This rule must remain last in INITIAL so that every otherwise
  * unrecognized character is reported as a lexical error.
  */

. {
        invalid_char_error[0] = yytext[0];
        invalid_char_error[1] = '\0';

        cool_yylval.error_msg = invalid_char_error;

        return (ERROR);
}

%%
