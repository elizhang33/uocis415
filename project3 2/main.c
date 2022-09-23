/*=============================================================================
 * Program Name: project3
 * Author: Xiangqian Zhang
 * Due Date: 12/5/2020
 * Description: The objective is to get experience with a combination of OS techniques 
 * in your solution, mainly threading, synchronization, and file I/O.
 *
 *===========================================================================*/

//========================== Preprocessor Directives ==========================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "queue.h"
#include "string_parser.h"
//=============================================================================

//================================= Constants =================================
#define MAXNAMES 15
#define MAXTOPICS 4
#define URLSIZE 100
#define CAPSIZE 50
#define MAXTENTRIES 3
#define NUMPROXIES	10

#define MAXentryNum 20
#define MAXPUBS 4
#define MAXSUBS 4
//=============================================================================

//============================ Structs and Macros =============================
//struct timeval timeStamp;

typedef struct topicEntry{
	int entryNum;
	struct timeval timeStamp;
	int pubID;
	char photoURL[URLSIZE];
	char photoCaption[CAPSIZE];
} topicEntry;

typedef struct TQ {
	char name[MAXNAMES];
	struct topicEntry *buffer;
	int head;
	int tail;
	int length;
	int entry_counter;
	int max_length;
	pthread_mutex_t struct_mutex;
} TQ;

typedef struct pub_args {
		struct topicEntry  *buffer;
		char *TQ_ID;
		pthread_t tid;
}pub_args;

typedef struct sub_args {
		char *TQ_ID;
		pthread_t tid;
		struct topicEntry TE;
}sub_args;

TQ * registry[MAXTOPICS]; 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cond1_mutex = PTHREAD_MUTEX_INITIALIZER;


TQ *init(char *TQ_ID, int max_length) {
    TQ * q = (TQ *)malloc(sizeof(TQ));
		q->buffer = (topicEntry*)malloc(sizeof(topicEntry)*MAXTENTRIES);

		strcpy(q->name, TQ_ID);
		q->head = 0;
		q->tail = 0;
		q->length = 0;
		q->entry_counter = 1; 
		q->max_length = max_length;

		return q;
}

void freeTQ(int pos, char *TQ_ID) {
		free(registry[pos]->buffer);
		free (registry[pos]);
}

//=============================================================================

//================================= Functions =================================
int TQ_enqueue(char *TQ_ID, topicEntry *TE) {
	
	for (int i = 0; i < 4; i++) {
        if (strcmp(registry[i]->name, TQ_ID) == 0) {
          if (registry[i]->length == registry[i]->max_length) {
              printf("pushing: Queue: %s - ERROR Queue is full\n", registry[i]->name);
              return 0;
          }
			pthread_mutex_lock(&(registry[i]->struct_mutex));
				
         	registry[i]->tail = (registry[i]->tail + 1) % (registry[i]->max_length);
         	registry[i]->length++;
         	registry[i]->buffer[registry[i]->tail].entryNum = registry[i]->entry_counter++;
            //registry[i]->buffer[registry[i]->tail].timeStamp = gettimeofday(&timeStamp, NULL);
		    //registry[i]->buffer[registry[i]->head].pubID = TE->pubID;
            //strcpy(registry[i]->buffer[registry[i]->head].photoURL, TE->photoURL);
            //strcpy(registry[i]->buffer[registry[i]->head].photoCaption, TE->photoCaption);   

			pthread_mutex_unlock(&(registry[i]->struct_mutex));
        }
    }
    return 1;
}

