bison -d parser.y
flex scanner.l
cc -o shell parser.tab.c lex.yy.c -lfl

