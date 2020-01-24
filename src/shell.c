#include "builtins.h"
#include <log.h>
#include <task.h>
#include "shell.h"
#include "variables.h"
#include <errno.h>

void initShell() {
    // deskryptorem terminala jest poczatkowy stdin
    terminalFD = STDIN_FILENO;
    // pobiera strukture reprezentujaca uzytkownika o podanym UID
    struct passwd *pass = getpwuid(getuid());
    // pobranie nazwy użytkownika
    strcpy(user, pass->pw_name);
    log_trace("Pobrano nazwe uzytkwnika: %s", user);
    // pobiera nazwe hosta
    gethostname(host, 128);
    log_trace("Pobrano nazwe hosta: %s", host);
    log_trace("Session id:%d", getsid(0));


    set_variable("PATH", getenv("PATH"));
    set_variable("HOME", getenv("HOME"));
    set_variable("USER", getenv("USER"));
    set_variable("PWD", getenv("PWD"));


    struct sigaction act;
    act.sa_handler = SIG_IGN; // ign - ignore
    act.sa_flags = 0;

    // shell ignoruje wszystkie sygnaly
//    sigaction(SIGINT, &act, NULL);
//    sigaction(SIGQUIT, &act, NULL);
//    sigaction(SIGCHLD, &act, NULL);
//    sigaction(SIGTSTP, &act, NULL);
//    sigaction(SIGTTIN, &act, NULL);
//    sigaction(SIGTTOU, &act, NULL);

    // stworzenie nowej grupy
    shellPGID = getpid();
    setpgid(0, shellPGID);

    // przejecie kontroli nad terminalem
    tcsetpgrp(terminalFD, shellPGID);

    // zapisanie poczatkowych atrybutow terminala
    tcgetattr(terminalFD, &terminalModes);

    command_out_fd = STDOUT_FILENO;
}

void print_prompt() {

    printf(ANSI_BOLD ANSI_FG_GREEN "%s@%s" ANSI_RESET, user, host);
    printf(":");

    char *homeDir = get_variable("HOME");
    char curPath[PATH_MAX];
    getcwd(curPath, PATH_MAX);

    printf(ANSI_FG_BLUE ANSI_BOLD);
    if (strstr(curPath, homeDir) != NULL) {
        printf( "~");
        printf("%s", curPath + strlen(homeDir));
    } else {
        printf("%s", curPath);
    }
    printf(ANSI_RESET);
    printf( "$ ");
}

