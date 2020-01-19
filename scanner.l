%{
  #include "parser.h"
%}

var   [a-zA-Z_]+[a-zA-Z0-9\_]*
wrd   [a-zA-Z0-9\.\_\-\/\\]+
str   [\'][a-zA-Z0-9\.\_\-\/\$\`\=\ \\\"]*[\']
str2  [\"][a-zA-Z0-9\.\_\-\/\$\`\=\ \\]*[\"]

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

{var}             { yylval.char_pointer_type = strdup(yytext); return VARNAME; }
{wrd}             { yylval.char_pointer_type = strdup(yytext); return WORD; }
{str}             { char * s = strdup(yytext+1); s[strlen(s)-1] ='\0'; yylval.char_pointer_type = s;  return STRING; }
{str2}            { char * s = strdup(yytext+1); s[strlen(s)-1] ='\0'; yylval.char_pointer_type = s;  return STRING; }

.                 {}

%%
