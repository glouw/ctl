#include <stdio.h>
#include <assert.h>

#include <str.h>

#define T int
#include <vec.h>

#define T float
#include <vec.h>

#define T double
#include <vec.h>

typedef struct
{
    int age;
    str name;
    double weight;
    float height;
}
person;

#define T person
#include <vec.h>

int main(void)
{
    printf("%s: PASSED\n", __FILE__);
}
