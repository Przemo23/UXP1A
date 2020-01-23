//
// Created by maciej on 23.01.2020.
//

#include <stdio.h>
#include <zconf.h>
#include <string.h>

int main(int argc, char **argv, char *envp[]) {
    char **temp = argv;
    printf("Wypisuje argv:\n");
    for (int i = 0; i < argc; i ++) {
        printf("%s ", *temp);
        temp++;
    }
    printf("\n");
    return 0;
}