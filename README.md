# C TEMPLATE LIBRARY (CTL)

CTL is a type safe header only template library for ISO C99.

## Motivation

CTL aims to back port useful C++ template containers to C99 to
make maximum use of C99's fast compile time.

## Usage

Define type `T` before including a CTL container:

```C
#include <stdio.h>

#define P
#define T int
#include <vec.h>

int compare(int* a, int* b) { return *b < *a; }

int main(void)
{
    vec_int a = vec_int_init();
    vec_int_push_back(&a, 9);
    vec_int_push_back(&a, 1);
    vec_int_push_back(&a, 3);
    vec_int_sort(&a, compare);
    foreach(vec_int, &a, it,
        printf("%d\n", *it.ref);
    );
    vec_int_free(&a);
}
```

The definition `P` implies the type is Plain Old Data (POD) and no
copy constructor, default constructor, or destructor is needed.
Both `P` and `T` definitions are consumed (`#undef`) by a
CTL container include directive.

To compile, include the `ctl` directory as a system directory:

    gcc main.c -isystem ctl

Containers and types are hot swappable:

```C
#include <stdio.h>
#include <math.h>

typedef struct
{
    double x;
    double y;
}
point;

#define P
#define T point
#include <lst.h>

double mag(point* p) { return sqrt(p->x * p->x + p->y * p->y); }

int compare(point* a, point* b) { return mag(b) < mag(a); }

int main(void)
{
    lst_point a = lst_point_init();
    lst_point_push_back(&a, (point) { 5.5, 6.6 });
    lst_point_push_back(&a, (point) { 1.1, 2.2 });
    lst_point_push_back(&a, (point) { 7.7, 8.8 });
    lst_point_sort(&a, compare);
    foreach(lst_point, &a, it,
        printf("%f %f\n", it.ref->x, it.ref->y);
    );
    lst_point_free(&a);
}
```
Types that acquire resources with `malloc` require that the `P` definition be omitted,
and require function definitions for the type's default constructor, copy constructor,
and destructor in the form of `T + init_default`, `T + copy`, and `T + free`,
respectively:

```C

#include <stdio.h>
#include <str.h>

typedef struct
{
    double x;
    double y;
}
point;

#define P
#define T point
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
    person self = { vec_point_init(), str_create(first) };
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
    return (Person) { vec_point_copy(&self->path), str_copy(&self->name) };
}

#define T person
#include <vec.h>

int main(void)
{
    vec_person a = vec_person_init();
    vec_person_push_back(&a, person_init(128, "Midnight", "Walker"));
    vec_person_push_back(&a, person_init(256, "Moonlit", "Sunrise"));
    vec_person_push_back(&a, person_init(512, "Daytime", "Eclipse"));
    vec_person_resize(&a, 8); // Default constructor called for indices 3, 4, 5, 6, 7.
    foreach(vec_person, &a, it,
        printf("%lu %lu %s\n", it.ref->path.size, it.ref->path.capacity, it.ref->name.value);
    );
    vec_person b = vec_person_copy(&a); // Copy constructor called for each index.
    vec_person_free(&a); // Default destructor called for each index.
    vec_person_free(&b); // Likewise, but on the copy.
}
```

### Using Pointers

If pointers wish to be stored within a container, type define a pointer type
before including the template container header:

```C
typedef int* intp;
#define P
#define T intp
#include <vec.h>
```

### Templating Containers with Containers

Containers can be templated from other containers. For instance, a list of
vectors holding integers:

```C
#define P
#define T int
#include <vec.h>

#define T vec_int
#include <lst.h>
```

The constructors, destructors, and copy constructors for `vec_int` are consumed by `lst_vec_int`,
so no further definitions are required.

### Resolving Namespace Collisions

Two containers with the same type may not be instantiated.
For instance, the following examples result in multiple definition
compile time errors:

```C
#define P
#define T char
#include <vec.h>

#define P
#define T char
#include <vec.h>
```
If the above scenario is encountered and needs a work around,
a template expansion can be renamed:

```C
#define P
#define T char
#include <vec.h>

#define vec_char my_char_vec
#define P
#define T char
#include <vec.h>
#undef vec_char
```

## Running Tests

Each test randomly fills both CTL and STL containers with random data and then
proceeds to randomly call functions. A test passes when the maximum number of
iterations within `test.h` is reached, and the container meta data, as well actual data,
within both CTL and STL containers, match for each iteration.

To compile and run all tests, run:

    make

To compile and run all tests with greater memory usage and more max iterations,
pass the `LONG=1` flag to make:

    make LONG=1

To run overnight and catch subtle regressions:

    while true; do make LONG=1 || break; done

## Keywords

CTL reserves the following:

    iterate, foreach, len, CAT, PASTE, TEMP, IMPL, A, B, I, T, P
