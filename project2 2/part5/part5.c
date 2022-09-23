/* CIS 415 Project 2
* Author: Xiangqian Zhang
* due date: Nov-12-2020
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

void signaler (pid_t* pid_ary, int size, int sig) {
    for (int i = 0; i < size; i++ ) {
        printf("Parent process: %d-Sending signal: %d to child process: %d\n", getppid(), sig, (int)(pid_ary[i]));
        kill(pid_ary[i], sig);
    }
}

void pid_info (int pid) {
    char *line = NULL;
    //char* line1 =NULL;
    size_t len = 0;
    int nread;
    char file[128];
    FILE *fp;

    sprintf(file, "/proc/%d/status", pid);
    fp = fopen(file, "r");
    if (fp != NULL) {
          while ((nread = getline(&line, &len, fp)) != -1) {
              //num_tok = token_num(&command_arry, line, " ");
              if (strstr(line, "Name:") != NULL) {
                  printf("%15s", line);
                  }
              else if (strstr(line, "State:") != NULL) {
                  printf("%15s", line);
                  }
              else if (strstr(line, "Pid:") != NULL) {
                  printf("%15s", line);
                  }
            else   if (strstr(line, "PPid:") != NULL) {
                  printf("%15s", line);
                  }
            else   if (strstr(line, "Gid:") != NULL) {
                  printf("%15s", line);
                  }
                //free_arry(&command_arry, num_tok);
          } fclose(fp);
    }
}

void run_time (int pid) {
    char *line = NULL;
    //char* line1 =NULL;
    size_t len = 0;
    int nread;
    char file[128];
    FILE *fp;

    sprintf(file, "/proc/%d/sched", pid);
    fp = fopen(file, "r");
    if (fp != NULL) {
          while ((nread = getline(&line, &len, fp)) != -1) {
              //num_tok = token_num(&command_arry, line, " ");
              if (strstr(line, "se.sum_exec_runtime") != NULL) {
                  printf("%s", line);
                  }

                //free_arry(&command_arry, num_tok);
          } fclose(fp);
    }
}


void scheduler (pid_t* pid_ary, int size, sigset_t set, int sig, int flag) {
    int status;
    int j = 0;
    int arry[size];
    for (int h = 0; h < size; h++) {
        arry[h] = 1;
    }
    int total = size;  //number of unfinished process

    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);

        while(1) {
            //signal(SIGALRM, sig_handler);
          if (flag == 1) {
             alarm(10);
          } else {
              alarm(2);
          }

          if (sigwait(&set, &sig) == 0){
            //printf("SIGALRM is received\n");

            while (1) {
                if (arry[j] != 0) {
                    break;
                } else {
                    j++;
                    j = j%size;
                }
            }

            kill(pid_ary[j], SIGSTOP);
            int m = (j+1)%size;
            //find next availabe process which can continue
            while (1) {
                if (arry[m] != 0) {
                  break;
                } else {
                    m++;
                    m = m%size;
                }
            }
            kill(pid_ary[m], SIGCONT);
            pid_info(pid_ary[m]);
            run_time(pid_ary[m]);
            //printf("pid %d is stopped\n",  pid_ary[(j)%size]);
            waitpid(pid_ary[(j)%size], &status, WUNTRACED | WCONTINUED | WNOHANG /*| WEXITED*/);

            //printf("pid state is %d\n",  state);
            if (WIFEXITED(status)) {

                if (arry[(j)%size] == 0 ) {
                    //printf("childe process %d was terminated before\n", pid_ary[(j)%size]);
                    j = m;
                    continue;

                } else {
                    arry[(j)%size] = 0;
                    total--;
                    j = m;
                    //printf("child process %d just finished\n", pid_ary[(j)%size]);
                }
            } else if (WIFSTOPPED(status)) {
                //printf("arry[%d] is : %d\n", (j)%size, arry[(j)%size]);
                //printf("# of unfinished process is %d after pid_ary[%d] is stopped by signal\n", total, pid_ary[j%size]);

                j = m;
                continue;
              }
              //printf("d is %d :array[j-size] is %d\n", (j)%size, arry[(j)%size]);
              //printf("# of unfinished process: %d\n", total);

              printf("\n");
            if (total == 0) {
                exit(0);
          }
        }
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
    //sigaddset(&set, SIGCHLD);
    //sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);
    int sig;
    int status;
    //int j = 0;
    int n = 0;
    int total_time = 0;
    int flag =0;

    FILE *file = fopen(argv[1], "r");
    while ((nread = getline(&line, &len, file)) != -1) {
        num_tok = token_num(&command_arry, line, " ");

        if (command_arry[2] > 10) {
            n++;
            total_time += atoi(command_arry[2]);
        }


        if (nread == -1) {
            nread = 0;
            continue;
        }
        pid[i] = fork();

        if (pid[i] == 0) {
            if (sigwait(&set, &sig) == 0) {
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
    if (n > 2 && total_time > 20) {
        flag = 1;
    }

    script_print(pid, i);
    signaler(pid, i, SIGUSR1);
    signaler(pid, i, SIGSTOP);
    scheduler(pid, i, set, sig, flag);
    /*for (int s = 0; s < i; s++) {
        pid_info(pid[s]);
    }*/
    //int status;
    /*int ret_val;
    int exit_val;
    int sig_val;
    for (int j = 0; j < i; j++) {
        ret_val = waitpid(pid[j], &status, WUNTRACED | WCONTINUED | WNOHANG  );
        exit_val = WEXITSTATUS(status);
        sig_val = WIFSIGNALED(status);
        printf("%d\n", ret_val);
        printf("%d\n", exit_val);
        printf("sig_val is %d\n", sig_val);
    }*/


    for (int j = 0; j < i; j++) {
        waitpid(pid[j], &status, WEXITED );
    }


    fclose(file);
    free(line);
    exit(0);
}
