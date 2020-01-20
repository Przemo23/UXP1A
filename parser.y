%{
	#include <stdio.h>
	#include "task.h"
	#include "log.h"
	#include "list.h"
	#include "builtins.h"

	int yylex();
	void yyerror(const char *s);
%}
%error-verbose
%debug


%union{
	int     int_type;
	char*	char_pointer_type;
	struct list_node * arguments_type;
}

%type <char_pointer_type>	text
%type <char_pointer_type>   assignment
%type <arguments_type>      text_sequence

%token REDIRECTION_IN
%token REDIRECTION_OUT
%token PIPE
%token EQUALS

%token PWD_CMD
%token ECHO_CMD
%token CD_CMD
%token EXPORT_CMD

%token <char_pointer_type> VARNAME
%token <char_pointer_type> WORD
%token <char_pointer_type> STRING
%token <char_pointer_type> STRING2

%%
input:
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
  	| built_in_operation PIPE task {
		log_trace("built_in_operation PIPE task");
	}
  	| task {
		log_trace("runTask();");
		run_task();
		free_process_list();

	}

task:
	text_sequence {
		char * s = list_convert_to_str($1);
		log_trace("Dodaje nowy proces do zadania:%s", s);
		free(s);
		add_process_to_task($1);
		list_free($1);
	}
  	| text_sequence PIPE task {
		char * s = list_convert_to_str($1);
		log_trace("Dodaje nowy proces do zadania:%s", s);
		free(s);
		add_process_to_task($1);
		list_free($1);
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
 		log_trace("Inicjuję liste:%s", $1);
		$$ = list_init($1);
	}
  	| text text_sequence {
		log_trace("Dodaje do listy:%s", $1);
		$$ = list_add($2, $1);
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
