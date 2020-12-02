#define P
#define T int
#include <deq.h>

#define P
#define T int
#include <lst.h>

#define P
#define T int
#include <que.h>

#define P
#define T int
#include <set.h>

#define P
#define T int
#include <stk.h>

#define P
#define T int
#include <vec.h>

#define P
#define T int
#include <pqu.h>

#include <str.h>

static int
compare_key_int(int* a, int* b)
{
    return (*a == *b) ? 0 : (*a < *b) ? -1 : 1;
}

static int
compare_int(int* a, int* b)
{
    return *a < *b;
}

int main(void)
{
    deq_int a = deq_int_init();
    vec_int b = vec_int_init();
    lst_int c = lst_int_init();
    que_int d = que_int_init();
    set_int e = set_int_init(compare_key_int);
    stk_int f = stk_int_init();
    str g = str_init("test");
    pqu_int i = pqu_int_init(compare_int);

    deq_int_push_back(&a, 1);
    deq_int_push_front(&a, 1);
    vec_int_push_back(&b, 1);
    lst_int_push_back(&c, 1);
    lst_int_push_front(&c, 1);
    que_int_push(&d, 1);
    set_int_insert(&e, 1);
    stk_int_push(&f, 1);
    pqu_int_push(&i, 1);

    deq_int_pop_back(&a);
    deq_int_pop_front(&a);
    vec_int_pop_back(&b);
    lst_int_pop_back(&c);
    lst_int_pop_front(&c);
    que_int_pop(&d);
    set_int_erase(&e, 1);
    stk_int_pop(&f);
    pqu_int_pop(&i);

    deq_int_free(&a);
    lst_int_free(&c);
    vec_int_free(&b);
    que_int_free(&d);
    set_int_free(&e);
    stk_int_free(&f);
    str_free(&g);
    pqu_int_free(&i);
}
