//
// Created by maciej on 17.01.2020.
//

#ifndef UXP1A_SHELL_H
#define UXP1A_SHELL_H


#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include <unistd.h>
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

// task jest potokiem procesow
typedef struct task {
    struct task *next;     // nastepny aktywny task
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
} Task;

// numer grupy procesów shella
pid_t shellPGID;
// Deskryptor terminala
int shellTerminal;
// przechowuje ustawienia terminala
struct termios shellModes;

Task *firstGroup;

void runProcess(Proc *proc, pid_t pgid, int inputFile, int outputFile, int errorFile, int fg);

void initShell();

Task *findTask(pid_t pgid);

void waitForTaskToFinish(Task *t);

void removeTask(Task *t);

int bringTaskIntoForeground(Task *t);

void bringTaskIntoBackground(Task *t);

void startTask(Task *task, int fg);

void addProcessIntoLastTask(char *args[]);

int checkIsTaskDone(Task *t);

Task *addNewTask();

void refreshStatus(void);

int setProcesStatus(pid_t pid, int status);

void runProcess(Proc *proc, pid_t pgid, int inputFile, int outputFile, int errorFile, int fg);

#endif //UXP1A_SHELL_H
