//
// Created by maciej on 17.01.2020.
//

#include <log.h>
#include <variables.h>
#include "shell.h"
#include <errno.h>

extern int yyparse();

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern YY_BUFFER_STATE yy_scan_string(const char *str);

extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

#define REPLACE 0
#define IGNORE 1

typedef struct cmd_queue {
    struct cmd_queue * prev, * next;
    char * cmd;
    struct cmd_queue * result_insert_element;
    unsigned result_insert_pos;
} Cmd_queue;

typedef struct backquote_parent_command_queue {
    struct backquote_parent_command_queue * next;
    struct cmd_queue * element;
} Backquote_parent_command_queue;

typedef struct cmd_result_queue {
    struct cmd_result_queue * next;
    char * str;
    unsigned insert_pos;
} Cmd_result_queue;

void cmd_queue_append(Cmd_queue * to_be_inserted, Cmd_queue ** preceding_element, Cmd_queue ** list_head) {
    if(*preceding_element == NULL) {
        *list_head = *preceding_element = to_be_inserted;
        to_be_inserted->next = to_be_inserted->prev = NULL;
        return;
    }
    if((*preceding_element)->next == NULL) {
        to_be_inserted->next = NULL;
        to_be_inserted->prev = *preceding_element;
        (*preceding_element)->next = to_be_inserted;
        return;
    }
    to_be_inserted->next = (*preceding_element)->next;
    to_be_inserted->prev = *preceding_element;
    to_be_inserted->prev->next = to_be_inserted;
    to_be_inserted->next->prev = to_be_inserted;
}

/// zanim zaczniemy parsować stringa musimy podstawić wszystkie zmienne
void replace_env_variables(char **str) {

    char * line = *str;
    int state = REPLACE;

    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == '\'') {
            if (state == IGNORE) {
                state = REPLACE;
            } else {
                state = IGNORE;
            }
            continue;
        }

        // zmienna zaczyna sie $
        if (state == REPLACE && line[i] == '$') {

            size_t j = i + 1;

            // zmienna konczy sie spacja, $ lub kiedy konczy sie string
            while (line[j] != '$' && line[j] != ' ' && j < strlen(line)) {
                j++;
            }

            // teraz nazwa naszej zmiennej jest pomiedzy i+1 a j-1
            size_t name_len = (j - 1) - (i + 1) + 1; // rozmiar bez \0
            // dodajemy dodatkowy bajt na \0
            char* name = malloc((name_len + 1) * sizeof(char) );
            strncpy(name, line + i + 1, name_len);
            name[name_len] = '\0';

            char* value = get_variable(name);
            log_trace("Podstawiam zmienna %s na %s", name, value);

            // alokujemy bufor ktorego rozmiar jest wiekszy o tyle o ile value zmiennej jest wieksza od jej wartosci
            char* tmp = malloc(strlen(line) + strlen(value) - strlen(name) + 2); // dodatkowo na $ i \0
            // kopiujemy i poprzednich znakow (bez $)
            strncpy(tmp, line, i);
            // kopiujemy value zmiennej do bufora zaczynajac od pozycji i (od znaku $)
            strncpy(tmp + i, value, strlen(value));
            // kopiujemy pozostala czesc line
            strcpy(tmp + i + strlen(value), line + j);

            free(line);
            free(name);
            line = tmp;

            log_trace("Nowa komenda:%s", line);

            // szukamy dalej od wartosci i + strlen(value)
            i += strlen(value) - 1;

        }
    }
    *str = line;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void redirect_logs(char * filename) {
    FILE * fp = fopen(filename, "a");
    if (fp == NULL) {
        log_error("Nie udalo sie przekierowac logow do pliku: %s", strerror(errno));
    } else {
        log_set_fp(fp);
        log_set_quiet(1);
    }
}

