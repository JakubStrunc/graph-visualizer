
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

/* ____________________________________________________________________________

    stack *stack_create(uint size, uint item_size)

    Creates a new stack with a specified size and item size

    Parameters:
        size - The maximum number of items the stack can hold
        item_size - The size of each item

    Returns:
        A pointer to the created stack or NULL if memory allocation fails

   ____________________________________________________________________________
*/
stack *stack_create(uint size, uint item_size) {

    stack *temp;

    // sanity check
    if(!size || !item_size) return NULL;

    // allocate memory for the stack structure
    temp = (stack *)malloc(sizeof(stack));
    if(!temp) return NULL;

    // initialize parameters
    temp->sp = -1;
    temp->size = size;
    temp->item_size = item_size;

    // allocate memory for stack items
    temp->items = malloc(item_size * size);
    if(!temp->items) {
        free(temp);
        return NULL;
    }

    return temp;

}

/* ____________________________________________________________________________

    int stack_push(stack *s, void *item)

    Adds an item to the top of the stack

    Parameters:
        s - A pointer to the stack
        item - A pointer to the item to add

    Returns:
        1 if the item is successfully added
        0 if the stack is full or invalid parameters are provided
   ____________________________________________________________________________
*/
int stack_push(stack *s, void *item) {

    // sanity check
    if(!s || !item || s->sp >= (int) s->size - 1) return 0;

    s->sp++; // increment the stack pointer

    // copy the item into the stack
    for(int i = 0; i < (int) s->item_size; i++) {
       ((char*) s->items)[s->sp * s->item_size + i] = ((char*) item)[i];
    }

    return 1;
}

/* ____________________________________________________________________________

    int stack_pop(stack *s, void *item)

    Removes the item from the top of the stack and stores it in the provided buffer

    Parameters:
        s - A pointer to the stack
        item - A pointer to a buffer where the removed item will be stored

    Returns:
        1 if the item is successfully removed
        0 if the stack is empty or invalid parameters are provided
   ____________________________________________________________________________
*/
int stack_pop(stack *s, void *item){

    // sanity check
    if(!s || !item || s->sp < 0) return 0;

    // copy the item from the top of the stack to the buffer
    for(int i = 0; i < (int) s->item_size; i++){
       ((char*) item)[i] = ((char*) s->items)[s->sp * s->item_size + i];
    }

    s->sp--; // decrement the stack pointer

    return 1;
}


/* ____________________________________________________________________________

    int stack_peek(stack *s, void *item)

    Retrieves the item from the top of the stack without removing it

    Parameters:
        s - A pointer to the stack
        item - A pointer to a buffer where the peeked item will be stored

    Returns:
        1 if the item is successfully retrieved
        0 if the stack is empty or invalid parameters are provided
   ____________________________________________________________________________
*/
int stack_peek(stack *s, void *item) {

    // sanity check
    if(!s || s->sp < 0) return 0;

    // copy the item from the top of the stack to the buffer
    for(int i = 0; i < (int)s->item_size; i++) {
        ((char *)item)[i] = ((char *)s->items)[s->sp * s->item_size + i];
    }

    return 1;
}

/* ____________________________________________________________________________

    void stack_free(stack **s)

    Frees the memory allocated for the stack and its items

    Parameters:
        s - A double pointer to the stack to be freed

    Returns:
        Nothing. The stack pointer is set to NULL after freeing memory
   ____________________________________________________________________________
*/
void stack_free(stack **s) {

    // sanity check
    if(!s || !*s) return;

    // free memory for stack items and the stack structure
    free((*s)->items);
    free(*s);
    *s = NULL;
}

