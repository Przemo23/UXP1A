//
// Created by maciej on 17.01.2020.
//

#include <builtins.h>
#include <shell.h>
#include <variables.h>

void pwd_cmd() {

    char* cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd);
    } else {
        log_error("pwd error");
    }
}

void cd_cmd(char **argv) {
    // nie ma argumentow
    if (*(argv + 1) == NULL) {
        chdir(getenv("HOME"));
        return;
    }
    char *path = argv[1];
    if (path[0] == '~') {
        char *home = getenv("HOME");
        char *s = malloc((strlen(path) + strlen(home) + 1) * sizeof(char));
        s[0] = '\0';
        strcat(s, home);
        strcat(s,path + 1);
        if (chdir(s) != 0) {
            printf("%s: No such file or directory\n", s);
        }
        free(s);
        return;
    }

    if (chdir(path) != 0) {
        printf("%s: No such file or directory\n", path);
    }
}

void echo_cmd(char **argv) {
    // nie ma argumentow
    if (*(argv + 1) == NULL) {
        printf("\n");
        return;
    }
    char ** tmp = argv + 1;
    for (; *tmp != NULL; tmp++) {
        printf("%s ", *tmp);
    }
    printf("\n");

}

extern char ** environ;

void export_cmd(char **argv) {
    // nie ma argumentow
    if (*(argv + 1) == NULL) {
        for (char **temp = environ; *temp != NULL; temp++) {
            printf("declare -x %s\n", *temp);
        }
        return;
    }

    char ** tmp = argv + 1;
    for (; *tmp != NULL; tmp++) {
        char * s = get_variable(*tmp);
        if (strcmp(s, "") != 0) {
            setenv(*tmp, s, 1);
        } else {
            char * m = malloc(sizeof(char) * (strlen(*tmp) + 1));
            strcpy(m,*tmp);
            putenv(m);
        }
    }
}

void unset_cmd(char **argv) {
    char ** tmp = argv + 1;
    for (; *tmp != NULL; tmp++) {
        list_remove(*tmp);
    }
}
