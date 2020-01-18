//
// Created by maciej on 18.01.2020.
//

#ifndef UXP1A_PIPE_H
#define UXP1A_PIPE_H


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

// reprezentuje proces
typedef struct proc {
    struct proc *next;     // nastepny proces w pipie
    char **argv;            // argument dla exec
    pid_t pid;              // ID procesu
    char finished;          // jezeli proces sie zakonczyl to 1
    char paused;            // jezeli proces zatrzymany to 1
    int status;
} Proc;
// pipe jest potokiem procesow
typedef struct pipe {
    struct pipe *next;     // nastepny aktywny pipe
    //char * command;         // wiersz polecen uzywany dla wiadomosci
    pid_t pgid;             // ID grupy
    Proc *firstProc;       // lista procesow skladajaca sie na potok
    ///////////////////////////////////////////////////////////////////////////////
    //char notified;              /* true if user told about stopped job */
    struct termios modes;   // przechowuje ustawienia terminala
    ///////////////////////////////////////////////////////////////////////////////
    // standardowe kanaly I/O
    int stdin;
    int stdout;
    int stderr;
} Pipe;
Pipe *firstGroup;

Pipe *findPipe(pid_t pgid);

void waitForPipeToFinish(Pipe *t);

void removePipe(Pipe *t);

int bringTaskIntoForeground(Pipe *t);

void bringTaskIntoBackground(Pipe *t);

int setProcesStatus(pid_t pid, int status);

void refreshStatus(void);

int checkIsPipeDone(Pipe *t);

void startPipe(Pipe *task, int fg);

void runProcess(Proc *proc, pid_t pgid, int inputFile, int outputFile, int errorFile, int fg);

void runProcess(Proc *proc, pid_t pgid, int inputFile, int outputFile, int errorFile, int fg);

Pipe *addNewPipe();

void addProcessIntoLastPipe(char **args);

#include "shell.h"

#endif //UXP1A_PIPE_H
