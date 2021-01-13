#include "../test.h"

#include <str.h> // MULTIPLE INCLUDES OKAY.
#include <str.h>
#include <str.h>
#include <str.h>

#define P
#define T int
#include <ust.h>

#define P
#define T float
#include <ust.h>

size_t
int_hash(int* x)
{ return abs(*x); }

int
int_equal(int* a, int* b)
{ return *a == *b; }

size_t
float_hash(float* x)
{ return abs((int) *x); }

int
float_equal(float* a, float* b)
{ return *a == *b; }

#define P
#define T int
#include <stk.h>

#define P
#define T int
#include <pqu.h>

#define P
#define T int
#include <que.h>

#define P
#define T int
#include <lst.h>

#define P
#define T int
#include <deq.h>

#define P
#define T int
#include <set.h>

#define P
#define T char
#include <vec.h>

#define P
#define T int
#include <vec.h>

#define P
#define T unsigned
#include <vec.h>

#define P
#define T float
#include <vec.h>

#define P
#define T double
#include <vec.h>

typedef struct
{
    int x;
    int y;
}
point;

#define P
#define T point
#include <vec.h>

#define T str
#include <vec.h>

typedef struct
{
    vec_point path;
    str name;
}
person;

static person
person_init(size_t path_capacity, const char* first, const char* last)
{
    person self;
    self.path = vec_point_init();
    self.name = str_init(first);
    str_append(&self.name, " ");
    str_append(&self.name, last);
    vec_point_reserve(&self.path, path_capacity);
    return self;
}

static void
person_free(person* self)
{
    vec_point_free(&self->path);
    str_free(&self->name);
}

static person
person_copy(person* self)
{
    person copy = {
        vec_point_copy(&self->path),
        str_copy(&self->name),
    };
    return copy;
}

#define T person
#include <vec.h>

static int
int_match(int* a, int* b)
{
    return *a == *b;
}

static int
int_compare(int* a, int* b)
{
    return *a < *b;
}

int
main(void)
{
    {
        vec_int a = vec_int_init();
        vec_int_push_back(&a, 1);
        vec_int_push_back(&a, 2);
        vec_int_push_back(&a, 3);
        vec_int_push_back(&a, 4);
        vec_int_free(&a);
    }{
        const size_t size = 16;
        deq_int a = deq_int_init();
        for(size_t i = 0; i < size; i++) deq_int_push_back(&a, i);
        for(size_t i = 0; i < size; i++) deq_int_push_front(&a, i);
        deq_int_insert(&a, 1, 99);
        deq_int_sort(&a, int_compare);
        deq_int_free(&a);
    }{
        lst_int a = lst_int_init();
        lst_int_push_back(&a, 1);
        lst_int_push_back(&a, 2);
        lst_int_push_back(&a, 3);
        lst_int_push_back(&a, 4);
        lst_int_push_back(&a, 5);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 7);
        lst_int_push_back(&a, 8);
        lst_int_free(&a);
    }{
        vec_str b = vec_str_init();
        vec_str_push_back(&b, str_init("This"));
        vec_str_push_back(&b, str_init("is"));
        vec_str_push_back(&b, str_init("a"));
        vec_str_push_back(&b, str_init("test"));
        vec_str_resize(&b, 512, str_init(""));
        vec_str_free(&b);
    }{
        vec_person c = vec_person_init();
        vec_person_push_back(&c, person_init(128, "FIRST", "JONES"));
        vec_person_push_back(&c, person_init(256, "LAST", "ALEXA"));
        vec_person_push_back(&c, person_init(512, "NAME", "ANOTHER"));
        vec_person d = vec_person_copy(&c);
        vec_person_free(&c);
        vec_person_free(&d);
    }{
        lst_int a = lst_int_init();
        lst_int_push_back(&a, 1);
        lst_int_push_back(&a, 1);
        lst_int_push_back(&a, 1);
        lst_int_push_back(&a, 2);
        lst_int_push_back(&a, 3);
        lst_int_push_back(&a, 3);
        lst_int_push_back(&a, 4);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 6);
        lst_int_push_back(&a, 8);
        lst_int_push_back(&a, 8);
        lst_int_unique(&a, int_match);
        lst_int_free(&a);
    }{
        ust_int a = ust_int_init(int_hash, int_equal);
        ust_int_insert(&a, -0);
        ust_int_insert(&a, -1);
        ust_int_insert(&a, -2);
        ust_int_insert(&a, -3);
        ust_int_insert(&a, -4);
        ust_int_insert(&a, -5);
        ust_int_insert(&a, -6);
        ust_int_insert(&a, -7);
        ust_int_free(&a);
    }{
        ust_float a = ust_float_init(float_hash, float_equal);
        ust_float_insert(&a, -0);
        ust_float_insert(&a, -1);
        ust_float_insert(&a, -2);
        ust_float_insert(&a, -3);
        ust_float_insert(&a, -4);
        ust_float_insert(&a, -5);
        ust_float_insert(&a, -6);
        ust_float_insert(&a, -7);
        ust_float_free(&a);
    }
    TEST_PASS(__FILE__);
}
