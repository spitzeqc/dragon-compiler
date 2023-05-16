%{
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "structures/tree.h"
#include "structures/hashmap.h"

#include "dragon.tab.h"

int yylex(void);
int yyerror(char* s);
%}

identifier	[a-zA-Z][a-zA-Z0-9]*
number		[0-9]+
string		\".*\"

whitespace	[ \t]+
commentA	(\{.*\})
commentB	(\(\*.*\*\))
%%

{commentA}	{ ; }
{commentB}	{ ; }

"def"		{ fprintf( stderr, "[DEF]" ); return DEF; }
"var"		{ fprintf( stderr, "[VAR]" ); return VAR; }
"array"		{ fprintf( stderr, "[ARRAY]" ); return ARRAY; }
"of"		{ fprintf( stderr, "[OF]" ); return OF; }
"integral"	{ fprintf( stderr, "[INTEGRAL]" ); return INTEGRAL; }
"rational"	{ fprintf( stderr, "[RATIONAL]" ); return RATIONAL; }
"string"	{ fprintf( stderr, "[STRING]" ); return STRING; }
"begin"		{ fprintf( stderr, "[BEGIN]" ); return BBEGIN; }
"end"		{ fprintf( stderr, "[END]" ); return END; }
"if"		{ fprintf( stderr, "[IF]" ); return IF; }
"then"		{ fprintf( stderr, "[THEN]" ); return THEN; }
"else"		{ fprintf( stderr, "[ELSE]" ); return ELSE; }
"while"		{ fprintf( stderr, "[WHILE]" ); return WHILE; }
"do"		{ fprintf( stderr, "[DO]" ); return DO; }
"repeat"	{ fprintf( stderr, "[REPEAT]" ); return REPEAT; }
"until"		{ fprintf( stderr, "[UNTIL]" ); return UNTIL; }
"for"		{ fprintf( stderr, "[FOR]" ); return FOR; }
"func"		{ fprintf( stderr, "[FUNC]" ); return FUNC; }
"proc"		{ fprintf( stderr, "[PROC]" ); return PROC; }
"print"		{ fprintf( stderr, "[PRINT]" ); return PRINT; }
"scan"		{ fprintf( stderr, "[SCAN]" ); return SCAN; }

"or"		{ /* fprintf( stderr, "[ADDOP:%s]", yytext ); yylval.opval = OR;    return ADDOP; */ fprintf( stderr, "[OR]" ); return OR; }
"div"		{ fprintf( stderr, "[MULOP:%s]", yytext ); yylval.opval = DIV;   return MULOP; }
"mod"		{ fprintf( stderr, "[MULOP:%s]", yytext ); yylval.opval = MOD;   return MULOP; }
"and"		{ /* fprintf( stderr, "[MULOP:%s]", yytext ); yylval.opval = AND;   return MULOP;*/ fprintf( stderr, "[AND]" ); return AND; }

"<>"		{ fprintf( stderr, "[NOT]" ); return NOT; }
"="		{ fprintf( stderr, "[RELOP:%s]", yytext ); yylval.opval = EQ; return RELOP; }
"<="		{ fprintf( stderr, "[RELOP:%s]", yytext ); yylval.opval = LE; return RELOP; }
"<"		{ fprintf( stderr, "[RELOP:%s]", yytext ); yylval.opval = LT; return RELOP; }
">="		{ fprintf( stderr, "[RELOP:%s]", yytext ); yylval.opval = GE; return RELOP; }
">"		{ fprintf( stderr, "[RELOP:%s]", yytext ); yylval.opval = GT; return RELOP; }

"+"		{ fprintf( stderr, "[ADDOP:%s]", yytext ); yylval.opval = PLUS;  return ADDOP; }
"-"		{ fprintf( stderr, "[ADDOP:%s]", yytext ); yylval.opval = MINUS; return ADDOP; }

"*"		{ fprintf( stderr, "[MULOP:%s]", yytext ); yylval.opval = STAR;  return MULOP; }
"/"		{ fprintf( stderr, "[MULOP:%s]", yytext ); yylval.opval = SLASH; return MULOP; }

"("		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
")"		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
";"		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
","		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
"."		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
"["		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
"]"		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
":="		{ fprintf( stderr, "[ASSIGNOP]" ); return ASSIGNOP; }
":"		{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }
\n		{ fprintf( stderr, "\n" ); }

{identifier}	{ fprintf( stderr, "[ID:%s]", yylval.sval = (char *)strdup(yytext) ); return ID; }

{string}	{ fprintf( stderr, "[STR_VAL:%s]", yylval.sval = (char *)strdup(yytext) ); return STR_VAL; }
{number}	{ fprintf( stderr, "[INUM:%d]", yylval.ival = atoi(yytext) ); return INUM; }
{number}\.{number} { fprintf( stderr, "[RNUM:%f]", yylval.rval = atof(yytext) ); return RNUM; }
{whitespace}	{ ; }

.		{ fprintf( stderr, "<%c>", yytext[0] ); return yytext[0]; }

%%

int yyerror (char *message) {
	fprintf (stderr, "ERROR: %s\n", message );
	exit(1);
}
