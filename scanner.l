%{
  #include "parser.h"
%}

var   [a-zA-Z_]+[a-zA-Z0-9_]*
wrd   [a-zA-Z0-9._\-/]+
str   ['][a-zA-Z0-9._\-/$"`= ]*[']
str2  ["][a-zA-Z0-9._\-/$"`= ]*["]

%%
"<"               { return REDIRECTION_IN; }
">"               { return REDIRECTION_OUT; }
"|"               { return PIPE; }
"="               { return EQUALS; }
";"               { return SEMICOLON; }

"pwd"             { return PWD_CMD; }
"echo"            { return ECHO_CMD; }
"cd"              { return CD_CMD; }
"export"          { return EXPORT_CMD; }

{var}             { return VARNAME; }
{wrd}             { return WORD; }
{str}             { return STRING; }
{str2}            { return STRING2; }

%%
