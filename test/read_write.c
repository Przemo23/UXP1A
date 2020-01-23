//
// Created by maciej on 23.01.2020.
//

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv, char *envp[]) {

    printf("Ten program po prostu wypisuje to co przeczyta\n");

    while(1){
        char * s = NULL;
        size_t n;
        getline(&s, &n, stdin);
        if (strcmp(s, "stop\n") == 0) {
            break;
        }
        printf("Wpisałeś:%s", s);
    }

    return 0;
}