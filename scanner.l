/* just like Unix wc */
%{
  #include "parser.h"
  /*
  enum yytokentype {
    REDIRECTION_IN = 258;
    REDIRECTION_OUT = 259;
    FUNKY = 260;
    PIPE = 261;
    APOSTROPHE = 262;
    SEMICOLON = 263;
    NEWLINE = 264;
    PWD_CMD = 265;
    ECHO_CMD = 266;
    CD_CMD = 267;
    EXPORT_CMD= 268;
    REDIRECTION_IN = 269;
    REDIRECTION_IN = 270;

  }*/
%}
str   ['][^ \t\n\r\f\v]+[']
var   [a-zA-Z0-9_\./-]+
var2  [a-zA-Z0-9_\./$-]+

%%
"<"               { return REDIRECTION_IN; }
">"               { return REDIRECTION_OUT; }
"`"               { return FUNKY; }
"|"               { return PIPE; }
';'               { return SEMICOLON; }
"="               {  return EQUALS; }
\n                { return NEWLINE; }
"pwd"             { return PWD_CMD; }
"echo"            { return ECHO_CMD; }
"cd"              { return CD_CMD; }
"export"          { return EXPORT_CMD; }
{var}             { return VARNAME;}
{var2}            { return VARNAME2;}
{str}             { return STRING; }
%%
