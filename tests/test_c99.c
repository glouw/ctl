#include <stdio.h>
#include <assert.h>

#define CTL_T char
#include <vec.h>

#include <str.h>
#include <str.h>

#define CTL_T int
#include <vec.h>

#define CTL_T float
#include <vec.h>

#define CTL_T double
#include <vec.h>

typedef struct
{
    int age;
    str name;
    double weight;
    float height;
}
person;

#define CTL_T person
#include <vec.h>

#define CTL_T str
#include <vec.h>

int main(void)
{
    printf("%s: PASSED\n", __FILE__);
}
