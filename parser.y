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
%type <int_type>      redirection

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
	built_in_operation {
		log_trace("built_in_operation");
	}
  	| built_in_operation PIPE task {
		log_trace("built_in_operation PIPE task");
	}
  	| task redirection {
  		if ($2 == 0){
			log_trace("runTask();");
			run_task();
  		}
		free_process_list();
		reset_rediractions();
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
 		// log_trace("Inicjuję liste:%s", $1);
		$$ = list_init($1);
	}
  	| text text_sequence {
		// log_trace("Dodaje do listy:%s", $1);
		$$ = list_add($2, $1);
	}

redirection:
	REDIRECTION_IN text {
		int in = open($2, O_RDWR, 00666);
		if (in < 0){
			printf("%s: Nie ma takiego pliku\n", $2);
			$$ = -1;
		} else {
			log_trace("Przekierowuje stdin z pliku: %s", $2);
			before_redirection_stdin = dup(STDIN_FILENO);
			dup2(in, STDIN_FILENO);
			$$ = 0;
		}
	}
	|REDIRECTION_OUT text {
		int out = open($2, O_RDWR | O_CREAT, 00666);
		if (out < 0){
			printf("%s: Nie mozna otworzyc pliku\n", $2);
			$$ = -1;
		} else {
			log_trace("Przekierowuje stdout do pliku: %s", $2);
			before_redirection_stdout = dup(STDOUT_FILENO);
			dup2(out, STDOUT_FILENO);
			$$ = 0;
		}
	}
	|REDIRECTION_IN text REDIRECTION_OUT text {
		int in = open($2, O_RDWR, 00666);
		int out = open($4, O_RDWR | O_CREAT, 00666);
		if (in < 0){
			printf("%s: Nie ma takiego pliku\n", $2);
			$$ = -1;
		} else if (out < 0) {
			printf("%s: Nie mozna otworzyc pliku\n", $4);
			$$ = -1;
		}
		else {
			log_trace("Przekierowuje stdin z pliku: %s", $2);
			log_trace("Przekierowuje stdout do pliku: %s", $4);
			before_redirection_stdin = dup(STDIN_FILENO);
			dup2(in, STDIN_FILENO);
			before_redirection_stdout = dup(STDOUT_FILENO);
			dup2(out, STDOUT_FILENO);
			$$ = 0;
		}
	}
	|REDIRECTION_OUT text REDIRECTION_IN text {
		int in = open($4, O_RDWR, 00666);
		int out = open($2, O_RDWR | O_CREAT, 00666);
		if (in < 0){
			printf("%s: Nie ma takiego pliku\n", $4);
			$$ = -1;
		} else if (out < 0) {
			printf("%s: Nie mozna otworzyc pliku\n", $2);
			$$ = -1;
		}
		else {
			log_trace("Przekierowuje stdin z pliku: %s", $4);
			log_trace("Przekierowuje stdout do pliku: %s", $2);
			before_redirection_stdin = dup(STDIN_FILENO);
			dup2(in, STDIN_FILENO);
			before_redirection_stdout = dup(STDOUT_FILENO);
			dup2(out, STDOUT_FILENO);
			$$ = 0;
		}
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
		// log_trace("WORD"); $$ = $1;
	}
	| VARNAME {
		// log_trace("VARNAME %s", $1); $$ = $1;
	}
	| STRING {
		// log_trace("STRING"); $$ = $1;
	}
	| STRING2 {
		// log_trace("STRING2"); $$ = $1;
	}
%%

void yyerror (char const *s) {
   	extern char *yytext;
   	printf("ERROR: %s at %s \n", s, yytext);
 }
