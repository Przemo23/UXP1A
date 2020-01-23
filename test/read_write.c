//
// Created by maciej on 23.01.2020.
//

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv, char *envp[]) {

    printf("Ten program po prostu wypisuje to co przeczyta\n");
    fflush(stdout);

    while(1){
        char * s = NULL;
        size_t n;
        if(getline(&s, &n, stdin) == -1){
            return 1;
        }
        printf("%s", s);
        fflush(stdout);
        if (strcmp(s, "stop\n") == 0) {
            break;
        }
    }

    return 0;
}