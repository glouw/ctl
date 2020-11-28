#define P
#define T int
#include <deq.h>

#define P
#define T int
#include <lst.h>

#define P
#define T int
#define U int
#include <map.h>

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

#include <str.h>

int main(void)
{
    deq_int a = deq_int_init();
    lst_int c = lst_int_init();
    vec_int b = vec_int_init();
    que_int d = que_int_init();
    set_int e = set_int_init();
    stk_int f = stk_int_init();
    vec_int g = vec_int_init();
    str h = str_init();
    map_int_int i = map_int_int_init();
    deq_int_free(&a);
    lst_int_free(&c);
    vec_int_free(&b);
    que_int_free(&d);
    set_int_free(&e);
    stk_int_free(&f);
    vec_int_free(&g);
    str_free(&h);
    map_int_int_free(&i);
}
