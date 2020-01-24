//
// Created by maciej on 23.01.2020.
//

#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv, char *envp[]) {
    char **temp = envp;
    setenv("TEST", "TEST", 1);

    printf("%s\n", getenv("TEST"));

    printf("Wypisuje envp:\n");
    for (char **temp = envp; *temp != NULL; temp++) {
        printf("%s\n", *temp);
    }
    printf("\n");
    return 0;
}