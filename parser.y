%{
#include <stdio.h>
int yylex();
void yyerror(const char *s);

%}

/*TOKENS*/
%token REDIRECTION_IN REDIRECTION_OUT FUNKY PIPE NEWLINE SEMICOLON EQUALS
%token PWD_CMD ECHO_CMD CD_CMD EXPORT_CMD
%token VARNAME VARNAME2
%token STRING

%%
input:
  | full_command input
full_command: command NEWLINE{} {printf("command called \n");}
  | command redirection NEWLINE{} {printf("command with redirection \n");}
  | assignment_sequence NEWLINE{} {printf("assignment_sequence called \n");}

command:built_in_operation {printf("built_in_operation called \n");}
  | built_in_operation PIPE pipe {printf("built_in_operation with pipe called \n");}
  | pipe {printf("pipe called \n");}

pipe: external_call {printf("external_call called \n");}
  | external_call PIPE pipe {printf("external_call with pipe called \n");}

built_in_operation: built_in_command text_sequence {printf("command with param called \n");}
  | built_in_command {printf("built_in_command called \n");}
  | export_with_assignment {printf("export_with_assignment called \n" );}

external_call: text_sequence {printf("text_sequence called \n");}/* check if not redundant */

text_sequence: text
  | text text_sequence

built_in_command: PWD_CMD {printf("pwd called \n" );}
  | ECHO_CMD {printf("echo called \n");}
  | CD_CMD{printf("cd called \n" );}
  | EXPORT_CMD{printf("export called \n");}

redirection: redirection_in {printf("redirection_in \n");}
  |redirection_out  {printf("redirection_out \n");}
  |redirection_inout  {printf("redirection_inout \n");}
  |redirection_outin  {printf("redirection_outin \n");}

redirection_in: REDIRECTION_IN text

redirection_out: REDIRECTION_OUT text

redirection_inout: REDIRECTION_IN text REDIRECTION_OUT text

redirection_outin: REDIRECTION_OUT text REDIRECTION_IN text

assignment_sequence: assignment {printf("assignment called");}
  | assignment_sequence assignment

export_with_assignment: EXPORT_CMD assignment

assignment: VARNAME EQUALS text
  | VARNAME EQUALS VARNAME2
text: VARNAME {printf("VARNAME \n");}
  | STRING {printf("STRING \n" );}
%%
void yyerror (char const *s) {
   fprintf (stderr, "%s\n", s);
 }
