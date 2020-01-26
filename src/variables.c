//
// Created by maciej on 18.01.2020.
//

#include <stdlib.h>
#include <task.h>
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

    char *var = getenv(name);
    if (var != NULL) {
        setenv(name, d, 1);
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
    if (strcmp(name, "?") == 0) {
        if (last_process_status == NULL) {
            return "0";
        }
        else
            return last_process_status;
    }

    char *var = getenv(name);
    if (var != NULL) {
        return var;
    }

    struct node *tmp = list_find(name);
    if (tmp != NULL)
        return tmp->value;
    else {
        return "";
    }
}

void list_remove(char *name)
{
    unsetenv(name);
    if(name == NULL || variables_head == NULL)
        return;

    Node* tmp = variables_head;
    Node* toRem;

    if(strcmp(tmp->name, name) == 0)
    {
        variables_head = tmp->next;
        free(tmp);
        return;
    }

    while(tmp->next)
    {
        if(strcmp(tmp->next->name, name) == 0)
        {
            toRem = tmp->next;
            tmp->next = tmp->next->next;
            free(toRem);
            return;
        }
        tmp = tmp->next;
    }
}