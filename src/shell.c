#include "shell.h"

// nalezy uwazac, gdy wykonujemy shella który ma kontrole pracy i jest wolany z innego shell, ktory ma swoja kontrole pracy
void initShell() {
    firstGroup = NULL;

    // deskryptor pliku dla standardowego wejscia
    shellTerminal = STDIN_FILENO;

    init_variable();

    struct sigaction act;
    act.sa_handler = SIG_IGN;  /* set up signal handler */
    act.sa_flags = 0;

    // kiedy shell przejmuje kontrole, powinien ignorowac ponizsze sygnaly, zeby samemu sie przypadkowo nie killnac
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


Task *findTask(pid_t pgid) {
    Task *t = firstGroup;

    while (t != NULL) {
        if (t->pgid == pgid)
            return t;
        t = t->next;
    }

    return NULL;
}

//czekamy na zakonczenie procesow w tej grupie ( lub na ich zatrzymanie )
void waitForTaskToFinish(Task *t) {
    Proc *tmp = t->firstProc;

    while (tmp != NULL) {
        // odbieramy status zakonczenie procesu
        // WAIT_ANY - czekamy na wszystkie dzieci
        // WUTRACED - odbieramy sygnaly od procesow zakonczonych jak rowniez zatrzymanych
        waitpid(tmp->pid, NULL, WUNTRACED);
        // kontynuujemy dopoki wwszystkie procesy nie beda oznaczoone ( zakonczone lub zatrzymane )
        tmp = tmp->next;
    }
}

void removeTask(Task *t) {
    if (t == NULL)
        return;

    Proc *next;

    Proc *p = t->firstProc;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }

    Task *tmp = firstGroup;

    if (t == tmp) {
        firstGroup = tmp->next;
        free(t);
        return;
    }

    while (tmp->next) {
        if (t == tmp->next) {
            tmp->next = tmp->next->next;
            free(t);
            return;
        }
        tmp = tmp->next;
    }
    return;
//  free(t->command);
}

int bringTaskIntoForeground(Task *t) {
    // wzucenie pracy do foreground - dokladniej oddanie shella dla grupy procesow z taska
    if (tcsetpgrp(shellTerminal, t->pgid) == -1)
        printf("Nie mozna wstawic procesu do foreground");

    tcsetattr(shellTerminal, TCSADRAIN, &t->modes);
    kill(-t->pgid, SIGCONT);

    // kiedy wszystkie procesy zwykle/pipe sa zakonczone lub zastopowane, shell powinien przywrocic sobie kontrole nad terminalem
    waitForTaskToFinish(t);

    // Przenieś shell z powrotem do pierwszego planu
    if (tcsetpgrp(shellTerminal, shellPGID) == -1)
        printf("Nie mozna wstawic shella z powrotem do foreground");

    // task moze zostawic terminal w dziwnym stani
    // przywrocenie ustawien terminala jakie byly przy shellu
    tcgetattr(shellTerminal, &t->modes);
    tcsetattr(shellTerminal, TCSADRAIN, &shellModes);

    return 0;
}

void bringTaskIntoBackground(Task *t) {
    // wznowienie procesow zatrzymanych
    kill(-t->pgid, SIGCONT);
}

// oznaczenie status procesu
int setProcesStatus(pid_t pid, int status) {
    Task *task;
    Proc *proc;

    if (pid > 0) {
        // uaktualnienie status dla danego procesu
        for (task = firstGroup; task != NULL; task = task->next) {
            for (proc = task->firstProc; proc; proc = proc->next) {
                if (proc->pid == pid) {
                    proc->status = status;
                    // flaga sprawdzajaca czy dany status pochodzi od procesu zatrzymanego
                    if (WIFSTOPPED (status))
                        proc->paused = 1;
                    else {
                        proc->finished = 1;
                        // sprawdzenie czy proces zakonczyl sie w wyniku otrzymania sygnalu
                        /*if (WIFSIGNALED (status))
                          fprintf (stderr, "%d: Zatrzymany poprzez sygnal o numerze: %d.\n",
                                   (int) pid, WTERMSIG (p->status));*/
                    }
                    return 0;
                }
            }
        }
        fprintf(stderr, "Nie znaleziono procesu o pid: %d.\n", pid);
        return -1;
    }

        // jesli nie ma procesu potomnego
    else if (pid == 0)
        return -1;

        // inne bledy
    else
        //perror ("waitpid");
        return -1;
}

// sprawdzenie statusow procesow zakonczonych badz zatrzymanych - bez blokowania
// dla procesow wykonujacych sie w tle
void refreshStatus(void) {
    int status;
    pid_t pid;

    do {
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    } while (!setProcesStatus(pid, status));
}

// sprawdzenie czy procesy sa zatrzymane badz skonczone
int checkIsTaskPaused(Task *t) {
    Proc *proc;

    for (proc = t->firstProc; proc; proc = proc->next)
        if (!proc->finished && !proc->paused)
            return 0;
    return 1;
}

// sprawdzenie czy procesy sie zakonczyly
int checkIsTaskDone(Task *t) {
    Proc *proc;

    for (proc = t->firstProc; proc; proc = proc->next)
        if (!proc->finished)
            return 0;
    return 1;
}

