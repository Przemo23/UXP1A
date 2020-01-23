//
// Created by maciej on 18.01.2020.
//

#include <task.h>


// prywatna
// nie wraca (exec)
void runProgram(Proc *proc) {

    // todo zmienic na execvpe
    execvp(proc->argv[0], proc->argv);
    exit(-1);
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
    log_trace("uruchamiam task:%s\n", proc_head->argv[0]);
    int fd[2], out = -1;
    pipe(fd);
    int pipefd[2];
    pipe(pipefd);

    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        if (tmp->next)
            pipe(fd);

        pid_t pid;
        pid = fork();
        if (pid == 0) {
            if(tmp != proc_head) {
                dup2(out, STDIN_FILENO);
                close(out);
            }
            if(tmp->next) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            else{
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            runProgram(tmp);
        }
        tmp->pid = pid;
        if (!pgid)
            pgid = pid;
        setpgid(pid, pgid);
        if(tmp != proc_head)
            close(out);
        if(tmp->next) {
            out = fd[0];
            close(fd[1]);
        }
    }

    close(pipefd[1]);  // close the write end of the pipe in the parent

    while (read(pipefd[0], result, sizeof(result)) != 0)
    {
    }

    printf("%s",result);

    for (Proc* tmp = proc_head; tmp != NULL; tmp = tmp->next) {
        waitpid(tmp->pid, NULL, WUNTRACED);
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
