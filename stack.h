#ifndef __STACK__
#define __STACK__
/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/
typedef unsigned int uint;
typedef struct{
    int sp;
    uint size, item_size;
    void *items;
} stack;

stack *stack_create(uint size, uint item_size);

int stack_push(stack *s, void *item);

int stack_pop(stack *s, void *item);

int stack_peek(stack *s, void *item);

void stack_free(stack **s);




#endif