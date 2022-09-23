#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token_func.h"




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
