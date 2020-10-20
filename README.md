# THE C TEMPLATE LIBRARY (CTL)

CTL is a type safe header only template library for ISO C99.

## Motivation

CTL aims to back port useful C++ template containers to C99 to
make maximum use of C99's portability and fast compile times.

## Usage

Define type `CTL_T` before including a CTL container:

```C

#define CTL_POD
#define CTL_T int
#include <vec.h>

int main(void)
{
    vec_int a = vec_int_init();
    vec_int_push_back(&a, 1);
    vec_int_push_back(&a, 2);
    vec_int_push_back(&a, 3);
    vec_int_push_back(&a, 4);
    vec_int_it it = vec_int_it_each(&a);
    CTL_FOR(it, {
        printf("%d\n", *it.ref);
    });
    vec_int_free(&a);
}
```

Compile by including the `ctl` directory as a system directory:

    gcc main.c -isystem ctl

Containers and types are hot swappable:

```C

typedef struct
{
    double x;
    double y;
}
point;

#define CTL_POD
#define CTL_T point
#include <lst.h>

int main(void)
{
    vec_point a = vec_point_init();
    vec_point_push_back(&a, (point) { 1.1, 2.2 });
    vec_point_push_back(&a, (point) { 3.3, 4.4 });
    vec_point_push_back(&a, (point) { 5.5, 6.6 });
    vec_point_push_back(&a, (point) { 7.7, 8.8 });
    vec_point_it it = vec_point_it_each(&a);
    CTL_FOR(it, {
        printf("%f %f\n", it.ref->x, it.ref->y);
    });
    vec_point_free(&a);
}
```
Plain Old Data (POD) types do not require require definitions for a default constructor,
a copy constructor, and a destructor. Types that acquire resources with malloc do, and
the aforementioned constructors and destructor must take the form of `CTL_T + init_default`,
`CTL_T + copy`, and `CTL_T + free`, respectively:

```C

#include <str.h>

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

int main(void)
{
    vec_person a = vec_person_init();
    vec_person_push_back(&a, person_init(128, "Midnight", "Walker"));
    vec_person_push_back(&a, person_init(256, "Moonlit", "Sunrise"));
    vec_person_push_back(&a, person_init(512, "Daytime", "Eclipse"));
    vec_person b = vec_person_copy(&a);
    vec_person_free(&a);
    vec_person_free(&b);
}
```

## Caveats

Two containers with the same type may not be instantiated. For instance, the following
will result in multiple definition compile time errors:

```C
#define CTL_POD
#define CTL_T char
#include <vec.h>

#define CTL_POD
#define CTL_T char
#include <vec.h>
```
If the above scenario is encountered and needs a work around, a template expansion can be renamed:

```C
#define CTL_POD
#define CTL_T char
#include <vec.h>

#define vec_char my_char_vec
#define CTL_POD
#define CTL_T char
#include <vec.h>
#undef my_char_vec
```
