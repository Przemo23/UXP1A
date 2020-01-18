//
// Created by maciej on 17.01.2020.
//

#include <log.h>
#include "builtins.h"
#include "variables.h"
#include "shell.h"
#include "defines.h"


int do_pwd() {
    char *cwd;
    // PATH_MAX zdefiniowane w pliku naglowkowym limits.h
    char buff[PATH_MAX + 1];

    cwd = getcwd(buff, PATH_MAX + 1);
    if (cwd != NULL) {
        printf(ANSI_FG_BLUE "\n%s.\n" ANSI_RESET, cwd);
        return SUCCESS;
    } else {
        perror(ANSI_FG_RED "pwd error" ANSI_RESET);
        return FAILURE;
    }
}

int do_cd(char *fileDir) {
    // jezeli wykonamy cd
    if (fileDir == NULL)
        fileDir = "/home";

    if (chdir(fileDir) == 0) {
        printf("Directory changed to " ANSI_FG_BLUE "%s\n" ANSI_RESET, fileDir);
        return SUCCESS;
    } else {
        fprintf(stderr, ANSI_FG_RED "cd erorr\n" ANSI_RESET);
        return FAILURE;
    }
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
        printf(ANSI_FG_RED "cannot open dir %s\n" ANSI_RESET, dirName);
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

            printf(ANSI_FG_BLUE "%s  " ANSI_RESET, wpis->d_name);
        } else {
            // pliki zwykle
            printf(ANSI_FG_GREEN "%s  " ANSI_RESET, wpis->d_name);
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
        printf(ANSI_FG_RED "cannot create directory $s\n" ANSI_RESET, dirName);
        return FAILURE;
    } else {
        printf("create directory " ANSI_FG_BLUE "%s\n" ANSI_RESET, dirName);
        return SUCCESS;
    }
}

int do_rmdir(char *dirName) {
    if (rmdir(dirName) == -1) {
        printf(ANSI_FG_RED "cannot remove directory %s\n" ANSI_RESET, dirName);
        return FAILURE;
    } else {
        printf("remove directory " ANSI_FG_BLUE "%s\n" ANSI_RESET, dirName);
        return SUCCESS;
    }
}

int do_touch(char *fileName) {
    int dp = open(fileName, O_CREAT | O_EXCL, 0666);
    if (dp <= 0) {
        printf(ANSI_FG_RED "cannot create file %s\n" ANSI_RESET, fileName);
        return FAILURE;
    } else {
        printf("create file " ANSI_FG_GREEN "%s\n" ANSI_RESET, fileName);
        close(dp);
        return SUCCESS;
    }
}

int do_rm(char *fileName) {
    if (remove(fileName) == -1) {
        printf(ANSI_FG_RED "cannot remove file %s\n" ANSI_RESET, fileName);
        return FAILURE;
    } else {
        printf("remove file " ANSI_FG_GREEN "%s\n" ANSI_RESET, fileName);
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
        printf(ANSI_FG_RED "cannot copy\n" ANSI_RESET);
        return FAILURE;
    }

    wej = open(fileFrom, O_RDONLY);
    wyj = open(fileTo, O_WRONLY | O_CREAT, 0666);

    if (wej <= 0 || wyj <= 0) {
        printf(ANSI_FG_RED "cannot copy\n" ANSI_RESET);
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


