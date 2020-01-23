//
// Created by maciej on 18.01.2020.
//

#ifndef UXP1A_TASK_H
#define UXP1A_TASK_H


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
#include "list.h"
#include <variables.h>
#include "shell.h"



// reprezentuje proces
typedef struct proc {
    struct proc *next;     // nastepny proces w pipie
    char **argv;            // argument dla exec
    pid_t pid;              // ID procesu
} Proc;

Proc *proc_head;

// zazwyczaj to stdout, ta wartosc jest ustawiana jako pipe dla komend wywolywanych w ` `
int command_out_fd;

// stdin lub deskryptor pliku jezeli bylo przekierowanie
int first_process_in_fd;

// ta wartosc inicjowana jest przez command_out_fd
int last_process_out_fd;

pid_t pgid;

void init_task();

// dodaje proces do listy procesow
void add_process_to_task(List_node * node);

// tworzy potoki, startuje procesy, czeka na ich zakonczenie
void run_task();

void free_process_list();

#include "shell.h"

#endif //UXP1A_TASK_H
