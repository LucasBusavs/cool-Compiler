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

%%

 /*
  *  Nested comments
  */


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
