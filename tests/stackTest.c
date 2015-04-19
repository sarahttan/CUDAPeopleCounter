#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../peopleCounter.h"

int main()
{
    int no, ch;
    pixel_t *e;
    int temp, temp1;

    printf("\n 1 - Push");
    printf("\n 2 - Pop");
    printf("\n 3 - Top");
    printf("\n 4 - Empty");
    printf("\n 5 - Exit");
    printf("\n 6 - Dipslay");
    printf("\n 7 - Stack Count");
    printf("\n 8 - Destroy stack");
    printf("\n 9 - Create stack");

    createStack();

    while (1)
    {
        printf("\n Enter choice : ");
        scanf("%d", &ch);

        switch (ch)
        {
        case 1:
            printf("Enter data : ");
            scanf("%d", &no);
            pixel_t *p = (pixel_t *)malloc(sizeof(struct pixel_s));
            p->L = no;
            p->A = 0;
            p->B = 0;
            push(p,1,1);
            break;
        case 2:
            pop(&temp, &temp1);
            break;
        case 3:
            if (isEmpty() == 0)
                printf("\n No elements in stack");
            else
            {
                e = topelement();
                printf("\n Top element : (%d, %d, %d)", e->L, e->A, e->B);
            }
            break;
        case 4:
            isEmpty();
            break;
        case 5:
            exit(0);
        case 6:
            displayStack();
            break;
        case 7:
            stack_count();
            break;
        case 8:
            destroyStack();
            break;
        case 9:
            createStack();
            break;
        default :
            printf(" Wrong choice, Please enter correct choice  ");
            break;
        }
    }
    return 0;
}

