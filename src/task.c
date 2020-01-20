//
// Created by maciej on 18.01.2020.
//

#include <task.h>


// prywatna
// nie wraca (exec)
void runProgram(Proc *proc, int inFD, int outFD) {

    pid_t pid = getpid();

    if( pgid == 0 )
        pgid = pid;

    setpgid(pid, pgid);
    tcsetpgrp( terminalFD, pgid );

    struct sigaction act;
    act.sa_handler = SIG_DFL;  /* dfl - default */
    act.sa_flags = 0;

    // nie ignorujemy sygnalów
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
    sigaction(SIGTTIN, &act, NULL);
    sigaction(SIGTTOU, &act, NULL);

    if (inFD != STDIN_FILENO) {
        // dup2(oldfd,newfd) makes newfd be the copy of oldfd, closing newfd first if necessary
        dup2(inFD, STDIN_FILENO);
        close(inFD);
    }

    if (outFD != STDOUT_FILENO) {
        dup2(outFD, STDOUT_FILENO);
        close(outFD);
    }

    // todo zmienic na execvpe
    execvp(proc->argv[0], proc->argv);

    exit(-1);
}


void add_process_to_task(List_node * node) {
    Proc *p = (Proc *) malloc(sizeof(Proc));
    p->next = NULL;

    // tworzymy tablice argv
    int len = list_len(node);
    p->argv = malloc(sizeof(char*) * (len+1));
    p->argv[len]= NULL;

    int i = 0;
    for(List_node* tmp = node; tmp != NULL; tmp= tmp->next){
        p->argv[i] = (char*) malloc(sizeof(char) * (strlen(tmp->str) + 1));
        strcpy(p->argv[i++], tmp->str);
    }

    // dodaj na koniec listy
    if (proc_head == NULL) {
        proc_head = p;
    }
    else {
        Proc *tmp = proc_head;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = p;
    }

}


// rozpoczecie wykonywania zadania
void run_task() {
    log_trace("uruchamiam task:%s\n", proc_head->argv[0]);
    int fd[2], in, out;

    in = first_process_stdin;


    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        if (tmp->next != NULL) {
            pipe(fd);
            out = fd[1];
        } else {
            out = last_process_stdout;
        }

        // todo przed forkiem trzeba sprawdzić czy nie builtin
        // jezeli tak to otworzyc stdout jako out
        // po wszystkim trzeba znowu otworzyc stdout



//        if (strcmp(tmp->argv[0], "pwd") == 0) {
//            pwd_cmd();
//        } else if (strcmp(tmp->argv[0], "cd") == 0) {
//            cd_cmd(tmp->argv[1]);
//        } else if (strcmp(tmp->argv[0], "echo") == 0) {
//            echo_cmd(tmp->argv[1]);
//        }


        pid_t pid;
        pid = fork();
        if (pid == 0)
            runProgram(tmp, in, out);
            // powyzsza funkcja nigdy nie wraca (exec)

        tmp->pid = pid;
        if (!pgid)
            pgid = pid;
        setpgid(pid, pgid);
        close(in);
        close(out);

        in = fd[0];
    }

//    if (tcsetpgrp(terminalFD, pgid) == -1)
//        log_error("Nie udalo sie oddac terminala");

    kill(pgid, SIGCONT);


    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        // WUTRACED - czekamy na zakonczenie lub zatrzymanie
        waitpid(tmp->pid, NULL, WUNTRACED);
    }

    // Przenieś shell z powrotem do pierwszego planu
//    if (tcsetpgrp(terminalFD, shellPGID) == -1)
//        log_error("Nie mozna wstawic shella z powrotem do foreground");

    tcsetattr(terminalFD, TCSADRAIN, &terminalModes);
}

// prywatna
void free_recursive(Proc *tmp) {
    if(tmp == NULL)
        return;

    free_recursive(tmp->next);

    for(int i = 0; tmp->argv[i] != NULL; i++) {
        free(tmp->argv[i]);
    }
    free(tmp->argv);
    free(tmp);
}

void free_process_list() {
    free_recursive(proc_head);
    proc_head = NULL;
}
