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

int before_redirection_stdin;
int before_redirection_stdout;
char * last_process_status;


pid_t pgid;

void reset_redirections();

// dodaje proces do listy procesow
void add_process_to_task(List_node * node);

// tworzy potoki, startuje procesy, czeka na ich zakonczenie
void run_task();

void free_process_list();

char * proc_list_convert_to_str();

#include "shell.h"

#endif //UXP1A_TASK_H
