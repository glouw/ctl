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

#define P
#define T short
#include <deq.h>

#define P
#define T short
#include <lst.h>

#define P
#define T short
#include <que.h>

#define P
#define T short
#include <set.h>

#define P
#define T short
#include <stk.h>

#define P
#define T short
#include <vec.h>

#define P
#define T short
#include <pqu.h>

#include <str.h>

#define P
#define T float
#include <deq.h>

#define P
#define T float
#include <lst.h>

#define P
#define T float
#include <que.h>

#define P
#define T float
#include <set.h>

#define P
#define T float
#include <stk.h>

#define P
#define T float
#include <vec.h>

#define P
#define T float
#include <pqu.h>

#include <str.h>

#define P
#define T double
#include <deq.h>

#define P
#define T double
#include <lst.h>

#define P
#define T double
#include <que.h>

#define P
#define T double
#include <set.h>

#define P
#define T double
#include <stk.h>

#define P
#define T double
#include <vec.h>

#define P
#define T double
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

static int
compare_key_short(short* a, short* b)
{
    return (*a == *b) ? 0 : (*a < *b) ? -1 : 1;
}

static int
compare_short(short* a, short* b)
{
    return *a < *b;
}

static int
compare_key_float(float* a, float* b)
{
    return (*a == *b) ? 0 : (*a < *b) ? -1 : 1;
}

static int
compare_float(float* a, float* b)
{
    return *a < *b;
}

static int
compare_key_double(double* a, double* b)
{
    return (*a == *b) ? 0 : (*a < *b) ? -1 : 1;
}

static int
compare_double(double* a, double* b)
{
    return *a < *b;
}

void
A(void)
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

void
B(void)
{
    deq_short a = deq_short_init();
    vec_short b = vec_short_init();
    lst_short c = lst_short_init();
    que_short d = que_short_init();
    set_short e = set_short_init(compare_key_short);
    stk_short f = stk_short_init();
    str g = str_init("test");
    pqu_short i = pqu_short_init(compare_short);

    deq_short_push_back(&a, 1);
    deq_short_push_front(&a, 1);
    vec_short_push_back(&b, 1);
    lst_short_push_back(&c, 1);
    lst_short_push_front(&c, 1);
    que_short_push(&d, 1);
    set_short_insert(&e, 1);
    stk_short_push(&f, 1);
    pqu_short_push(&i, 1);

    deq_short_pop_back(&a);
    deq_short_pop_front(&a);
    vec_short_pop_back(&b);
    lst_short_pop_back(&c);
    lst_short_pop_front(&c);
    que_short_pop(&d);
    set_short_erase(&e, 1);
    stk_short_pop(&f);
    pqu_short_pop(&i);

    deq_short_free(&a);
    lst_short_free(&c);
    vec_short_free(&b);
    que_short_free(&d);
    set_short_free(&e);
    stk_short_free(&f);
    str_free(&g);
    pqu_short_free(&i);
}

void
C(void)
{
    deq_float a = deq_float_init();
    vec_float b = vec_float_init();
    lst_float c = lst_float_init();
    que_float d = que_float_init();
    set_float e = set_float_init(compare_key_float);
    stk_float f = stk_float_init();
    str g = str_init("test");
    pqu_float i = pqu_float_init(compare_float);

    deq_float_push_back(&a, 1.0);
    deq_float_push_front(&a, 1.0);
    vec_float_push_back(&b, 1.0);
    lst_float_push_back(&c, 1.0);
    lst_float_push_front(&c, 1.0);
    que_float_push(&d, 1.0);
    set_float_insert(&e, 1.0);
    stk_float_push(&f, 1.0);
    pqu_float_push(&i, 1.0);

    deq_float_pop_back(&a);
    deq_float_pop_front(&a);
    vec_float_pop_back(&b);
    lst_float_pop_back(&c);
    lst_float_pop_front(&c);
    que_float_pop(&d);
    set_float_erase(&e, 1.0);
    stk_float_pop(&f);
    pqu_float_pop(&i);

    deq_float_free(&a);
    lst_float_free(&c);
    vec_float_free(&b);
    que_float_free(&d);
    set_float_free(&e);
    stk_float_free(&f);
    str_free(&g);
    pqu_float_free(&i);
}

void
D(void)
{
    deq_double a = deq_double_init();
    vec_double b = vec_double_init();
    lst_double c = lst_double_init();
    que_double d = que_double_init();
    set_double e = set_double_init(compare_key_double);
    stk_double f = stk_double_init();
    str g = str_init("test");
    pqu_double i = pqu_double_init(compare_double);

    deq_double_push_back(&a, 1.0);
    deq_double_push_front(&a, 1.0);
    vec_double_push_back(&b, 1.0);
    lst_double_push_back(&c, 1.0);
    lst_double_push_front(&c, 1.0);
    que_double_push(&d, 1.0);
    set_double_insert(&e, 1.0);
    stk_double_push(&f, 1.0);
    pqu_double_push(&i, 1.0);

    deq_double_pop_back(&a);
    deq_double_pop_front(&a);
    vec_double_pop_back(&b);
    lst_double_pop_back(&c);
    lst_double_pop_front(&c);
    que_double_pop(&d);
    set_double_erase(&e, 1.0);
    stk_double_pop(&f);
    pqu_double_pop(&i);

    deq_double_free(&a);
    lst_double_free(&c);
    vec_double_free(&b);
    que_double_free(&d);
    set_double_free(&e);
    stk_double_free(&f);
    str_free(&g);
    pqu_double_free(&i);
}

int
main(void)
{
    A();
    B();
    C();
    D();
}
