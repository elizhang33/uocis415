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
#include "token_func.h"

void signaler (pid_t* pid_ary, int size, int sig) {
    for (int i = 0; i < size; i++ ) {
        printf("Parent process: %d-Sending signal: %d to child process: %d\n", getppid(), sig, (int)(pid_ary[i]));
        kill(pid_ary[i], sig);
    }
}

int main (int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    char **command_arry = NULL;
    int nread;
    int num_tok = 0;
    int i = 0;
    pid_t pid[BUFSIZ];

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);
    int sig;

    FILE *file = fopen(argv[1], "r");
    while ((nread = getline(&line, &len, file)) != -1) {
        num_tok = token_num(&command_arry, line, " ");

        if (nread == -1) {
            nread = 0;
            continue;
        }
        pid[i] = fork();

        if (pid[i] == 0) {
            printf("Child Process: %d-Waiting for SIGUSR1...\n", getpid());
            if (sigwait(&set, &sig) == 0) {
                printf("Child Process: %d-Received signal: %d-Calling exec()\n", getpid(), sig);
                if (execvp(command_arry[0], command_arry) == -1) {
                    perror("execv");
                }
                exit(0);
            } else {
                perror("sigwait");
            }
        }
        i++;
        free_arry(&command_arry, num_tok);
        }
    sleep(3);
    signaler(pid, i, SIGUSR1);
    sleep(3);
    signaler(pid, i, SIGSTOP);
    sleep(5);
    signaler(pid, i, SIGCONT);
    sleep(3);
    signaler(pid, i, SIGINT);  /*SIGINT VS SIGTERM  */
    int status;
    int ret_val;
    int exit_val;
    for (int j = 0; j < i; j++) {
        ret_val = waitpid(pid[j], &status, WUNTRACED | WEXITED );
        /*exit_val = WEXITSTATUS(status);
        printf("%d\n", ret_val);
        printf("%d\n", exit_val);*/
    }

    fclose(file);
    free(line);
    exit(0);
}
