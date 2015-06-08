/*
 *  * C Program to Implement a Stack using Linked List
 *   */
#include <stdio.h>
#include <stdlib.h>
#include "stack.h" 

//#define DEBUG_STACK
#ifdef DEBUG_STACK
#define LOG_ERR_STACK(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOG_ERR_STACK(...)
#endif

int nStackCount = 0;
stack_t *top;
#if 0 
void main()
{
    int no, ch, e;
 
    LOG_ERR_STACK("\n 1 - Push");
    LOG_ERR_STACK("\n 2 - Pop");
    LOG_ERR_STACK("\n 3 - Top");
    LOG_ERR_STACK("\n 4 - Empty");
    LOG_ERR_STACK("\n 5 - Exit");
    LOG_ERR_STACK("\n 6 - Dipslay");
    LOG_ERR_STACK("\n 7 - Stack Count");
    LOG_ERR_STACK("\n 8 - Destroy stack");
 
    create();
 
    while (1)
    {
        LOG_ERR_STACK("\n Enter choice : ");
        scanf("%d", &ch);
 
        switch (ch)
        {
        case 1:
            LOG_ERR_STACK("Enter data : ");
            scanf("%d", &no);
            push(no);
            break;
        case 2:
            pop();
            break;
        case 3:
            if (top == NULL)
                LOG_ERR_STACK("No elements in stack");
            else
            {
                e = topelement();
                LOG_ERR_STACK("\n Top element : %d", e);
            }
            break;
        case 4:
            empty();
            break;
        case 5:
            exit(0);
        case 6:
            display();
            break;
        case 7:
            stack_nStackCount();
            break;
        case 8:
            destroy();
            break;
        default :
            LOG_ERR_STACK(" Wrong choice, Please enter correct choice  ");
            break;
        }
    }
}
#endif 

/* Create empty stack */
void createStack()
{
    top = NULL;
}
 
/* Count stack elements */
void stack_count()
{
    LOG_ERR_STACK("\n No. of elements in stack : %d", nStackCount);
}
 
/* Push data into stack */
void push(pixel_t *data, int x, int y)
{
    if (top == NULL)
    {
        top =(stack_t *)malloc(sizeof(struct stack_s));
        top->ptr = NULL;
        top->P = data;
        top->x = x;
        top->y = y;
    }
    else
    {
        stack_t *temp =(stack_t *)malloc(sizeof(struct stack_s));
        temp->ptr = top;
        temp->P = data;
        temp->x = x;
        temp->y = y;
        top = temp;
    }
    nStackCount++;
}
 
/* Display stack elements */
void displayStack()
{
    stack_t *top1 = top;
 
    if (top1 == NULL)
    {
        LOG_ERR_STACK("Stack is empty");
        return;
    }
 
    while (top1 != NULL)
    {
        LOG_ERR_STACK("(%d, %d, %d) ", top1->P->L, top1->P->A, top1->P->B);
        top1 = top1->ptr;
    }
 }
 
/* Pop Operation on stack */
pixel_t* pop(int *x, int *y)
{
    pixel_t *P;
    stack_t *top1 = top;
 
    if (top1 == NULL)
    {
        LOG_ERR_STACK("\n Error : Trying to pop from empty stack");
        return NULL;
    }
    else
        top1 = top1->ptr;
    P = top->P;
    *x = top->x;
    *y = top->y;
    free(top);
    LOG_ERR_STACK("\n Popped pixel at [y,x] -> [%d, %d]: (L,A,B,S,label) -> (%d, %d, %d, %d, %lu)", *y, *x, P->L, P->A, P->B, P->S, P->label);
    top = top1;
    nStackCount--;
    return P;
}
 
/* Return top element */
pixel_t* topelement()
{
    return(top->P);
}
 
/* Check if stack is empty or not */
int isEmpty()
{
    if (top == NULL){
        LOG_ERR_STACK("\n Stack is empty");
        return 0;
    } else {
        LOG_ERR_STACK("\n Stack is not empty with %d elements", nStackCount);
        return 1;
    }
}
 
/* Destroy entire stack */
void destroyStack()
{
    stack_t *top1 = top;
 
    while (top1 != NULL)
    {
        top1 = top->ptr;
        free(top);
        top = top1;
        top1 = top1->ptr;
    }
    free(top1);
    top = NULL;
 
    LOG_ERR_STACK("\n All stack elements destroyed");
    nStackCount = 0;
}
