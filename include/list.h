#pragma once
#ifndef ARGSLIST_H_
#define ARGSLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>

#include "defines.h"

typedef struct list_node
{
    struct list_node * next;
    char str[MAX_LIST_DATA_LEN];


} List_node;


List_node *     list_init(char * name);
List_node *     list_add(List_node * node, char* name);
void            list_free(List_node * head);
int             list_len(List_node * node);
void            list_print(List_node * tmp);
char *          list_convert_to_str(List_node * tmp);

#endif