// rozpoczecie wykonywania zadania
void startTask(Task *task, int fg) {
    Proc *proc;
    pid_t pid;
    int commPipe[2], infile, outfile;

    printf("task: %s\n", task->firstProc->argv[0]);

    infile = task->stdin;
    for (proc = task->firstProc; proc != NULL; proc = proc->next) {
        // jesli wiecej niz jeden proces w tasku to uzyj potoku
        if (proc->next != NULL) {
            pipe(commPipe);
            outfile = commPipe[1];
        } else
            outfile = task->stdout;

        // forkujemy
        pid = fork();
        if (pid == 0)
            // potomek - startujemy nowy proces
            runProcess(proc, task->pgid, fg, infile, outfile, task->stderr);
        else {
            // w procesie macierzystym
            proc->pid = pid;
            // jesli task nie ma przypisanej grupy
            if (!task->pgid)
                task->pgid = pid;
            setpgid(pid, task->pgid);
        }

        // zamkniecie niepotrzebnych deskryptorow
        if (infile != task->stdin)
            close(infile);
        if (outfile != task->stdout)
            close(outfile);
        infile = commPipe[0];
    }

    // uruchomienie zadanie w foreground badz tle
    if (fg == 1)
        bringTaskIntoForeground(task);
    else
        bringTaskIntoBackground(task);
}

void runProcess(Proc *proc, pid_t pgid, int fg, int inputFile, int outputFile, int errorFile) {
    // wrzucenie procesu do grupy procesow i danie przekazanie grupy terminalowu jezeli jest to stosowne
    // utowrzenie grupy lub dolaczenie do juz istniejacej
    pid_t pid = getpid();

    if (pgid == 0)
        pgid = pid;

    setpgid(pid, pgid);

    // jesli w foreground to oddajemy terminal dla grupy procesow
    if (fg == 1) {
        tcsetpgrp(shellTerminal, pgid);
    }

    struct sigaction act;
    act.sa_handler = SIG_DFL;  /* set up signal handler */
    act.sa_flags = 0;

    // kiedy shell przejmuje kontrole, powinien ignorowac ponizsze sygnaly, zeby samemu sie przypadkowo nie killnac
    sigaction(SIGINT, &act, NULL);
    // powrot do ustawien domyslnych (przez nasz shell te sygnaly byly ignorowane)
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
    sigaction(SIGTTIN, &act, NULL);
    sigaction(SIGTTOU, &act, NULL);

    // ustawienie standardowego I/O dla nowego procesu
    // jesli wyjscia sa inne niz standardowe to zamieniamy je - przekierowanie wyjscia
    if (inputFile != STDIN_FILENO) {
        dup2(inputFile, STDIN_FILENO);
        close(inputFile);
    }

    if (outputFile != STDOUT_FILENO) {
        dup2(outputFile, STDOUT_FILENO);
        close(outputFile);
    }

    if (errorFile != STDERR_FILENO) {
        dup2(errorFile, STDERR_FILENO);
        close(errorFile);
    }
    if (strcmp(proc->argv[0], "pwd") == 0) {
        do_pwd();
    } else if (strcmp(proc->argv[0], "cd") == 0) {
        do_cd(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "ls") == 0) {
        do_ls(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "mkdir") == 0) {
        do_mkdir(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "rmdir") == 0) {
        do_rmdir(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "touch") == 0) {
        do_touch(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "rm") == 0) {
        do_rm(proc->argv[1]);
    } else if (strcmp(proc->argv[0], "cp") == 0) {
        do_cp(proc->argv[1], proc->argv[2]);
    } else if (strcmp(proc->argv[0], "echo") == 0) {
        do_echo(proc->argv[1]);
    } else
        run(proc->argv[0], proc->argv);
    //execvp(proc->argv[0], proc->argv);
    // nie powinien wykonac exit'a przy poprawnym wykonaniu
    //exit(-1);
}

void run(char *program, char *args[]) {
    int pid;
    int cp[2]; /* potok z dziecka do rodzica */
    char ch;
    char out[10000];
    int incount = 0, outcount = 0;

    if (pipe(cp) < 0) {
        perror("Nie można stworzyć potoku");
        return NULL;
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

Task *addNewTask() {
    Task *t = (Task *) malloc(sizeof(Task));
    t->pgid = 0;

    t->next = NULL;
    t->firstProc = NULL;
    t->stdin = STDIN_FILENO;
    t->stdout = STDOUT_FILENO;
    t->stderr = STDERR_FILENO;

    tcgetattr(shellTerminal, &t->modes);

    Task *temp = firstGroup;
    if (firstGroup == NULL) {
        firstGroup = t;
    } else {
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = t;
    }

    return t;
}

void addProcessIntoLastTask(char *args[]) {
    Proc *p = (Proc *) malloc(sizeof(Proc));

    p->next = NULL;
    p->finished = 0;
    p->paused = 0;
    p->pid = 0;
    p->status = 0;

    p->argv = args;

    // temp - iteracja po taskach
    // temp2 - iteracja po procesach w ost. tasku
    if (firstGroup == NULL)
        return;
    Task *temp = firstGroup;
    while (temp->next != NULL)
        temp = temp->next;
    Proc *temp2 = temp->firstProc;
    if (temp2 == NULL) {
        temp->firstProc = p;
    } else {
        while (temp2->next != NULL) {
            temp2 = temp2->next;
        }
        temp2->next = p;
    }
}


