//
// Created by maciej on 17.01.2020.
//

#include "shell.h"

extern int yyparse();

int main(int argc, char **argv, char *envp[]) {
    printf("Wypisano z maina\n");
    initShell();
    yyparse();

    return 0;
}
