#include <stdio.h>
#include <assert.h>

#include <str.h>
#include <string>

#define MAX_LETTERS    (23)
#define MAX_ITERS  (100000)
#define MAX_SIZE     (4096)
#define MIN_SIZE       (30) // SHORT STRING OPTIMIZATIONS NOT SUPPORTED.

static char*
create_test_string(size_t size)
{
    char* temp = (char*) malloc(MAX_SIZE + 1);
    for(size_t i = 0; i < size; i++)
    {
        temp[i] = 'a' + rand() % MAX_LETTERS;
        if(rand() % 2)
            temp[i] = toupper(temp[i]);
    }
    temp[size] = '\0';
    return temp ;
}

static void
test_equal(str* a, std::string& c)
{
    char* c_str = str_c_str(a);
    assert(strcmp(c_str, c.c_str()) == 0);
    free(c_str);
    assert(a->capacity == c.capacity());
    assert(a->size == c.size());
    assert(str_empty(a) == c.empty());
}

int main(void)
{
    srand(time(NULL));
    const size_t iters = rand() % MAX_ITERS;
    for(size_t i = 0; i < iters; i++)
    {
        for(size_t j = 0; j < 2; j++)
        {
            size_t size_a = rand() % MAX_SIZE;
            size_t size_b = rand() % MAX_SIZE;
            if(size_a < MIN_SIZE) size_a = MIN_SIZE;
            if(size_b < MIN_SIZE) size_b = MIN_SIZE;
            char* tsa = create_test_string(size_a);
            char* tsb = create_test_string(size_b);
            str a;
            str b;
            std::string c;
            std::string d;
            // INIT DIRECTLY.
            if(j == 0)
            {
                a = str_create(tsa);
                b = str_create(tsb);
                c = tsa;
                d = tsb;
            }
            // INIT WITH GROWTH.
            if(j == 1)
            {
                a = str_create("");
                b = str_create("");
                for(size_t k = 0; k < size_a; k++)
                {
                    str_push_back(&a, tsa[k]);
                    c.push_back(tsa[k]);
                }
                for(size_t k = 0; k < size_b; k++)
                {
                    str_push_back(&b, tsb[k]);
                    d.push_back(tsb[k]);
                }
            }
            test_equal(&a, c);
#define LIST X(APPEND) X(C_STR) X(TOTAL)
#define X(name) name,
            enum { LIST };
#undef X
            size_t which = rand() % TOTAL;
#ifdef VERBOSE
#define X(name) #name,
            const char* names[] = { LIST };
#undef X
            puts(names[which]);
#endif
            switch(which)
            {
                case APPEND:
                {
                    str_append(&a, &b);
                    c.append(d);
                    break;
                }
                case C_STR:
                {
                    free(str_c_str(&a));
                    break;
                }
            }
            test_equal(&a, c);
            str_destruct(&a);
            str_destruct(&b);
            free(tsa);
            free(tsb);
        }
    }
    printf("%s: PASSED\n", __FILE__);
}
