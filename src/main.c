//
// Created by maciej on 17.01.2020.
//

#include "shell.h"

extern int yyparse();

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char **argv, char *envp[]) {
    initShell();

    while(1) {
        printf("> ");
        char *line = NULL;
        size_t n;
        getline(&line, &n, stdin);
        YY_BUFFER_STATE buffer = yy_scan_string(line);

        yyparse();
        yy_delete_buffer(buffer);
        free(line);
    }
}
#pragma clang diagnostic pop
