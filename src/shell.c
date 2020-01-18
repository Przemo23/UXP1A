#include "builtins.h"
#include <log.h>
#include "shell.h"
#include "variables.h"
#include "pipe.h"

// nalezy uwazac, gdy wykonujemy shella który ma kontrole pracy i jest wolany z innego shell, ktory ma swoja kontrole pracy
void initShell() {
    firstGroup = NULL;

    // deskryptor pliku dla standardowego wejscia
    shellTerminal = STDIN_FILENO;

    // pobiera strukture reprezentujaca uzytkownika o podanym UID
    struct passwd *pass = getpwuid(getuid());
    // pobranie nazwy użytkownika
    strcpy(user, pass->pw_name);
    log_trace("Pobrano nazwe uzytkwnika: %s", user);
    // pobiera nazwe hosta
    gethostname(host, 128);
    log_trace("Pobrano nazwe hosta: %s", host);


    list_add("PATH", getenv("PATH"));
    list_add("HOME", getenv("HOME"));
    list_add("USER", getenv("USER"));
    list_add("PWD", getenv("PWD"));


    struct sigaction act;
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;

    // shell ignoruje wszystkie sygnaly
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
    sigaction(SIGTTIN, &act, NULL);
    sigaction(SIGTTOU, &act, NULL);

    // stworzenie nowej grupy, nowe procesy beda do niej nalezaly
    shellPGID = getpid();
    setpgid(0, shellPGID);

    // przejecie kontroli nad terminalem
    tcsetpgrp(shellTerminal, shellPGID);

    // zapisanie domyslnych atrybutow terminala dla shella
    tcgetattr(shellTerminal, &shellModes);
}


// sprawdzenie czy procesy sa zatrzymane badz skonczone
int checkIsTaskPaused(Pipe *t) {
    Proc *proc;

    for (proc = t->firstProc; proc; proc = proc->next)
        if (!proc->finished && !proc->paused)
            return 0;
    return 1;
}

void run(char *program, char *args[]) {
    int pid;
    int cp[2]; /* potok z dziecka do rodzica */
    char ch;
    char out[10000];
    int incount = 0, outcount = 0;

    if (pipe(cp) < 0) {
        perror("Nie można stworzyć potoku");
        return;
    }


    /*switch( pid = fork() ) {
        case -1:
                perror("Nie mozna wykonać fork()");
                return NULL;
        case 0:
                // Child
                close(1); // zamknij obecne stdout
                dup( cp[1]); // przekieruj stdout na potok
                close( cp[0]);*/

    //wywołanie z użyceim PATH
    char *path, *dir, *tmp, *args_tmp;

    path = (char *) malloc(1024);
    dir = (char *) malloc(1024);
    tmp = (char *) malloc(1024);
    args_tmp = (char *) malloc(1024);

    strcpy(path, get_variable("PATH"));
    strcat(path, ":/home/michal/tkom/uxp/poligon");
    strcpy(args_tmp, args[0]);

    dir = strtok(path, ":");
    while (dir != NULL) {
        strcpy(tmp, dir);
        strcat(tmp, "/");
        strcat(tmp, program);

        char *temp3[] = {"/bin/ps", "-a", (char *) 0};

        //execl(tmp, program, (char *)NULL);
        execv(tmp, args);
        dir = strtok(NULL, ":");
    }

    /*perror("No exec");
    exit(1);
default:
    // Parent
    close(cp[1]);
    int i=0;
    while( read(cp[0], &ch, 1) == 1)
    {
        write(1, &ch, 1);
        out[i++]=ch;
    }
    //printf("Wyjście: %s", out);
}
return NULL;*/

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