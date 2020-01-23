//
// Created by maciej on 18.01.2020.
//

#include <task.h>
#include <sys/wait.h>
#include <errno.h>


// prywatna
// nie wraca (exec)
void runProgram(Proc *proc) {

    // todo zmienic na execvpe
    execvp(proc->argv[0], proc->argv);
    log_trace("Nie udało się uruchomić zadania %s", proc->argv[0] );
    exit(7);
}

void init_task() {
    log_trace("Wywołano init_task");
    first_process_in_fd = STDIN_FILENO;
    last_process_out_fd = command_out_fd;
}

void add_process_to_task(List_node * node) {
    Proc *p = (Proc *) malloc(sizeof(Proc));
    p->next = NULL;

    // tworzymy tablice argv
    int len = list_len(node);
    p->argv = (char **) malloc(sizeof(char *) * (len + 1));
    p->argv[len]= NULL;

    int i = 0;
    for(List_node* tmp = node; tmp != NULL; tmp= tmp->next){
        p->argv[i] = (char*) malloc(sizeof(char) * (strlen(tmp->str) + 1));
        strcpy(p->argv[i++], tmp->str);
    }

    // dodaj na początek listy
    p->next = proc_head;
    proc_head = p;
}

// rozpoczecie wykonywania zadania
void run_task() {
    log_trace("uruchamiam task: %s", proc_head->argv[0]);
    int fd[2];
    int previous = -1; // wyjscie z poprzedniego procesu w tasku

    // czysczenie bufora wyniku
    // memset(result, '\0', 1024);

    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        // jezeli nie jestesmy ostatni to tworzymy potok, z ktorym bedziemy komunikowac sie z nastepnym procesem
        if (tmp->next != NULL) {
            pipe(fd);
        }

        pid_t pid;

        log_trace("Tworze nowy proces dla: %s", tmp->argv[0]);
        pid = fork();
        if (pid == 0) {
            // pierwszy
            if (tmp == proc_head) {
                if (first_process_in_fd != STDIN_FILENO) {
                    dup2(first_process_in_fd, STDIN_FILENO);
                    close(first_process_in_fd);
                }
            } // nie pierwsze
            else {
                // otwieramy jako stdin wyjscie z poprzedniego potoku
                dup2(previous, STDIN_FILENO);
                close(previous);
            }
            // nie ostatni
            if(tmp->next) {
                // otwieramy stdout jako wejscie utworzonego przez nas potoku
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                // zamykamy wyjscie utworzonego przez nas potoku, bo z niego bedzie czytal nastepny proces
                close(fd[0]);
            } // ostatni
            else{
                if (last_process_out_fd != STDOUT_FILENO) {
                    dup2(last_process_out_fd, STDOUT_FILENO);
                    close(last_process_out_fd);
                }
            }
            runProgram(tmp);
        }
        log_trace("Utworzono proces o pidzie %d", pid);
        tmp->pid = pid;
//        if (!pgid)
//            pgid = pid;
//        setpgid(pid, pgid);

        // nie pierwszy
//        if(tmp != proc_head)
//            close(previous);
        if(tmp->next != NULL) {
            // ustawiamy wejscie nastepnego potoku
            previous = fd[0];
        }
        // todo trzeba pozamykac fd 0 i fd 1 w macierzystym
    }


//    while (read(pipefd[0], result, sizeof(result)) != 0)
//    {
//    }
//
//    printf("%s",result);

    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        // WUNTRACED
        int status = 0;
        log_trace("Czekam na proces o pidzie %d", tmp->pid);

        pid_t x = waitpid(tmp->pid, &status, 0);
        if(x != tmp->pid) {
            log_error("Nie udal sie waitpid, zwrocil %d: %s", x,strerror(errno));
        }
        if (WIFEXITED(status)) {
            log_trace("Proces zakonczyl kodem: %d",WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            log_trace("Proces zakonczyl sie przez sygnal %d\n",WTERMSIG(status));
        }
    }
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
