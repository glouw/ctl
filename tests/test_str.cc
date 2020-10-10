#include <stdio.h>
#include <assert.h>
#include <algorithm>

#include <str.h>
#include <string>

#define MAX_LETTERS  (23)
#define MIN_SIZE     (30) // SHORT STRING OPTIMIZATIONS NOT SUPPORTED.
#define MAX_SIZE    (512)
#define MAX_ITERS  (1000)

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
    assert(strcmp(str_c_str(a), c.c_str()) == 0);
    assert(a->capacity == c.capacity());
    assert(a->size == c.size());
    assert(str_empty(a) == c.empty());
}

static int
char_compare(const void* a, const void* b)
{
    char* aa = (char*) a;
    char* bb = (char*) b;
    return *aa > *bb;
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
#define LIST X(APPEND) X(C_STR) X(CLEAR) X(ERASE) X(RESIZE) X(RESERVE) X(SHRINK_TO_FIT) X(SORT) X(COPY) X(SWAP) X(INSERT) X(ASSIGN) X(REPLACE) X(TOTAL)
#define X(name) name,
            enum { LIST };
#undef X
            size_t which = rand() % TOTAL;
#ifdef VERBOSE
#define X(name) #name,
            const char* names[] = { LIST };
#undef X
            printf("-> %s\n", names[which]);
#endif
            switch(which)
            {
                // NEW TO STRING.
                case APPEND:
                {
                    str_append(&a, &b);
                    c.append(d);
                    break;
                }
                case C_STR:
                {
                    assert(strlen(str_c_str(&a)));
                    break;
                }
                // INHERITED FROM VECTOR.
                case CLEAR:
                {
                    str_clear(&a);
                    c.clear();
                    break;
                }
                case ERASE:
                {
                    const size_t index = rand() % a.size;
                    c.erase(c.begin() + index);
                    str_erase(&a, index);
                    break;
                }
                case INSERT:
                {
                    size_t letters = rand() % 512;
                    for(size_t count = 0; count < letters; count++)
                    {
                        const char value = rand() % MAX_LETTERS;
                        const size_t index = rand() % a.size;
                        c.insert(c.begin() + index, value);
                        str_insert(&a, index, value);
                    }
                    break;
                }
                case RESIZE:
                {
                    const size_t resize = (a.size == 0) ? 0 : (rand() % (a.size * 3));
                    c.resize(resize);
                    str_resize(&a, resize);
                    break;
                }
                case RESERVE:
                {
                    const size_t capacity = (a.capacity == 0) ? 0 : (rand() % (a.capacity * 2));
                    c.reserve(capacity);
                    str_reserve(&a, capacity);
                    break;
                }
                case SHRINK_TO_FIT:
                {
                    c.shrink_to_fit();
                    str_shrink_to_fit(&a);
                    break;
                }
                case SORT:
                {
                    str_sort(&a, char_compare);
                    std::sort(c.begin(), c.end());
                    break;
                }
                case COPY:
                {
                    str ca = str_copy(&a);
                    std::string cc = c;
                    test_equal(&ca, cc);
                    str_destruct(&ca);
                    break;
                }
                case ASSIGN:
                {
                    const char value = rand() % MAX_LETTERS;
                    size_t assign_size = rand() % a.size;
                    if(assign_size == 0)
                        assign_size = 1;
                    str_assign(&a, assign_size, value);
                    c.assign(assign_size, value);
                    break;
                }
                case SWAP:
                {
                    str aa = str_copy(&a);
                    str aaa;
                    std::string cc = c;
                    std::string ccc;
                    str_swap(&aaa, &aa);
                    std::swap(cc, ccc);
                    test_equal(&aaa, ccc);
                    str_destruct(&aaa);
                    break;
                }
                case REPLACE:
                {
                    char* replacer = create_test_string(rand() % a.size);
                    str other = str_create(replacer);
                    const size_t index = rand() % a.size;
                    const size_t size = rand() % a.size;
                    str_replace(&a, index, size, &other);
                    c.replace(index, size, replacer);
                    free(replacer);
                    str_destruct(&other);
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
