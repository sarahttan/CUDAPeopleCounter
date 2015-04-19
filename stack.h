#ifndef STACK
#define STACK
#include "peopleCounter.h"

// IMPLEMENTATION WAS VARIED AND IS NOW UNTESTED

typedef struct stack_s
{
    pixel_t *P;
    struct stack_s *ptr;
}stack_t;

pixel_t* topelement();
void push(pixel_t *data);
pixel_t* pop();
int isEmpty();
void display();
void destroy();
void stack_count();
stack_t *create();

#endif 
