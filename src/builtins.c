//
// Created by maciej on 17.01.2020.
//

#include <builtins.h>

void pwd_cmd() {

    char* cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd);
    } else {
        log_error("pwd error");
    }
}

void cd_cmd(char *fileDir) {
    if (chdir(fileDir) != 0) {
        printf("%s: No such file or directory\n", fileDir);
    }
}

void echo_cmd(char *buf) {
    if (buf == NULL ) {
        printf("\n");
        return;
    }
    printf("%s\n", buf);
}


