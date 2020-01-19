%{
#include <stdio.h>
int yylex();
void yyerror(const char *s);
#include <log.h>
%}
%error-verbose
%debug

/*TOKENS*/
%token REDIRECTION_IN
%token REDIRECTION_OUT
%token PIPE
%token EQUALS
%token SEMICOLON

%token PWD_CMD
%token ECHO_CMD
%token CD_CMD
%token EXPORT_CMD

%token VARNAME
%token WORD
%token STRING
%token STRING2

%%
input:
	| full_command 	{log_trace("full_command");}
	| full_command SEMICOLON {log_trace("full_command SEMICOLON");}

full_command:
  	assignment_sequence {log_trace("assignment_sequence");}
	| command  {log_trace("command");}
  	| command redirection {log_trace("command redirection");}

command:
	built_in_operation {log_trace("built_in_operation");}
  	| built_in_operation PIPE pipe {log_trace("built_in_operation PIPE pipe");}
  	| pipe {log_trace("pipe");}
q
pipe:
	external_call {log_trace("external_call");}
  	| external_call PIPE pipe {log_trace("external_call PIPE pipe");}

built_in_operation:
 	built_in_command text_sequence {log_trace("built_in_command text_sequence");}
  	| built_in_command {log_trace("built_in_command");}
  	| export_with_assignment {log_trace("export_with_assignment");}

external_call:
	text_sequence {log_trace("text_sequence");}

text_sequence:
 	text {log_trace("text");}
  	| text text_sequence {log_trace("text text_sequence");}

built_in_command:
 	PWD_CMD {log_trace("PWD_CMD");}
  	| ECHO_CMD {log_trace("ECHO_CMD");}
  	| CD_CMD{log_trace("CD_CMD");}
  	| EXPORT_CMD{log_trace("EXPORT_CMD");}

redirection:
	redirection_in {log_trace("redirection_in");}
	|redirection_out  {log_trace("redirection_out");}
	|redirection_inout  {log_trace("redirection_inout");}
	|redirection_outin  {log_trace("redirection_outin");}

redirection_in:
	REDIRECTION_IN text {log_trace("REDIRECTION_IN text");}

redirection_out:
	REDIRECTION_OUT text {log_trace("REDIRECTION_OUT text");}

redirection_inout:
	REDIRECTION_IN text REDIRECTION_OUT text {log_trace("redirection_in text REDIRECTION_OUT text");}

redirection_outin:
	REDIRECTION_OUT text REDIRECTION_IN text {log_trace("REDIRECTION_OUT text REDIRECTION_IN text");}

assignment_sequence:
 	assignment {log_trace("assignment");}
  	| assignment_sequence assignment {log_trace("assignment_sequence assignment");}

export_with_assignment:
 	EXPORT_CMD assignment {log_trace("EXPORT_CMD assignment");}

assignment:
 	VARNAME EQUALS WORD {log_trace("VARNAME EQUALS VARNAME");}

text:
	WORD {log_trace("WORD");}
	| VARNAME {log_trace("VARNAME");}
	| STRING {log_trace("STRING");}
	| STRING2 {log_trace("STRING2");}
%%
void yyerror (char const *s) {
   	extern char *yytext;
   	printf("ERROR: %s at %s \n", s, yytext);
 }
