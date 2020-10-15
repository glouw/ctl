#include <str.h>

#define CTL_T char
#include <vec.h>

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

int main(void)
{
    vec_int a = vec_int_init();
    vec_int_push_back(&a, 1);
    vec_int_push_back(&a, 2);
    vec_int_push_back(&a, 3);
    vec_int_push_back(&a, 4);
#ifdef VERBOSE
    vec_int_it it0 = vec_int_it_each(&a);
    CTL_FOR(it0, {
        printf("%d\n", *it0.value);
    })
#endif
    vec_int_free(&a);
    vec_str b = vec_str_init();
    vec_str_push_back(&b, str_create("This"));
    vec_str_push_back(&b, str_create("is"));
    vec_str_push_back(&b, str_create("a"));
    vec_str_push_back(&b, str_create("test"));
    vec_str_resize(&b, 512);
#ifdef VERBOSE
    vec_str_it it1 = vec_str_it_each(&b);
    CTL_FOR(it1, {
        str* s = it1.value;
        if(s->size > 0)
            printf("%s\n", str_c_str(s));
    })
#endif
    vec_str_free(&b);
    vec_person c = vec_person_init();
    vec_person_push_back(&c, person_init(128, "GUSTAV", "LOUW"));
    vec_person_push_back(&c, person_init(256, "SUSAN", "YU"));
    vec_person_push_back(&c, person_init(512, "JACO", "LOUW"));
#ifdef VERBOSE
    vec_person_it it2 = vec_person_it_each(&c);
    CTL_FOR(it2, {
        person* p = it2.value;
        puts(p->name.value);
    })
#endif
    vec_person d = vec_person_copy(&c);
    vec_person_free(&c);
    vec_person_free(&d);
    printf("%s: PASSED\n", __FILE__);
}
