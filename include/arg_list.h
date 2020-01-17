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

typedef struct ang_node
{
    struct arg_node * next;
    char arg[50];
} Arg_node;

Arg_node * init_arg(char * name);
Arg_node *  arg_list_add(Arg_node * node, char* name);
void arg_list_remove_all(Arg_node * tmp);
int arg_list_count(Arg_node * node);
void print_arg_list(Arg_node * tmp);

#endif
