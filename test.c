#include <stdlib.h>
#include <stdio.h>

#define P
#define T int 
#include <stk.h>

int main(void)
{
    stk_int a = stk_int_init();
    for(int i = 0; i < 16; i++)
        stk_int_push(&a, rand() % 1024);
    int sum = 0;
    while(!stk_int_empty(&a))
    {   
        int* x = stk_int_top(&a);
        printf("%d\n", *x);
        sum += *x; 
        stk_int_pop(&a);
    }   
    printf("sum: %d\n", sum);
    stk_int_free(&a);
}

