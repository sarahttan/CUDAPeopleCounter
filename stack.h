#ifndef STACK
#define STACK
#include "peopleCounter.h"

typedef struct stack_s
{
    pixel_t *P;
    int x;
    int y;
    struct stack_s *ptr;
}stack_t;

pixel_t* topelement();
void push(pixel_t *data, int x, int y);
pixel_t* pop(int *x, int *y);
int isEmpty();
void displayStack();
void destroyStack();
void stack_count();
void createStack();

#endif 