int TQ_dequeue(char *TQ_ID, int entryNum, topicEntry *TE) {

	for (int i = 0; i < 4; i++) {

        if (strcmp(registry[i]->name, TQ_ID) == 0) {
          if (registry[i]->length == 0) {
              printf("poping: Queue: %s - Queue is empty, nothing to pop\n", registry[i]->name);
              return 0;
          }
			pthread_mutex_lock(&(registry[i]->struct_mutex));

            TE->entryNum = registry[i]->buffer[registry[i]->head].entryNum;
            //TE->timeStamp = registry[i]->buffer[registry[i]->head].timeStamp;
            //TE->pubID = registry[i]->buffer[registry[i]->head].pubID;
            //strcpy(TE->photoURL, registry[i]->buffer[registry[i]->head].photoURL);
            //strcpy(TE->photoCaption, registry[i]->buffer[registry[i]->head].photoCaption);
            registry[i]->head++;
            registry[i]->length--;

			pthread_mutex_unlock((&registry[i]->struct_mutex));
        }
    }

	return 1;
}

void *publisher(void * arg) {
	
		/*printf("type: publisher, thread_ID: %ld\n", pthread_self());

		pthread_mutex_lock(&cond1_mutex);
		pthread_cond_wait(&cond1, &cond1_mutex);
		pthread_mutex_unlock(&cond1_mutex);

		for (int i = 0; i < MAXTENTRIES; i++) {
				TQ_enqueue(pa->TQ_ID, &(pa->buffer[i]));
				sched_yield();
				sleep(1);
				}*/

	FILE *pub_in = (FILE*)arg;
	//FILE *pub_in = fopen(pub, "r");
	command_line command;

	size_t len = 200;
	char* line_buf = malloc (len);

	printf("Hi\n");

	while (getline (&line_buf, &len, pub_in) != -1)
	{
		command = str_filler (line_buf, " ");

		for (int i = 0; command.command_list[i] != NULL; i++)
		{
			printf ("%s\n", command.command_list[i]);
		}

		printf("\n");

		free_command_line (&command);
		memset (&command, 0, 0);
	}
	free (line_buf);
	//fclose(pub_in);
}

void *subscriber(void *arg) {
		FILE *sa = arg;
		printf("type: subscriber, thread_ID: %ld\n", pthread_self());

		pthread_mutex_lock(&cond1_mutex);
		pthread_cond_wait(&cond1, &cond1_mutex);
		pthread_mutex_unlock(&cond1_mutex);

		/*for (int i = 0; i < MAXTENTRIES; i++) {
				if (TQ_dequeue(sa->TQ_ID, i, &(sa->TE)) != 0) {
						//printf("Hello from subscriber thread\n");
						printf("entry num: %d, pubID: %s, thread_ID: %ld\n", sa->TE.entryNum, sa->TE.pubID, pthread_self());
						sched_yield();
						sleep(1);
				} else {
						sleep(1);
				}
		}*/
}

int queue_empty(TQ * q) {
	if (q == NULL) {
		return -1;
	} else if (q->length == 0) {
		return 1;
	} else {
		return 0;
	}
}

int queue_full(TQ * q) {
	if (q == NULL) {
		return -1;
	} else if (q->length == q->max_length) {
		return 1;
	} else {
		return 0;
	}
}

/*int getEntry (int lastEntry, topicEntry * TE) {
	for (int i = 0; i < MAXTOPICS; i++) {
		if (queue_empty(registry[i])) {
			return 0;
		} else if (lastEntry + 1 < registry[i]->length)
	}

}*/

//=============================================================================

