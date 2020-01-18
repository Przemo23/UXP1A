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
#include "defines.h"

#define SUCCESS 1
#define FAILURE 0
#define TRUE    1
#define FALSE   0

// Kolory tekstu na wyjsciu


int do_pwd();

int do_cd(char *fileDir);

int do_ls(char *dirName);

int do_mkdir(char *dirName);

int do_rmdir(char *dirName);

int do_touch(char *fileName);

int do_rm(char *fileName);

int do_cp(char *fileFrom, char *fileTo);

void do_echo(char *buf);

#endif //UXP1A_BUILTINS_H
