#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/delist.h>

void
test_delist_creation(void)
{
    delist_node_int* node;
    delist_int ints = delist_int_init(NULL);
    delist_int_push_back(&ints, 0);
    delist_int_push_back(&ints, 1);
    delist_int_push_back(&ints, 2);
    delist_int_push_back(&ints, 3);
    delist_int_push_back(&ints, 4);
    delist_int_push_front(&ints, -1);
    delist_int_push_front(&ints, -2);
    delist_int_push_front(&ints, -3);
    delist_int_push_front(&ints, -4);
    for(node = ints.head; node->next; node = node->next)
        assert(node->next->value > node->value);
    delist_int_free(&ints);
    assert(ints.head == NULL);
    assert(ints.tail == NULL);
    assert(ints.size == 0);
    delist_int_push_front(&ints, 99);
    assert(ints.size != 0);
    assert(ints.head != NULL);
    assert(ints.tail != NULL);
    assert(ints.head == ints.tail);
    delist_int_free(&ints);
}

int
main(void)
{
    test_delist_creation();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
