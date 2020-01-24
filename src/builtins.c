//
// Created by maciej on 17.01.2020.
//

#include <builtins.h>
#include <shell.h>
#include <variables.h>

// todo zmienic wszystkie builtiny tak zeby przyjmowaly liste argumentow (argv)

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
    // todo zaimplementowac obsluge ~ w cd
    // todo zaimplementowac bezparametrowe wywolanie cd
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

extern char ** environ;

void export_cmd(char **argv) {
    char ** tmp = argv + 1;
    for (tmp; *tmp != NULL; tmp++) {
        char * s = get_variable(*tmp);
        if (strcmp(s, "") != 0) {
            setenv(*tmp, s, 1);
        } else {
            char * m = malloc(sizeof(char) * (strlen(*tmp) + 1));
            strcpy(m,*tmp);
            putenv(m);
        }
    }

    // nie bylo argumentow
    if (*(argv + 1) == NULL) {
        for (char **temp = environ; *temp != NULL; temp++) {
            printf("declare -x %s\n", *temp);
        }
    }

}

