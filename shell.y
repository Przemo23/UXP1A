%{
#ifndef CALC_Y_
#define CALC_Y_
void yyerror(const char *s);
void reset(void);
void init(void);
int yylex(void);

#define ARGS 10

int argc=0;
char *argv[ARGS];

#include "pipe.h"

%}

%error-verbose
%debug

%union{
	int     int_val;
	char*	op_val;
	char*	str;
	char*	var;
	Arg_node * arguments;
}

%start	input 


%token	<int_val>	INTEGER_LITERAL
%type	<int_val>	exp
%type	<int_val>	input
%type	<str>		cmd
%type	<arguments>		pipe
%token	<str>		STRING
%type	<var>		variable
%type	<arguments>		args

%token	PLUS
%token	MULT
%token	EQUALS
%token	QMARK
%token	COLON
%token	TILDE
%token	OUTPUT
%token	OUTPUT_APPEND
%token	INPUT
%token	LPARAM
%token	RPARAM
%token	AMPERSAND
%token	NEWLINE

%token	CD_CMD
%token	PWD_CMD
%token	ECHO_CMD
%token	TOUCH_CMD
%token	RM_CMD
%token	MKDIR_CMD
%token	RMDIR_CMD
%token	CP_CMD
%token	EXIT_CMD

%%

input:	{ init(); reset(); }
		| input variable NEWLINE { reset(); }
		| input cmd NEWLINE { reset(); }
		| input exp NEWLINE { reset(); }
		;
		
variable:	STRING EQUALS STRING {
			printf("Ustawianie zmiennej\n");
			set_variable($1, 0 ,$3);
		}
		;
		
cmd:	| CD_CMD STRING { 
			do_cd($2);
		}
		| CD_CMD { 
			do_cd(NULL);
		}
		| PWD_CMD {
			do_pwd();
		}
		| ECHO_CMD STRING {git@github.com:sstobiec/UXP1a-shell.git
			do_echo($2);
		}
		| TOUCH_CMD STRING {
			do_touch($2);
		}
		| RM_CMD STRING {
			do_rm($2);
		}
		| RMDIR_CMD STRING {
			do_rmdir($2);
		}
		| MKDIR_CMD STRING {
			do_mkdir($2);
		}
		| CP_CMD STRING STRING {
			do_cp($2, $3);
		}
		| EXIT_CMD {
			exit(0);
		}
		| pipe {
			printf("POTOK\n");
			Task * t1 = addNewTask(-1, -1, -1);
			startTask(t1, 0);
		}
		./a | ./5
args:   STRING {
			$$ = init_arg($1);
		}
		| STRING args {
			$$ = arg_list_add($2, $1);			
		}

pipe:	args { ./programa arg1 arg2 | ./program2 arg1 arg2
			$$ = $1;
			
			Arg_node *tmp = (Arg_node*)malloc(sizeof(Arg_node));
			tmp = $$;
			
			int size = arg_list_count(tmp);			
			char *tab[size+1];
			int i=0;
			tmp = $$;
			
			for(i=0; i<size; ++i, tmp=tmp->next) {
				tab[i] = (char*)malloc(400);
				strcpy(tab[i], tmp->arg);
			}
			tab[i]=(char*)0;
			for(i=0; i<size+1; ++i) {
				printf("%s, ", tab[i]);
			}
			printf("\n");
			
			addProcessIntoLastTask(tab);
				
			arg_list_remove_all($1);
		}
		| args TILDE pipe {			
			$$ = $1;
			
			Arg_node *tmp = (Arg_node*)malloc(sizeof(Arg_node));
			tmp = $$;
			
			int size = arg_list_count(tmp);			
			char *tab[size+1];
			int i=0;
			tmp = $$;
			
			for(i=0; i<size; ++i, tmp=tmp->next) {
				tab[i] = (char*)malloc(400);
				strcpy(tab[i], tmp->arg);
			}
			tab[i]=(char*)0;
			for(i=0; i<size+1; ++i) {
				printf("%s, ", tab[i]);
			}
			printf("\n");
			
			addProcessIntoLastTask(tab);
				
			arg_list_remove_all($1);
		}

exp:	INTEGER_LITERAL	{ $$ = $1; }
		| exp PLUS exp	{ $$ = $1 + $3; }
		| exp MULT exp	{ $$ = $1 * $3; }
		;
		

%%

void yyerror(const char* s)
{
	extern int yylineno;	// w lex.c
	extern char *yytext;	

	printf("ERROR: %s at symbol \"%s\n", s, yytext);
	printf("on line %d\n", yylineno);
}

void init(void) {
	int i=0;
	while(i<ARGS)
		argv[i++]=(char*)malloc(400);
}

void reset(void) {
	printf("# ");
	int i=0;
	while(i<ARGS) {
		strcpy(argv[i++], "");
	}
	argc=0;
}

#endif
