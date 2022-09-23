/* CIS 415 Project 2
* Author: Xiangqian Zhang
* due date: Nov-10-2020
*/

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>    /* for errno handle*/

void script_print(pid_t *pid_ary, int size) {
    FILE* fout;
    fout = fopen("top_script.sh", "w");
    fprintf(fout, "#!/bin/bash\ntop");
    for (int i = 0; i < size; i++) {
        fprintf(fout, " -p %d", (int)(pid_ary[i]));
    }
    fprintf(fout, "\n");
    fclose(fout);
}

int count_tok_number (char *line, char *delimiter) {
    int len = strlen(line);
    int flag = 1;
    int ct = 0;
    do {
        if (line == NULL) {
            break;
        }
        for (int i = 0; i < len - 1; i++) {
            if (line[i] == ' ') {
                flag = 1;
                continue;
            }
            if ((line[i] != ' ') && flag) {
                ct++;
                flag = 0;
            }
        }
    } while(0);
    return ct;
}

/* get # of tokens from each command and save a copy of command in an array for later execvp function */
int token_num (char ***input_arry, char *input, char *delimiter) {
    char *token = NULL;
    char *ptr = NULL;
    int num_tok = 0;
    num_tok = count_tok_number(input, delimiter);

    *input_arry = calloc(num_tok + 1, sizeof(char*));

    token = strtok_r(input, "\n", &ptr);
    token = strtok_r(token, delimiter, &ptr);


    for (int i = 0; i < num_tok ; i++ ) {
        (*input_arry)[i] = calloc(strlen(token) + 1, sizeof(char));
        strcpy((*input_arry)[i], token);
        token = strtok_r(ptr, delimiter, &ptr);
    }
    (*input_arry)[num_tok] = NULL;
    return num_tok;
}

void free_arry (char ***input_arry, int num_tok) {
    for (int i = 0; i < num_tok; i++ ) {
        free((*input_arry)[i]);
    }
    free(*input_arry);
}

int main (int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    char **command_arry = NULL;
    int nread;
    int num_tok = 0;
    int i = 0;
    pid_t pid[BUFSIZ];

    FILE *file = fopen(argv[1], "r");
    while ((nread = getline(&line, &len, file)) != -1) {
        num_tok = token_num(&command_arry, line, " ");

        if (nread == -1) {
            nread = 0;
            continue;
        }
        pid[i] = fork();
        if (pid[i] < 0) {
            printf("error\n");
        }
        if (pid[i] == 0) {
            for (int j = 0; command_arry[j] != NULL; j++) {
                printf("%s ", command_arry[j]);
            }
            execvp(command_arry[0], command_arry);
            exit(0);
        }
        i++;

        free_arry(&command_arry, num_tok);

        }
    script_print(pid, i-1);
    int status;
    for (int j = 0; j < i; j++) {
        waitpid(pid[j], &status, 0);
    }

    fclose(file);
    free(line);
    exit(0);
}
