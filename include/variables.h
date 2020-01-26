//
// Created by maciej on 18.01.2020.
//

#ifndef UXP1A_VARIABLES_H
#define UXP1A_VARIABLES_H

#include "log.h"
#include "defines.h"
#include "string.h"
#include "malloc.h"

typedef struct node {
    struct node *next;
    char name[MAX_VAR_NAME_LEN];    // nazwa zmiennej
    char value[MAX_VAR_DATA_LEN];  // zawartosc zmiennej

} Node;

Node *variables_head;

void set_variable(char *name, char *d);

char *get_variable(char *name);

void list_remove(char *name);

#endif //UXP1A_VARIABLES_H
