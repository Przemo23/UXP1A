//
// Created by maciej on 17.01.2020.
//

#ifndef UXP1A_SHELL_H
#define UXP1A_SHELL_H


#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include "builtins.h"
#include "defines.h"


// numer grupy procesów shella
pid_t shellPID;
// Deskryptor terminala
int terminalFD;
// przechowuje ustawienia terminala
struct termios terminalModes;

// nazwa użytkownika
char user[MAX_USER_LEN];
// nazwa komputera
char host[MAX_HOST_LEN];

void initShell();

void print_prompt();

unsigned char finish_execution, parse_error;

#endif //UXP1A_SHELL_H
