//
// Created by maciej on 18.01.2020.
//

#include "variables.h"

// prywatna
struct node *list_find(char *name) {
    if (name == NULL)
        return NULL;

    struct node *tmp = variables_head;

    while (tmp) {
        if (strcmp(tmp->name, name) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void set_variable(char *name, char *d) {

    if (d == NULL) {
        log_error("Niepoprawne wywolanie.");
        return;
    }

    Node *found = list_find(name);

    if (list_find(name) != NULL){
        strncpy(found->value, d, MAX_VAR_DATA_LEN);
        log_trace("Zmieniono zmienna %s na %s", found->name, found->value);
        return;
    }

    struct node *tmp = (struct node *) malloc(sizeof(struct node));

    if (tmp == NULL) {
        log_error("Nie udał się malloc");
        return;
    }

    strncpy(tmp->name, name, MAX_VAR_NAME_LEN);
    strncpy(tmp->value, d, MAX_VAR_DATA_LEN);

    tmp->next = variables_head;
    variables_head = tmp;

    log_trace("Ustawiono zmienna %s na %s", variables_head->name, variables_head->value);
}

char *get_variable(char *name) {
    struct node *tmp = list_find(name);
    if (tmp != NULL)
        return tmp->value;
    else {
        return "";
    }
}
