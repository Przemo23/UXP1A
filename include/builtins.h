//
// Created by maciej on 17.01.2020.
//

#ifndef UXP1A_BUILTINS_H
#define UXP1A_BUILTINS_H

#include "shell.h"
#include "log.h"

#include <limits.h>
#include <zconf.h>
#include <stdlib.h>

void pwd_cmd();

void cd_cmd(char **argv);

void echo_cmd(char **argv);

void export_cmd(char **argv);

void unset_cmd(char **argv);

#endif //UXP1A_BUILTINS_H
