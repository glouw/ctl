#include <stdio.h>
#include <assert.h>

#define T int
#define S 42
#include <ctl/array.h>

void
test_array_int_populate(void)
{
    array_int_42 ints = array_int_42_init(NULL, NULL, NULL);
    int sum= 0;
    int i;
    for(i = 0; i < ints.size; i++)
        ints.value[i] = 1;
    for(i = 0; i < ints.size; i++)
        sum += ints.value[i];
    assert(sum == ints.size);
}

void
test_array_int_copy(void)
{
    int i;
    array_int_42 copy, ints = array_int_42_init(NULL, NULL, NULL);
    for(i = 0; i < ints.size; i++)
        ints.value[i] = i;
    copy = array_int_42_copy(&ints);
    for(i = 0; i < copy.size; i++)
        assert(copy.value[i] == ints.value[i]);
}

typedef struct
{
    int height;
    int* path;
}
person;

static person
person_init(int height, int path)
{
    static person zero;
    person self = zero; 
    self.height = height;
    self.path = malloc(sizeof(*self.path));
    *self.path = path;
    return self;
}

static void
person_destruct(person* self)
{
    free(self->path);
}

static person
person_copy(person* self)
{
    int path = self->path ? *self->path : 0;
    return person_init(self->height, path);
}

static person
person_move(person* self)
{
    static person zero;
    person other = *self;
    *self = zero;
    return other;
}

#define T person
#define S 8
#include <ctl/array.h>

void
test_person_copy(void)
{
    array_person_8 copy;
    array_person_8 moved;
    array_person_8 persons = array_person_8_init(person_destruct, person_copy, person_move);
    int i;
    int height = 180;
    int path = 100;
    for(i = 0; i < persons.size; i++)
        persons.value[i] = person_init(i * height, i * path);
    copy = array_person_8_copy(&persons);
    for(i = 0; i < persons.size; i++)
    {
        person* a = &persons.value[i];
        person* b = &copy.value[i];
        assert(a->height == b->height);
        assert(a->path != b->path); /* DIFFERENT MEMORY LOCATIONS */
        assert(*a->path == *b->path);
    }
    moved = array_person_8_move(&persons);
    /* NO DOUBLE FREE BECAUSE MOVE */
    array_person_8_destruct(&persons);
    array_person_8_destruct(&moved);
    /* NO DOUBLE FREE BECAUSE COPY */
    array_person_8_destruct(&copy);
}

int
main(void)
{
    test_array_int_populate();
    test_array_int_copy();
    test_person_copy();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
