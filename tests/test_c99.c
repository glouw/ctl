#include "test.h"

#include <str.h>
#include <str.h>
#include <str.h>
#include <str.h>

#define CTL_T char
#include <vec.h>

#define CTL_T int
#include <lst.h>

#define CTL_T int
#include <vec.h>

#define CTL_T unsigned
#include <vec.h>

#define CTL_T float
#include <vec.h>

#define CTL_T double
#include <vec.h>

#define CTL_T str
#include <vec.h>

typedef struct
{
    int x;
    int y;
}
point;

#define point_init_default NULL
#define point_copy NULL
#define point_free NULL
#define CTL_T point
#include <vec.h>

typedef struct
{
    vec_point path;
    str name;
}
person;

person
person_init(size_t path_capacity, const char* first, const char* last)
{
    person self;
    self.path = vec_point_init();
    self.name = str_create(first);
    str_append(&self.name, " ");
    str_append(&self.name, last);
    vec_point_reserve(&self.path, path_capacity);
    return self;
}

person
person_init_default(void)
{
    return person_init(0, "NONE", "NONE");
}

void
person_free(person* self)
{
    vec_point_free(&self->path);
    str_free(&self->name);
}

person
person_copy(person* self)
{
    person copy;
    copy.path = vec_point_copy(&self->path);
    copy.name = str_copy(&self->name);
    return copy;
}

#define CTL_T person
#include <vec.h>

#include <stdio.h>

static bool
int_match(int* a, int* b)
{
    return *a == *b;
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
        vec_str b = vec_str_init();
        vec_str_push_back(&b, str_create("This"));
        vec_str_push_back(&b, str_create("is"));
        vec_str_push_back(&b, str_create("a"));
        vec_str_push_back(&b, str_create("test"));
        vec_str_resize(&b, 512);
        vec_str_free(&b);
    }{
        vec_person c = vec_person_init();
        vec_person_push_back(&c, person_init(128, "GUSTAV", "LOUW"));
        vec_person_push_back(&c, person_init(256, "SUSAN", "YU"));
        vec_person_push_back(&c, person_init(512, "JACO", "LOUW"));
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
    }
    TEST_PASS(__FILE__);
}
