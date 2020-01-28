//
// Created by maciej on 18.01.2020.
//

#include <task.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>


// prywatna
// nie wraca (exec)

void run_program(Proc *proc) {
    pid_t pid = getpid();

    if (!pgid) {
        pgid = pid;
        if(setpgid(pid, pgid == -1)){
            log_error("Nie udalo sie setpgid: %d", strerror(errno));
        }
    }

    struct sigaction act = {0};
    act.sa_handler = SIG_DFL; // dfl - default
    act.sa_flags = 0;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTTIN, &act, NULL);
    sigaction(SIGTTOU, &act, NULL);
  
    execvp(proc->argv[0], proc->argv);
    printf("Nie udalo sie uruchomic zadania %s\n", proc->argv[0]);
    exit(7);
}

void reset_rediractions() {
    log_trace("Wywołano reset_rediractions");
    if (before_redirection_stdin != -1) {
        dup2(before_redirection_stdin, STDIN_FILENO);
        close(before_redirection_stdin);
        before_redirection_stdin = -1;
    }
    if (before_redirection_stdout != -1) {
        dup2(before_redirection_stdout, STDOUT_FILENO);
        close(before_redirection_stdout);
        before_redirection_stdout = -1;
    }
}

void add_process_to_task(List_node *node) {
    Proc *p = (Proc *) malloc(sizeof(Proc));
    p->next = NULL;
    p->pid = -1;

    // tworzymy tablice argv
    int len = list_len(node);
    p->argv = (char **) malloc(sizeof(char *) * (len + 1));
    p->argv[len] = NULL;

    int i = 0;
    for (List_node *tmp = node; tmp != NULL; tmp = tmp->next) {
        p->argv[i] = (char *) malloc(sizeof(char) * (strlen(tmp->str) + 1));
        strcpy(p->argv[i++], tmp->str);
    }

    // dodaj na początek listy
    p->next = proc_head;
    proc_head = p;
}

bool run_builtin(Proc *proc) {
    if (strcmp(proc->argv[0], "pwd") == 0) {
        pwd_cmd();
        return true;
    } else if (strcmp(proc->argv[0], "cd") == 0) {
        cd_cmd(proc->argv);
        return true;
    } else if (strcmp(proc->argv[0], "echo") == 0) {
        echo_cmd(proc->argv);
        return true;
    } else if (strcmp(proc->argv[0], "export") == 0) {
        export_cmd(proc->argv);
        return true;
    } else if (strcmp(proc->argv[0], "unset") == 0) {
        unset_cmd(proc->argv);
        return true;
    } else if (strcmp(proc->argv[0], "exit") == 0) {
        exit(0);
    }
    return false;
}

// rozpoczecie wykonywania zadania
void run_task() {
    pgid = 0;

    char *log = proc_list_convert_to_str();
    log_trace("uruchamiam task: %s", log);
    free(log);

    int fd[2];
    int previous = -1; // wyjscie z poprzedniego procesu w tasku

    for (Proc *tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        // zapamietujemy sobie nasze aktualne stdin i stdout, potem je przywrocimy na macierzystym
        int our_stdin = dup(STDIN_FILENO);
        int our_stdout = dup(STDOUT_FILENO);

        // nie pierwszy
        if (tmp != proc_head) {
            // otwieramy jako stdin wyjscie z poprzedniego potoku
            dup2(previous, STDIN_FILENO);
            close(previous);
        }
        // nie ostatni
        if (tmp->next) {
            pipe(fd);
            // otwieramy stdout jako wejscie utworzonego przez nas potoku
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
        } // ostatni

        // zwroci false, jezeli to nie builtin
        if (!run_builtin(tmp)) {
            pid_t pid;
            // log_trace("Tworze nowy proces dla: %s", tmp->argv[0]);
            pid = fork();
            if (pid == 0) {
                // zamykamy wyjscie utworzonego przez nas potoku, bo z niego bedzie czytal nastepny proces
                if(tmp->next)
                    close(fd[0]);
                // zamykamy deskryptory do zapamietaniu stanu macierzystego
                close(our_stdin);
                close(our_stdout);
              
                run_program(tmp);
            }

            log_trace("Utworzono proces o pidzie %d", pid);

            // jeden raz dla tasku ustawiamy grupe i oddajemy terminal
            if (!pgid) {
                pgid = pid;
                if(setpgid(pid, pgid == -1)){
                    log_error("NIe udalo sie setpgid: %d", strerror(errno));
                }
                if(tcsetpgrp(terminalFD, pgid ) == - 1){
                    log_error("Nie udalo sie przeniesc procesu do foreground: %s", strerror(errno));
                }
                // jezeli proces probowal cos odczytac bedac na bg (przed tcsetpgr) to zostal zatrzymany
                // wysylamy mu SIGCONT zeby sie wznowil
                kill(pgid, SIGCONT);
            }

            tmp->pid = pid;
        }

        if (tmp->next != NULL) {
            // ustawiamy wejscie nastepnego procesu
            previous = fd[0];
        }

        dup2(our_stdout, STDOUT_FILENO);
        dup2(our_stdin, STDIN_FILENO);
        close(our_stdin);
        close(our_stdout);
    }


    for (Proc *tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        // to byla komenda wbudowana
        if (tmp->pid == -1) {
            continue;
        }

        int status = 0;
        log_trace("Czekam na proces o pidzie %d", tmp->pid);

        pid_t x = waitpid(tmp->pid, &status, 0);
        free(last_process_status);
        char *s = malloc(sizeof(char) * 12);
        sprintf(s, "%d", status);
        last_process_status = s;
        if (x != tmp->pid) {
            log_error("Nie udal sie waitpid %d, zwrocil %d: %s",tmp->pid, x, strerror(errno));
        }
        if (WIFEXITED(status)) {
            log_trace("Proces %d zakonczyl kodem: %d", tmp->pid, WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            log_trace("Proces %d zakonczyl sie przez sygnal %d",  tmp->pid, WTERMSIG(status));
        }
    }

    if(tcsetpgrp(terminalFD, shellPID ) == - 1){
        log_trace("Nie udalo sie przeniesc procesu do foreground, mozliwe ze juz bylismy na fg: %s", strerror(errno));
    }

    fflush(stdout);
    fflush(stderr);
}

// prywatna
void free_recursive(Proc *tmp) {
    if (tmp == NULL)
        return;

    free_recursive(tmp->next);

    for (int i = 0; tmp->argv[i] != NULL; i++) {
        free(tmp->argv[i]);
    }
    free(tmp->argv);
    free(tmp);
}

void free_process_list() {
    free_recursive(proc_head);
    proc_head = NULL;
}

char *proc_list_convert_to_str() {
    size_t size = 0;

    for (Proc *tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        size += strlen(tmp->argv[0]) + 1;
    }

    char *result = malloc(sizeof(char) * (size + 1));
    result[0] = '\0';

    for (Proc *tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        strcat(result, tmp->argv[0]);
        strcat(result, "|");
    }
    result[strlen(result) - 1] = '\0'; // usuwamy ostatni znak |, beda dwa \0 ale to nie szkodzi
    return result;
}
