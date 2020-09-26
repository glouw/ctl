#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/list.h>

static void
test_list_int_compile(void)
{
}

int
main(void)
{
    test_list_int_compile();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
