#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

/* ____________________________________________________________________________

    queue *queue_create(uint size, uint item_size)

    Creates a new queue with a specified size and item size

    Parameters:
        size - The maximum number of items the queue can hold
        item_size - The size of each item

    Returns:
        A pointer to the created queue or NULL if memory allocation fails

   ____________________________________________________________________________
*/
queue *queue_create(uint size, uint item_size) {

    // sanity check
    if(!size || !item_size) return NULL;

    // allocate memory for the queue structure
    queue *q = (queue *)malloc(sizeof(queue));
    if(!q) return NULL;

    // initialize parameters
    q->size = size;
    q->item_size = item_size;
    q->first = 0;
    q->last = -1;
    q->count = 0;

    // allocate memory for queue items
    q->items = malloc(size * item_size);
    if(!q->items) {
        free(q);
        return NULL;
    }

    return q;
}

/* ____________________________________________________________________________

    int queue_enqueue(queue *q, void *item)

    Adds an item to the end of the queue

    Parameters:
        q - A pointer to the queue
        item - A pointer to the item to add

    Returns:
        1 if the item is successfully added
        0 if the queue is full or invalid parameters are provided
   ____________________________________________________________________________
*/
int queue_enqueue(queue *q, void *item) {

    // sanity check
    if(!q || !item || q->count >= (int)q->size) return 0;

    // increment the index and set the position for the new item
    q->last = (q->last + 1) % (int)q->size;

    // copy the item to the queue
    for(int i = 0; i < (int)q->item_size; i++) {
        ((char *)q->items)[q->last * q->item_size + i] = ((char *)item)[i];
    }

    q->count++; // increase the count of items

    return 1;
}

/* ____________________________________________________________________________

    int queue_dequeue(queue *q, void *item)

    Removes an item from the front of the queue and stores it in the provided buffer

    Parameters:
        q - A pointer to the queue
        item - A pointer to a buffer where the dequeued item will be stored

    Returns:
        1 if the item is successfully removed
        0 if the queue is empty or invalid parameters are provided
   ____________________________________________________________________________
*/
int queue_dequeue(queue *q, void *item) {

    // sanity check
    if(!q || !item || q->count == 0) return 0;

    // copy the item from the queue
    for (int i = 0; i < (int)q->item_size; i++) {
        ((char *)item)[i] = ((char *)q->items)[q->first * q->item_size + i];
    }

    // move the index of the first element and decrease the count
    q->first = (q->first + 1) % (int)q->size;
    q->count--;

    return 1;
}

/* ____________________________________________________________________________

    void queue_free(queue **q)

    Frees the memory allocated for the queue and its items

    Parameters:
        q - A double pointer to the queue to be freed

    Returns:
        Nothing. The queue pointer is set to NULL after freeing memory
   ____________________________________________________________________________
*/
void queue_free(queue **q) {

    // sanity check
    if(!q || !*q) return;

    // free memory for queue items and the queue structure
    free((*q)->items);
    free(*q);
    *q = NULL;
}

/* ____________________________________________________________________________

    queue *queue_copy(queue *original)

    Creates a copy of the given queue

    Parameters:
        original - A pointer to the original queue

    Returns:
        A pointer to the newly created copy of the queue
   ____________________________________________________________________________
*/
queue *queue_copy(queue *original) {

    // sanity check
    if(!original || !original->items) return NULL;

    // create a new queue
    queue *copy = queue_create(original->size, original->item_size);
    if(!copy) return NULL;

    char element;

    // temporary storage for elements
    queue *temp = queue_create(original->size, original->item_size);

    // dequeue elements from the original queue and enqueue them to the copy,
    while(queue_dequeue(original, &element)) {
        queue_enqueue(copy, &element);
        queue_enqueue(temp, &element);
    }

    // restore the original queue
    while(queue_dequeue(temp, &element)) {
        queue_enqueue(original, &element);
    }

    // free the temporary queue
    queue_free(&temp);

    return copy;
}