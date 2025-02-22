#ifndef QUEUE_H
#define QUEUE_H

/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/

typedef unsigned int uint;

typedef struct {
    void *items;
    int first;
    int last;
    uint size;
    uint item_size;
    int count;
    void (*item_free)(void *);
} queue;

queue *queue_create(uint size, uint item_size);

int queue_enqueue(queue *q, void *item);

int queue_dequeue(queue *q, void *item);

void queue_free(queue **q);

queue *queue_copy(queue *q);


#endif //QUEUE_H
