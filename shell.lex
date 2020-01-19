%{
#ifndef CALC_LEX_
#define CALC_LEX_
#include "header.h"
#include "tok.h"

#define yyterminate() return 0
void yyerror(const char *s);

%}
%option yywrap nounput 

digit		[0-9]
int_const	{digit}+
str			[a-zA-Z0-9_.\-/~$'"`]+
var			[a-zA-Z0-9_.\-/~$]+


%%
"+"		{ yylval.op_val = strdup(yytext); return PLUS; }
"="		{ yylval.op_val = strdup(yytext); return EQUALS; }
"*"		{ yylval.op_val = strdup(yytext); return MULT; }
"?"		{ yylval.op_val = strdup(yytext); return QMARK; }
":"		{ yylval.op_val = strdup(yytext); return COLON; }
"|"		{ yylval.op_val = strdup(yytext); return TILDE; }
">"		{ yylval.op_val = strdup(yytext); return OUTPUT; }
">>"	{ yylval.op_val = strdup(yytext); return OUTPUT_APPEND; }
"<"		{ yylval.op_val = strdup(yytext); return INPUT; }
"("		{ yylval.op_val = strdup(yytext); return LPARAM; }
")"		{ yylval.op_val = strdup(yytext); return RPARAM; }
"&"		{ yylval.op_val = strdup(yytext); return AMPERSAND; }

"cd" 	{ yylval.str = strdup(yytext); return CD_CMD; }
"pwd" 	{ yylval.str = strdup(yytext); return PWD_CMD; }
"echo" 	{ yylval.str = strdup(yytext); return ECHO_CMD; }
"touch"	{ yylval.str = strdup(yytext); return TOUCH_CMD; }
"rm" 	{ yylval.str = strdup(yytext); return RM_CMD; }
"mkdir"	{ yylval.str = strdup(yytext); return MKDIR_CMD; }
"rmdir"	{ yylval.str = strdup(yytext); return RMDIR_CMD; }
"cp" 	{ yylval.str = strdup(yytext); return CP_CMD; }
"exit" 	{ yylval.str = strdup(yytext); return EXIT_CMD; }

{int_const}	{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
{str}	{ yylval.str = strdup(yytext); return STRING; }

[;|\n]		{ yylineno++; return NEWLINE; }

.		{ /*printf("SCANNER "); yyerror("");*/	}

%%

#endif