//=============================== Program Main ================================
int main(int argc, char *argv[]) {
	//Variables Declarations
	/*char *qNames[] = {"Breakfast", "Lunch", "Dinner", "Bar"};
	char *bFood[] = {"Eggs", "Bacon", "Steak"};
	char *lFood[] = {"Burger", "Fries", "Coke"};
	char *dFood[] = {"Steak", "Greens", "Pie"};
	char *brFood[] = {"Whiskey", "Sake", "Wine"};
	//int i, j, t = 1;
	topicEntry *test[4];
	char dsh[] = "Empty";
	topicEntry bfast[3] = {[0].entryNum = bFood[0], [1].entryNum = bFood[1], [2].entryNum = bFood[2]};
	topicEntry lnch[3] = {[0].entryNum = lFood[0], [1].entryNum = lFood[1], [2].entryNum = lFood[2]};
	topicEntry dnr[3] = {[0].entryNum = dFood[0], [1].entryNum = dFood[1], [2].entryNum = dFood[2]};
	topicEntry br[3] = {[0].entryNum = brFood[0], [1].entryNum = brFood[1], [2].entryNum = brFood[2]};
	topicEntry entryNum = {.entryNum=0};
	test[0] = bfast;
	test[1] = lnch;
	test[2] = dnr;
	test[3] = br;*/

	command_line command;

	size_t len = 64;
	char* line_buf = malloc (len);

	FILE* in = fopen ("input.txt", "r");

	int i = 0;

	pthread_t pub_tid[NUMPROXIES/2];
	//int pub_arry[NUMPROXIES/2] = {0};
	pthread_t sub_tid[NUMPROXIES/2];
	//int sub_arry[NUMPROXIES/2] = {0};

	while (getline (&line_buf, &len, in) != -1)
	{
		command = str_filler (line_buf, " ");

		for (int i = 0; command.command_list[i] != NULL; i++)
		{
			printf ("%s\n", command.command_list[i]);
		}

		// to regcoginize the command
		// create queue
		if (strcmp(command.command_list[0], "create") == 0) {
			char * name = command.command_list[3];
			int thread_num = command.command_list[4];
			//TQ * name = init(command.command_list[3], thread_num);
			registry[i] = init(name, thread_num);
			printf("name: %s\n", name);
			printf("registry: %d %s\n", i, registry[i]->name);
			i++;
			
		}

		//add publisher
		if (strcmp(command.command_list[0], "add") == 0 && (strcmp(command.command_list[1], "publisher") == 0)) {
			FILE *pub_in = fopen(command.command_list[2], "r");
			printf("hello\n");
			//char * pub_file_name = command.command_list[2];
			//printf("pub_file_name: %s\n", pub_file_name);
			//FILE *fd = fopen(command.command_list[2], "r");
			//pthread_create(pub_tid+i, NULL, publisher, pub_file_name);
			pthread_create(pub_tid+i, NULL, publisher, pub_in);

			fclose(pub_in);
		}


		printf("\n"); 

		free_command_line (&command);
		memset (&command, 0, 0);
	}
	free (line_buf);




	//STEP-1: Initialize the registry
	/*for (int i = 0; i < MAXTOPICS; i++) {
			registry[i] = init(qNames[i]);
	}

	pthread_t pub_tid[4];
	pthread_t sub_tid[4];

	pub_args pa[4];
	sub_args sa[4];

	//STEP-2: Create the publisher thread-pool
	for (int i = 0; i < 4; i++) {
			pa[i].buffer = test[i];
			pa[i].TQ_ID = qNames[i];
			pthread_create(pub_tid+i, NULL, publisher, pa + i);

	}

	//sleep(5);
	for (int i = 0; i < 4; i++) {
			sa[i].TQ_ID = qNames[i];
			sa[i].tid = pthread_self();
			sa[i].TE = entryNum;
			pthread_create(sub_tid+i, NULL, subscriber, sa + i);
	}

	sleep(3);

	pthread_mutex_lock(&cond1_mutex);
	pthread_cond_broadcast(&cond1);
	pthread_mutex_unlock(&cond1_mutex);

	//STEP-4: Join the thread-pools
	for (int j = 0; j < 4; j++) {
			pthread_join(*(pub_tid + j), NULL);
			}

	for (int j = 0; j < 4; j++) {
			pthread_join(*(sub_tid + j), NULL);
			}

	//STEP-5: Free the registry
	for (int i = 0; i < MAXTOPICS; i++) {
			freeTQ(i, qNames[i]);
	}*/

	fclose(in);

	return EXIT_SUCCESS;
}
//=============================================================================
