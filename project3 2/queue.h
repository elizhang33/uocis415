#include <string.h>

#define MAXNAME 20
//#define MAXQUEUES 4

typedef struct mealTicket {
    int ticketNum;
    char* dish_name;
}mt;

typedef struct MTQ {
    char name[MAXNAME];
    struct mealTicket * buffer;
    int head;
    int tail;
    int length;     //number of elements in queue
    //int capacity = 3;  //the size of the queue
}mtq;

mtq * mtq_create(char *nm, int limit) {
    mtq * q = (mtq *)malloc(sizeof(mtq));

    q->buffer = (mt*)malloc(sizeof(mt) * limit);
    strcpy(q->name, nm);
    q->head = 0;
    q->tail = 0;
    q->length = 0;

    return q;
}

mt * mt_create(int ticket_num, char * dn) {
    mt * n = (mt *)malloc(sizeof(mt));
    n->ticketNum = ticket_num;
    n->dish_name = dn;
    return n;
}



int enqueue(mtq * q, mt * MT) {
    int num_elements = 3;
    if (q->length == num_elements) {
        printf("ERROR: Queue is full\n");
        return 0;
    } else {
        q->buffer[q->head].ticketNum = MT->ticketNum;
        q->buffer[q->head].dish_name = MT->dish_name;
        q->head++;
        q->length++;

    }
    return 1;

}

int dequeue(mtq *q, mt * MT) {
    if (q->length == 0) {
        printf("Queue is empty, nothing to pop\n");
        return 0;
    } else {
       MT->ticketNum = q->buffer[q->tail].ticketNum;
       MT->dish_name = q->buffer[q->tail].dish_name;

       q->tail++;
       q->length--;
       // this is for RR
      if ((q->head = q->tail) && q->head != 0 ) {
              q->head = 0;
              q->tail = 0;
          }
    }
    return 1;
}

int isEmpty(mtq * q) {
    if (q == NULL) {
        return 0;
    }
    if (q->length == 0) {
        return 1;
    } else {
        return 0;
    }
}

void destructqueue(mtq * q) {
    /*mt * MT;
    while (!isEmpty(q)) {
        dequeue(q, MT);
        //free(q->buffer);
        free(MT);
    }*/
    free(q->buffer);
    free(q);
}
