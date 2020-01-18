//
// Created by maciej on 18.01.2020.
//

#include <log.h>
#include <memory.h>
#include <builtins.h>
#include "variables.h"

int list_add(char *name, char *d) {
    if (list_find(name) != NULL || d == NULL) {
        log_error("Nie udało się ustawić zmiennej");
        return FAILURE;
    }

    struct node *tmp = (struct node *) malloc(sizeof(struct node));

    if (tmp == NULL) {
        log_error("Nie udał się malloc");
        return FAILURE;
    }

    strncpy(tmp->name, name, MAX_VAR_NAME_LEN);
    strncpy(tmp->data, d, MAX_VAR_DATA_LEN);

    tmp->next = head;
    head = tmp;

    log_trace("Ustawiono zmienna %s na wartość %s", head->name, head->data);

    return SUCCESS;
}

void list_print() {
    struct node *tmp = head;

    while (tmp) {
        printf("%s  %s\n", tmp->name, tmp->data);
        tmp = tmp->next;
    }
}

struct node *list_find(char *name) {
    if (name == NULL)
        return NULL;

    struct node *tmp = head;

    while (tmp) {
        if (strcmp(tmp->name, name) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

// nalezy w argumencie podac wskaznik na head
void list_removeAll(struct node *tmp) {
    if (tmp == NULL)
        return;

    list_removeAll(tmp->next);

    if (head == tmp)
        head = NULL;

    free(tmp);
}

int list_remove(char *name) {
    if (name == NULL || head == NULL)
        return FAILURE;

    struct node *tmp = head;
    struct node *toRem;

    if (strcmp(tmp->name, name) == 0) {
        head = tmp->next;
        free(tmp);
        return SUCCESS;
    }

    while (tmp->next) {
        if (strcmp(tmp->next->name, name) == 0) {

            toRem = tmp->next;
            tmp->next = tmp->next->next;
            free(toRem);
            return SUCCESS;
        }
        tmp = tmp->next;
    }
    return FAILURE;
}

int list_change(char *name, char *d) {
    struct node *tmp = list_find(name);

    if (tmp == NULL || d == NULL)
        return FAILURE;

    strncpy(tmp->data, d, MAX_VAR_DATA_LEN);

    return SUCCESS;
}

char *get_variable(char *name) {
    struct node *tmp = list_find(name);
    if (tmp != NULL)
        return tmp->data;
    else {
        printf(ANSI_COLOR_RED "cannot get variable\n" ANSI_COLOR_RESET);
        return "";
    }
}

int set_variable(char *name, char *data) {
    if (strcmp(name, "USER") == 0 || strcmp(name, "CWD") == 0) {
        printf(ANSI_COLOR_RED "cannot overwrite %s\n" ANSI_COLOR_RESET, name);
        return FAILURE;
    }

    if (list_find(name) == NULL)
        return list_add(name, data);
    else
        return list_change(name, data);
}

int rm_variable(char *name) {
    return list_remove(name);
}

void rm_allVariable() {
    return list_removeAll(head);
}
