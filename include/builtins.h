//
// Created by maciej on 17.01.2020.
//

#ifndef UXP1A_BUILTINS_H
#define UXP1A_BUILTINS_H

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

#define SUCCESS 1
#define FAILURE 0
#define TRUE    1
#define FALSE   0

// Kolory tekstu na wyjsciu
#define ANSI_COLOR_RED      "\x1b[31;1m"
#define ANSI_COLOR_YELLOW   "\x1b[33;1m"
#define ANSI_COLOR_CYAN     "\x1b[36;1m"
#define ANSI_COLOR_RESET    "\x1b[0m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_BLUE        "\x1b[34m"
#define ANSI_COLOR_MAGENTA    "\x1b[35m"

// Zmienne powłoki pozwalają przypisać nazwie symbolicznej pewien ciąg znaków lub liczbę całkowitą
typedef struct variable {
    char name[20];    // nazwa zmiennej
    int mode;   // 0 - ciag znakow, 1 - liczba  calkowita
    char data[50];  // zawartosc zmiennej
} Variable;

typedef struct node {
    struct node *next;
    Variable var;
} Node;

Node *head;

// nazwa użytkownika
char user[128];
// nazwa komputera
char computer[128];
// aktualny katalog
char commandPrompt[255];


int list_add(char *name, int m, char *d);

void list_print();

Node *list_find(char *name);

void list_removeAll(Node *tmp);

int list_remove(char *name);

int list_change(char *name, int m, char *d);


int do_pwd();

int do_cd(char *fileDir);

int do_ls(char *dirName);

int do_mkdir(char *dirName);

int do_rmdir(char *dirName);

int do_touch(char *fileName);

int do_rm(char *fileName);

int do_cp(char *fileFrom, char *fileTo);


int init_variable();

char *get_variable(char *name);

int set_variable(char *name, int mode, char *data);

void refresh_prompt();


#endif //UXP1A_BUILTINS_H
