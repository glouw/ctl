#include <stdio.h>
#include <assert.h>

#include <str.h>

int main(void)
{
    str first = str_create("Gustav");
    str space = str_create(" ");
    str last = str_create("Louw");
    str name = str_create("");
    str_append(&name, first.value);
    str_append(&name, space.value);
    str_append(&name, last.value);
    str_append(&name, "...");
    assert(strcmp(name.value, "Gustav Louw...") == 0);
    str_replace(&name, 0, 6, "Test");
    assert(strcmp(name.value, "Test Louw...") == 0);
    puts(name.value);
    str_destruct(&name);
    str_destruct(&first);
    str_destruct(&space);
    str_destruct(&last);
    printf("%s: PASSED\n", __FILE__);
}
