//
// Created by maciej on 17.01.2020.
//

#include <log.h>
#include <variables.h>
#include "shell.h"

extern int yyparse();

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern YY_BUFFER_STATE yy_scan_string(const char *str);

extern void yy_delete_buffer(YY_BUFFER_STATE buffer);


#define REPLACE 0
#define IGNORE 1

/// zanium zaczniemy parsować stringa musimy podstawić wszystkie zmienne
void replace_env_variables(char **str) {

    char * line = *str;
    int state = REPLACE;

    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\'') {
            if (state == IGNORE) {
                state = REPLACE;
            } else {
                state = IGNORE;
            }
            continue;
        }

        // zmienna zaczyna sie $
        if (state == REPLACE && line[i] == '$') {

            size_t j = i + 1;

            // zmienna konczy sie spacja, $ lub kiedy konczy sie string
            while (line[j] != '$' && line[j] != ' ' && j < strlen(line)) {
                j++;
            }

            // teraz nazwa naszej zmiennej jest pomiedzy i+1 a j-1
            size_t name_len = (j - 1) - (i + 1) + 1; // rozmiar bez \0
            // dodajemy dodatkowy bajt na \0
            char* name = malloc((name_len + 1) * sizeof(char) );
            strncpy(name, line + i + 1, name_len);
            name[name_len] = '\0';
            char* value = get_variable(name);
            if(value == NULL) value = get_variable(name);
            log_trace("Podstawiam zmianna %s na %s", name, value);

            // alokujemy bufor ktorego rozmiar jest wiekszy o tyle o ile value zmiennej jest wieksza od jej wartosci
            char* tmp = malloc(strlen(line) + strlen(value) - strlen(name) + 2); // dodatkowo na $ i \0
            // kopiujemy i poprzednich znakow (bez $)
            strncpy(tmp, line, i);
            // kopiujemy value zmiennej do bufora zaczynajac od pozycji i (od znaku $)
            strncpy(tmp + i, value, strlen(value));
            // kopiujemy pozostala czesc line
            strcpy(tmp + i + strlen(value), line + j);

            free(line);
            free(name);
            line = tmp;

            log_trace("Nowa komenda:%s", line);

            // szukamy dalej od wartosci i + strlen(value)
            i += strlen(value) - 1;

        }
    }
    *str = line;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char **argv, char *envp[]) {

    log_trace("Inicjalizacja shell'a");
    initShell();

    while (1) {

        print_prompt();
        char *line = NULL;
        size_t n;
        getline(&line, &n, stdin);

        // trim \n at the end
        line[strlen(line) - 1] = '\0';

        replace_env_variables(&line);
        // TODO w tym miejscu trzeba preprocesowac polecenia
        // miedzy innymi trzeba obluzyc ` ; oraz podstawienia zmiennych

        log_trace("Parsuje komendę: %s", line);
        YY_BUFFER_STATE buffer = yy_scan_string(line);
        yyparse();

        yy_delete_buffer(buffer);
        free(line);
    }
}

#pragma clang diagnostic pop
