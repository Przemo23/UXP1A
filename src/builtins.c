//
// Created by maciej on 17.01.2020.
//

#include "builtins.h"


int list_add(char *name, int m, char *d) {
    if (list_find(name) != NULL || d == NULL || m < 0 || m > 1)
        return FAILURE;

    Node *tmp = (Node *) malloc(sizeof(Node));

    if (tmp == NULL)      // brak pamieci
        return FAILURE;

    strcpy(tmp->var.name, name);
    tmp->var.mode = m;
    strcpy(tmp->var.data, d);

    tmp->next = head;
    head = tmp;

    return SUCCESS;
}

void list_print() {
    Node *tmp = head;

    while (tmp) {
        printf("%s  %s\n", tmp->var.name, tmp->var.data);
        tmp = tmp->next;
    }
}

Node *list_find(char *name) {
    if (name == NULL)
        return NULL;

    Node *tmp = head;

    while (tmp) {
        if (strcmp(tmp->var.name, name) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

// nalezy w argumencie podac wskaznik na head
void list_removeAll(Node *tmp) {
    if (tmp == NULL)
        return;

    list_removeAll(tmp->next);

    if (head == tmp)
        head = NULL;

    free(tmp);
}

int list_remove(char *name) {
    if (name == NULL || head == NULL)
        return FAILURE;

    Node *tmp = head;
    Node *toRem;

    if (strcmp(tmp->var.name, name) == 0) {
        head = tmp->next;
        free(tmp);
        return SUCCESS;
    }

    while (tmp->next) {
        if (strcmp(tmp->next->var.name, name) == 0) {

            toRem = tmp->next;
            tmp->next = tmp->next->next;
            free(toRem);
            return SUCCESS;
        }
        tmp = tmp->next;
    }
    return FAILURE;
}

int list_change(char *name, int m, char *d) {
    Node *tmp = list_find(name);

    if (tmp == NULL || d == NULL || m < 0 || m > 1)
        return FAILURE;

    strcpy(tmp->var.data, d);
    tmp->var.mode = m;

    return SUCCESS;
}

int do_pwd() {
    char *cwd;
    // PATH_MAX zdefiniowane w pliku naglowkowym limits.h
    char buff[PATH_MAX + 1];

    cwd = getcwd(buff, PATH_MAX + 1);
    if (cwd != NULL) {
        printf(ANSI_COLOR_BLUE "\n%s.\n" ANSI_COLOR_RESET, cwd);
        return SUCCESS;
    } else {
        perror(ANSI_COLOR_RED "pwd error" ANSI_COLOR_RESET);
        return FAILURE;
    }
}

int do_cd(char *fileDir) {
    // jezeli wykonamy cd
    if (fileDir == NULL)
        fileDir = "/home";

    if (chdir(fileDir) == 0) {
        printf("Directory changed to " ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, fileDir);
        return SUCCESS;
    } else {
        fprintf(stderr, ANSI_COLOR_RED "cd erorr\n" ANSI_COLOR_RESET);
        return FAILURE;
    }
    return SUCCESS;
}


int do_ls(char *dirName) {
    DIR *dp;
    struct dirent *wpis;
    struct stat statbufor;

    char *cwd;
    // PATH_MAX zdefiniowane w pliku naglowkowym limits.h
    char buff[PATH_MAX + 1];

    cwd = getcwd(buff, PATH_MAX + 1);


    if (dirName == NULL)
        dirName = cwd;

    // nalezy sprawdzic czy katalog w ogole istnieje
    if ((dp = opendir(dirName)) == NULL) {
        printf(ANSI_COLOR_RED "cannot open dir %s\n" ANSI_COLOR_RESET, dirName);
        return FAILURE;
    }

    chdir(dirName);

    // readdir - zwraca wskaznik do strutury szczegolowo opisujacej nastepny wpis katalogowy ze strumienia katalogu - dp
    while ((wpis = readdir(dp)) != NULL) {
        //  stat() sluzy do pobierania statusu pliku przechowywanego w jego i-wezle
        stat(wpis->d_name, &statbufor);
        if (S_ISDIR(statbufor.st_mode)) {
            // znaleziony katalog ale pomijamy . oraz ..
            if (strcmp(".", wpis->d_name) == 0 || strcmp("..", wpis->d_name) == 0)
                continue;

            printf(ANSI_COLOR_BLUE "%s  " ANSI_COLOR_RESET, wpis->d_name);
        } else {
            // pliki zwykle
            printf(ANSI_COLOR_GREEN "%s  " ANSI_COLOR_RESET, wpis->d_name);
        }
    }
    printf("\n");
    chdir(cwd);
    //closedir(dp);
    return SUCCESS;
}

int do_mkdir(char *dirName) {
    // prawa read/write/search dla wszystkich
    if (mkdir(dirName, S_IRWXU | S_IRWXG | S_IWOTH | S_IROTH | S_IXOTH) == -1) {
        printf(ANSI_COLOR_RED "cannot create directory $s\n" ANSI_COLOR_RESET, dirName);
        return FAILURE;
    } else {
        printf("create directory " ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, dirName);
        return SUCCESS;
    }
}

int do_rmdir(char *dirName) {
    if (rmdir(dirName) == -1) {
        printf(ANSI_COLOR_RED "cannot remove directory %s\n" ANSI_COLOR_RESET, dirName);
        return FAILURE;
    } else {
        printf("remove directory " ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, dirName);
        return SUCCESS;
    }
}

int do_touch(char *fileName) {
    int dp = open(fileName, O_CREAT | O_EXCL, 0666);
    if (dp <= 0) {
        printf(ANSI_COLOR_RED "cannot create file %s\n" ANSI_COLOR_RESET, fileName);
        return FAILURE;
    } else {
        printf("create file " ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, fileName);
        close(dp);
        return SUCCESS;
    }
}

int do_rm(char *fileName) {
    if (remove(fileName) == -1) {
        printf(ANSI_COLOR_RED "cannot remove file %s\n" ANSI_COLOR_RESET, fileName);
        return FAILURE;
    } else {
        printf("remove file " ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, fileName);
        return SUCCESS;
    }
}

// kopiowanie blokami - duzo szybsze niz kopiowanie bajt po bajcie
// z kopiowaniem katalogu na razie sie nie bawie bo duzoo roboty - rekurencja i te sprawy
int do_cp(char *fileFrom, char *fileTo) {
    char blok[1024];

    int wej, wyj;
    int nwczyt;

    if (fileFrom == NULL || fileTo == NULL) {
        printf(ANSI_COLOR_RED "cannot copy\n" ANSI_COLOR_RESET);
        return FAILURE;
    }

    wej = open(fileFrom, O_RDONLY);
    wyj = open(fileTo, O_WRONLY | O_CREAT, 0666);

    if (wej <= 0 || wyj <= 0) {
        printf(ANSI_COLOR_RED "cannot copy\n" ANSI_COLOR_RESET);
        return FAILURE;
    }
//printf("%d \n", read(wej,blok, sizeof(blok)));
    while ((nwczyt = read(wej, blok, sizeof(blok))) > 0)
        write(wyj, blok, nwczyt);

    close(wej);
    close(wyj);

    printf("copy completed\n");
    return SUCCESS;
}

// pewnie do zmiany...
void do_echo(char *buf) {
    if (buf == NULL || strlen(buf) == 0) {
        printf("\n");
        return;
    }
    if (buf[0] == '$') {
        memmove(buf, buf + 1, strlen(buf));
        printf("%s\n", get_variable(buf));
    } else
        printf("%s\n", buf);
}

int init_variable() {
    // pobiera strukture reprezentujaca uzytkownika o podanym UID
    struct passwd *pass = getpwuid(getuid());
    // pobranie nazwy użytkownika
    strcpy(user, pass->pw_name);
    // pobiera sieciowa nazwe komputera
    gethostname(computer, 128);

    char *var[4] = {"PATH", "HOME", "USER", "PWD"};     // PS1 u mnie nie dziala
    int i;
    for (i = 0; i < 4; i++)
        list_add(var[i], 0, getenv(var[i]));

    refresh_prompt();

    return SUCCESS;
}

void refresh_prompt() {
    strcpy(commandPrompt, user);
    strcpy(commandPrompt + strlen(user), "@");
    strcpy(commandPrompt + strlen(user) + 1, computer);
    strcpy(commandPrompt + strlen(user) + 1 + strlen(computer), ":");

    char *homeDir = get_variable("HOME");
    char curPath[128];
    getcwd(curPath, 128);


    if (strstr(curPath, homeDir) != NULL) {
        strcpy(commandPrompt + strlen(commandPrompt), "~");
        strcpy(commandPrompt + strlen(commandPrompt), curPath + strlen(homeDir));
        strcpy(commandPrompt + strlen(commandPrompt), "$");
    } else {
        strcpy(commandPrompt + strlen(user) + 3 + strlen(computer), curPath);
        strcpy(commandPrompt + strlen(commandPrompt), "$");
    }
}

char *get_variable(char *name) {
    Node *tmp = list_find(name);
    if (tmp != NULL)
        return tmp->var.data;
    else {
        printf(ANSI_COLOR_RED "cannot get variable\n" ANSI_COLOR_RESET);
        return "";
    }
}

int set_variable(char *name, int mode, char *data) {
    if (strcmp(name, "USER") == 0 || strcmp(name, "CWD") == 0) {
        printf(ANSI_COLOR_RED "cannot overwrite %s\n" ANSI_COLOR_RESET, name);
        return FAILURE;
    }

    if (list_find(name) == NULL)
        return list_add(name, mode, data);
    else
        return list_change(name, mode, data);
}

int rm_variable(char *name) {
    return list_remove(name);
}

void rm_allVariable() {
    return list_removeAll(head);
}

