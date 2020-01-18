//
// Created by maciej on 18.01.2020.
//

#ifndef UXP1A_VARIABLES_H
#define UXP1A_VARIABLES_H

#include "defines.h"

typedef struct node {
    struct node *next;
    char name[MAX_VAR_NAME_LEN];    // nazwa zmiennej
    char value[MAX_VAR_DATA_LEN];  // zawartosc zmiennej

} Node;

Node *head;


int list_add(char *name, char *d);

void list_print();

Node *list_find(char *name);

void list_removeAll(Node *tmp);

int list_remove(char *name);

int list_change(char *name, char *d);

char *get_variable(char *name);

int set_variable(char *name, char *data);

int rm_variable(char *name);

void rm_allVariable();

#endif //UXP1A_VARIABLES_H