int main(int argc, char **argv, char ** env) {

    redirect_logs("logs.txt");

    log_trace("Inicjalizacja shella");
    initShell();

    while (1) {
        print_prompt();
        char *line = NULL;
        size_t n;
        getline(&line, &n, stdin);

        if (strlen(line) == 1) {
            continue;
        }

        // trim \n at the end
        line[strlen(line) - 1] = '\0';

        Cmd_queue * cmds_head = NULL, * cmds_tail = NULL;
        Cmd_queue * cmd_q_el;
        unsigned char first_backquote_found = 0;
        unsigned buffer_cmd_pos = 0, backquote_buffer_cmd_pos = 0;
        char * cmd_buffer = malloc(sizeof(char) * strlen(line)), * backquote_buffer = malloc(sizeof(char) * strlen(line));
        Backquote_parent_command_queue * bquote_head = NULL;

        unsigned char rewrite = 0;

        for (unsigned i = 0; i < strlen(line); ++i) {

            if(!first_backquote_found) {

                if(line[i] == '\'' || rewrite) {
                    if(line[i] == '\'')
                        rewrite = (rewrite == 1) ? 0 : 1;
                    cmd_buffer[buffer_cmd_pos++] = line[i];
                }
                else if(line[i] == '`')
                    first_backquote_found = 1;

                else if(line[i] == ';') {
                    cmd_q_el = malloc(sizeof(Cmd_queue));
                    cmd_q_el->cmd = malloc(sizeof(char) * (buffer_cmd_pos + 1));
                    strncpy(cmd_q_el->cmd, cmd_buffer, buffer_cmd_pos);
                    cmd_q_el->cmd[buffer_cmd_pos] = '\0';
                    cmd_q_el->result_insert_element = NULL;
                    cmd_queue_append(cmd_q_el, &cmds_tail, &cmds_head);
                    cmds_tail = cmd_q_el;

                    buffer_cmd_pos = 0;

                    Backquote_parent_command_queue * bq_temp = bquote_head, * bq_temp2;
                    while(bq_temp != NULL) {
                        bq_temp->element->result_insert_element = cmd_q_el;
                        bq_temp2 = bq_temp;
                        bq_temp = bq_temp->next;
                        free(bq_temp2);
                    }
                    bquote_head = NULL;
                }
                else
                    cmd_buffer[buffer_cmd_pos++] = line[i];
            }
            else {
                if(line[i] == '\'' || rewrite) {
                    if(line[i] == '\'')
                        rewrite = (rewrite == 1) ? 0 : 1;
                    backquote_buffer[backquote_buffer_cmd_pos++] = line[i];
                }
                else if(line[i] == '`') {
                    first_backquote_found = 0;

                    cmd_q_el = malloc(sizeof(Cmd_queue));
                    cmd_q_el->cmd = malloc(sizeof(char) * (backquote_buffer_cmd_pos + 1));
                    strncpy(cmd_q_el->cmd, backquote_buffer, backquote_buffer_cmd_pos);
                    cmd_q_el->cmd[backquote_buffer_cmd_pos] = '\0';
                    cmd_q_el->result_insert_pos = buffer_cmd_pos;
                    cmd_queue_append(cmd_q_el, &cmds_tail, &cmds_head);
                    cmds_tail = cmd_q_el;

                    Backquote_parent_command_queue * bq_temp = NULL;
                    bq_temp = malloc(sizeof(Backquote_parent_command_queue));
                    bq_temp->element = cmd_q_el;
                    bq_temp->next = bquote_head;
                    bquote_head = bq_temp;

                    backquote_buffer_cmd_pos = 0;
                }
                else if(line[i] == ';') {
                    cmd_q_el = malloc(sizeof(Cmd_queue));
                    cmd_q_el->cmd = malloc(sizeof(char) * (backquote_buffer_cmd_pos + 1));
                    strncpy(cmd_q_el->cmd, backquote_buffer, backquote_buffer_cmd_pos);
                    cmd_q_el->cmd[backquote_buffer_cmd_pos] = '\0';
                    cmd_queue_append(cmd_q_el, &cmds_tail, &cmds_head);
                    cmd_q_el->result_insert_pos = buffer_cmd_pos;
                    cmds_tail = cmd_q_el;

                    Backquote_parent_command_queue * bq_temp = NULL;
                    bq_temp = malloc(sizeof(Backquote_parent_command_queue));
                    bq_temp->element = cmd_q_el;
                    bq_temp->next = bquote_head;
                    bquote_head = bq_temp;

                    backquote_buffer_cmd_pos = 0;
                }
                else
                    backquote_buffer[backquote_buffer_cmd_pos++] = line[i];
            }
        }

        if(first_backquote_found || rewrite)
            parse_error = 1;
        if(parse_error)
            puts("syntax error");

        cmd_q_el = malloc(sizeof(Cmd_queue));
        cmd_q_el->cmd = malloc(sizeof(char) * (buffer_cmd_pos + 1));
        strncpy(cmd_q_el->cmd, cmd_buffer, buffer_cmd_pos);
        cmd_q_el->cmd[buffer_cmd_pos] = '\0';
        cmd_q_el->result_insert_element = NULL;
        cmd_queue_append(cmd_q_el, &cmds_tail, &cmds_head);

        Backquote_parent_command_queue * bq_temp = bquote_head, * bq_temp2;
        while(bq_temp != NULL) {
            bq_temp -> element -> result_insert_element = cmd_q_el;
            bq_temp2 = bq_temp;
            bq_temp = bq_temp -> next;
            free(bq_temp2);
        }
        free(cmd_buffer);
        free(backquote_buffer);

        Cmd_result_queue * cmd_result_head = NULL;
        Cmd_queue * cmd_result_head_element = NULL;

        while(cmds_head != NULL) {
            if(parse_error == 0 && !finish_execution) {
                replace_env_variables(&(cmds_head->cmd));

                int previous_stdout = 1, fd[2];
                if (cmds_head->result_insert_element != NULL) {
                    previous_stdout = dup(STDOUT_FILENO);
                    pipe(fd);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                }

                if(cmds_head == cmd_result_head_element) {
                    unsigned size = strlen(cmds_head->cmd);
                    Cmd_result_queue * temp = cmd_result_head;
                    while(temp) {
                        size += strlen(temp->str);
                        temp = temp -> next;
                    }
                    char * resultant_cmd = calloc(1, sizeof(char) * (size + 1));
                    unsigned base_str_pos = 0;
                    while(cmd_result_head != NULL) {
                        Cmd_result_queue * temp2 = cmd_result_head;
                        temp = cmd_result_head;
                        unsigned first_insert = UINT_MAX;
                        while(temp) {
                            if(temp->insert_pos < first_insert) {
                                temp2 = temp;
                                first_insert = temp->insert_pos;
                            }
                            temp = temp -> next;
                        }

                        strncat(resultant_cmd, (cmds_head->cmd) + base_str_pos, first_insert - base_str_pos);
                        base_str_pos = first_insert;
                        strncat(resultant_cmd, temp2->str, strlen(temp2->str));

                        if(cmd_result_head == temp2) {
                            cmd_result_head = temp2 -> next;
                            free(temp2->str);
                            free(temp2);
                        }
                        else {
                            while (temp->next != temp2)
                                temp = temp->next;
                            temp->next = temp2->next;
                            free(temp2->str);
                            free(temp2);
                        }
                    }
                    strncat(resultant_cmd, (cmds_head->cmd) + base_str_pos, strlen(cmds_head->cmd) - base_str_pos);
                    free(cmds_head->cmd);
                    cmds_head->cmd = resultant_cmd;
                }

                log_trace("Parsuję komendę: %s", cmds_head -> cmd);
                YY_BUFFER_STATE buffer = yy_scan_string(cmds_head -> cmd);
                yyparse();

                if(cmds_head->result_insert_element != NULL) {
                    dup2(previous_stdout, STDOUT_FILENO);
                    close(previous_stdout);
                    char result[4096];
                    unsigned chars_read = 0, a = 0;
                    while (a = read(fd[0], result, sizeof(result) - 1)) {chars_read += a;}
                    close(fd[0]);
                    result[chars_read] = '\0';
                    if(parse_error == 1)
                        printf("%s", result);
                    if(!cmd_result_head_element)
                        cmd_result_head_element = cmds_head->result_insert_element;
                    Cmd_result_queue * temp = cmd_result_head, *temp2 = NULL;
                    while(temp != NULL) {
                        temp2 = temp;
                        temp = temp->next;
                    }
                    temp = malloc(sizeof(Cmd_result_queue));
                    temp->next = NULL;
                    temp->str = malloc(sizeof(char) * (strlen(result) + 1));
                    snprintf(temp->str, 4096, "%s", result);
                    temp->str[strlen(temp->str) - 1] = '\0';
                    temp->insert_pos = cmds_head->result_insert_pos;
                    if(cmd_result_head)
                        temp2->next = temp;
                    else
                        cmd_result_head = temp;
                }

                yy_delete_buffer(buffer);
            }
            cmd_q_el = cmds_head;
            cmds_head = cmds_head -> next;
            free(cmd_q_el->cmd);
            free(cmd_q_el);
        }
        free(line);
        while(cmd_result_head != NULL) {
            Cmd_result_queue * temp = cmd_result_head;
            cmd_result_head = cmd_result_head -> next;
            free(temp->str);
            free(temp);
        }
        if(finish_execution)
            break;
        parse_error = 0;
    }
}

#pragma clang diagnostic pop