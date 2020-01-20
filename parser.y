%{
	#include <stdio.h>
	#include "log.h"
	#include "arg_list.h"
	#include "builtins.h"

	int yylex();
	void yyerror(const char *s);
%}
%error-verbose
%debug


%union{
	int     int_type;
	char*	char_pointer_type;
	struct arg_node * arguments_type;
}

%type <char_pointer_type>	text
%type <char_pointer_type>   assignment

%token REDIRECTION_IN
%token REDIRECTION_OUT
%token PIPE
%token EQUALS
%token SEMICOLON

%token PWD_CMD
%token ECHO_CMD
%token CD_CMD
%token EXPORT_CMD
%token EXIT_CMD

%token <char_pointer_type> VARNAME
%token <char_pointer_type> WORD
%token <char_pointer_type> STRING
%token <char_pointer_type> STRING2

%%
input:
	| full_command 	{
		log_trace("full_command");
	}
	| full_command SEMICOLON {
		log_trace("full_command SEMICOLON");
	}

full_command:
  	assignment_sequence {
		log_trace("assignment_sequence");
	}
	| command {
		log_trace("command");
	}
  	| command redirection {
		log_trace("command redirection");
	}

command:
	built_in_operation {
		log_trace("built_in_operation");
	}
  	| built_in_operation PIPE pipe {
		log_trace("built_in_operation PIPE pipe");
	}
  	| pipe {
		log_trace("pipe");
	}

pipe:
	text_sequence {
		log_trace("text_sequence");
	}
  	| text_sequence PIPE pipe {
		log_trace("text_sequence PIPE pipe");
	}

built_in_operation:
  	PWD_CMD {
		log_trace("pwd();");
		pwd_cmd();
	}
  	| ECHO_CMD text	{
		log_trace("echo(%s);", $2);
		echo_cmd($2);
	}
  	| CD_CMD text {
		log_trace("cd(%s);", $2);
		cd_cmd($2);
	}
	| EXIT_CMD {
		log_trace("exit");
		exit_cmd();
	}
  	| EXPORT_CMD {
		log_trace("export();");
		// TODO
	}
	| EXPORT_CMD assignment {
		log_trace("export(%s)",$2);
		// TODO
	}

text_sequence:
 	text {
		log_trace("text");
	}
  	| text text_sequence {
		log_trace("text text_sequence");
	}

redirection:
	REDIRECTION_IN text {
		log_trace("REDIRECTION_IN text");
	}
	|REDIRECTION_OUT text {
		log_trace("REDIRECTION_OUT text");
	}
	|REDIRECTION_IN text REDIRECTION_OUT text {
		log_trace("redirection_in text REDIRECTION_OUT text");
	}
	|REDIRECTION_OUT text REDIRECTION_IN text {
		log_trace("REDIRECTION_OUT text REDIRECTION_IN text");
	}

assignment_sequence:
 	assignment {
		log_trace("assignment");
	}
  	| assignment_sequence assignment {
		log_trace("assignment_sequence assignment");
	}

assignment:
 	VARNAME EQUALS text {
		log_trace("set_variable(%s,%s)",$1,$3);
		set_variable($1,$3);
		$$=$1;
	}

text:
	WORD {
		log_trace("WORD"); $$ = $1;
	}
	| VARNAME {
		log_trace("VARNAME %s", $1); $$ = $1;
	}
	| STRING {
		log_trace("STRING"); $$ = $1;
	}
	| STRING2 {
		log_trace("STRING2"); $$ = $1;
	}
%%

void yyerror (char const *s) {
   	extern char *yytext;
   	printf("ERROR: %s at %s \n", s, yytext);
 }
