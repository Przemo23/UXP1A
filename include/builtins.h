//
// Created by maciej on 17.01.2020.
//

#ifndef UXP1A_BUILTINS_H
#define UXP1A_BUILTINS_H

#include "log.h"

#include <limits.h>
#include <zconf.h>
#include <stdlib.h>
#include <variables.h>

void pwd_cmd();

void cd_cmd(char *fileDir);

void echo_cmd(char *buf);

void export_cmd();

#endif //UXP1A_BUILTINS_H
