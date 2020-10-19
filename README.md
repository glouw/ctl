# THE C TEMPLATE LIBRARY (CTL)

CTL is a type safe header only template library for ISO C99.

## Motivation

The primary motivation of the CTL is to back port useful C++ template containers
to C99 to make maximum use of C99's portability and fast compile times.

The secondary motivation of the CTL is to provide embedded systems without an ISO compatible
implementation of the C++ Standard Template Library (STL) with an ISO conforming
C99 implementation.

## Usage

Define type `CTL_T` before including a CTL container:

```C
#define CTL_T int
#include <vec.h>

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

```

To instantiate and iterate over a vector of integers:

```C
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

To compile, include the `ctl` directory as a system directory:

    gcc main.c -isystem ctl

To swap the container type, replace all instances of `int` with a new type, eg. `double`:

```C
int main(void)
{
    vec_double a = vec_double_init();
    vec_double_push_back(&a, 1.1);
    vec_double_push_back(&a, 2.2);
    vec_double_push_back(&a, 3.3);
    vec_double_push_back(&a, 4.4);
    vec_double_it it = vec_double_it_each(&a);
    CTL_FOR(it, {
        printf("%f\n", *it.ref);
    });
    vec_double_free(&a);
}
```

To swap the container, replace all instances of `vec` with the new container name (eg. `lst`),
provided the templated container was previously included with the correct type:

```C
#define CTL_T double
#include <lst.h>

int main(void)
{
    lst_double a = lst_double_init();
    lst_double_push_back(&a, 1.1);
    lst_double_push_back(&a, 2.2);
    lst_double_push_back(&a, 3.3);
    lst_double_push_back(&a, 4.4);
    lst_double_it it = lst_double_it_each(&a);
    CTL_FOR(it, {
        printf("%f\n", *it.ref);
    });
    lst_double_free(&a);
}
```

Custom types declared with `typedef` require definitions for a default constructor,
a copy constructor, and destructor.

Simple types that do not require internal memory management can nullify these
definitions:

```C
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
```

More complex types requiring internal memory management (memory acquired
by `malloc` through CTL or other invocations thereof) require function definitions in
the form of `CTL_T function(void)` for the default constructor,
`CTL_T function(CTL_T*)` for the copy constructor, and `void function(CTL_T*)` for the destructor:

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
```

The default constructor, for instance, is used internally by CTL when a container is resized.

To use the container, initialize it, and manually destruct the container when desired:


```C
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
will result in multiple defintion compile time errors:

```C
#define CTL_T char
#include <vec.h>

#define CTL_T char
#include <vec.h>
```

Proper design would probably dictate that all templated instatiations be done in a single
`templates.h` header file to keep track of template interactions, but for whatever reason
if the above scenario is encountered and needs a work around, a template expansion can be renamed:

```C
#define CTL_T char
#include <vec.h>

#define vec_char str
#define CTL_T char
#include <vec.h>
#undef vec_char
```
