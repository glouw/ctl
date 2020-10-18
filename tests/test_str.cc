#include "test.h"

#include "digi.cc"

#include <str.h>

#include <string>
#include <algorithm>

static char*
create_test_string(size_t size)
{
    char* temp = (char*) malloc(TEST_MAX_SIZE + 1);
    for(size_t i = 0; i < size; i++)
    {
        temp[i] = 'a' + rand() % TEST_ALPHA_LETTERS;
        if(rand() % 2)
            temp[i] = toupper(temp[i]);
    }
    temp[size] = '\0';
    return temp;
}

static void
test_equal(str* a, std::string& b)
{
    assert(strcmp(str_c_str(a), b.c_str()) == 0);
    assert(a->capacity == b.capacity());
    assert(a->size == b.size());
    assert(str_empty(a) == b.empty());
}

static int
char_compare(char* a, char* b)
{
    return *a > *b;
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    const size_t iters = rand() % TEST_MAX_ITERS;
    for(size_t i = 0; i < iters; i++)
    {
        for(size_t j = 0; j < 2; j++)
        {
            size_t str_size = rand() % TEST_MAX_SIZE;
            if(str_size < TEST_MIN_STR_SIZE)
                str_size = TEST_MIN_STR_SIZE;
            char* base = create_test_string(str_size);
            str a;
            std::string b;
            // INIT DIRECTLY.
            if(j == 0)
            {
                a = str_create(base);
                b = base;
            }
            // INIT WITH GROWTH.
            if(j == 1)
            {
                a = str_create("");
                for(size_t k = 0; k < str_size; k++)
                {
                    str_push_back(&a, base[k]);
                    b.push_back(base[k]);
                }
            }
#define LIST X(APPEND)            \
             X(C_STR)             \
             X(CLEAR)             \
             X(ERASE)             \
             X(RESIZE)            \
             X(RESERVE)           \
             X(SHRINK_TO_FIT)     \
             X(SORT)              \
             X(COPY)              \
             X(SWAP)              \
             X(INSERT)            \
             X(ASSIGN)            \
             X(REPLACE)           \
             X(FIND)              \
             X(RFIND)             \
             X(FIND_FIRST_OF)     \
             X(FIND_LAST_OF)      \
             X(FIND_FIRST_NOT_OF) \
             X(FIND_LAST_NOT_OF)  \
             X(SUBSTR)            \
             X(COMPARE)           \
             X(TOTAL)
#define X(name) name,
            enum { LIST };
#undef X
            size_t which = rand() % TOTAL;
#ifdef VERBOSE
#define X(name) #name,
            const char* names[] = { LIST };
#undef X
            printf("-> %lu : %s\n", j, names[which]);
#endif
            switch(which)
            {
                // NEW TO STRING.
                case APPEND:
                {
                    char* temp = create_test_string(rand() % 256);
                    str_append(&a, temp);
                    b.append(temp);
                    free(temp);
                    break;
                }
                case C_STR:
                {
                    assert(strlen(str_c_str(&a)));
                    assert(str_c_str(&a) == str_data(&a));
                    break;
                }
                case REPLACE:
                {
                    char* temp = create_test_string(rand() % a.size);
                    const size_t index = rand() % a.size;
                    const size_t size = rand() % a.size;
                    str_replace(&a, index, size, temp);
                    b.replace(index, size, temp);
                    free(temp);
                    break;
                }
                case FIND:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find(&a, temp) == b.find(temp));
                    free(temp);
                    break;
                }
                case RFIND:
                {
                    char* temp = create_test_string(rand() % 3);
                    assert(str_rfind(&a, temp) == b.rfind(temp));
                    free(temp);
                    break;
                }
                case FIND_FIRST_OF:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find_first_of(&a, temp) == b.find_first_of(temp));
                    free(temp);
                    break;
                }
                case FIND_LAST_OF:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find_last_of(&a, temp) == b.find_last_of(temp));
                    free(temp);
                    break;
                }
                case FIND_FIRST_NOT_OF:
                {
                    const size_t size = rand() % 192;
                    char* temp = create_test_string(size);
                    assert(str_find_first_not_of(&a, temp) == b.find_first_not_of(temp));
                    free(temp);
                    break;
                }
                case FIND_LAST_NOT_OF:
                {
                    const size_t size = rand() % 192;
                    char* temp = create_test_string(size);
                    assert(str_find_last_not_of(&a, temp) == b.find_last_not_of(temp));
                    free(temp);
                    break;
                }
                case SUBSTR:
                {
                    const size_t index = rand() % a.size;
                    const size_t size = rand() % (a.size - index);
                    if(size > TEST_MIN_STR_SIZE)
                    {
                        str substr1 = str_substr(&a, index, size);
                        std::string substr2 = b.substr(index, size);
                        test_equal(&substr1, substr2);
                        str_free(&substr1);
                    }
                    break;
                }
                case COMPARE:
                {
                    size_t size = rand() % 512;
                    char* _ta = create_test_string(size);
                    char* _tb = create_test_string(size);
                    str _a = str_create(_ta);
                    str _b = str_create(_tb);
                    std::string _aa = _ta;
                    std::string _bb = _tb;
                    assert(TEST_SIGN(str_compare(&_a, _tb)) == TEST_SIGN(_aa.compare(_tb)));
                    assert(TEST_SIGN(str_compare(&_b, _ta)) == TEST_SIGN(_bb.compare(_ta)));
                    assert(TEST_SIGN(str_compare(&_a, _ta)) == TEST_SIGN(_aa.compare(_ta)));
                    assert(TEST_SIGN(str_compare(&_b, _tb)) == TEST_SIGN(_bb.compare(_tb)));
                    str_free(&_a);
                    str_free(&_b);
                    free(_ta);
                    free(_tb);
                    break;
                }
                // INHERITED FROM VECTOR.
                case CLEAR:
                {
                    str_clear(&a);
                    b.clear();
                    break;
                }
                case ERASE:
                {
                    const size_t index = rand() % a.size;
                    b.erase(b.begin() + index);
                    str_erase(&a, index);
                    break;
                }
                case INSERT:
                {
                    size_t letters = rand() % 512;
                    for(size_t count = 0; count < letters; count++)
                    {
                        const char value = rand() % TEST_ALPHA_LETTERS;
                        const size_t index = rand() % a.size;
                        b.insert(b.begin() + index, value);
                        str_insert(&a, index, value);
                    }
                    break;
                }
                case RESIZE:
                {
                    const size_t resize = (a.size == 0) ? 0 : (rand() % (a.size * 3));
                    b.resize(resize);
                    str_resize(&a, resize);
                    break;
                }
                case RESERVE:
                {
                    const size_t capacity = (a.capacity == 0) ? 0 : (rand() % (a.capacity * 2));
                    b.reserve(capacity);
                    str_reserve(&a, capacity);
                    break;
                }
                case SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    str_shrink_to_fit(&a);
                    break;
                }
                case SORT:
                {
                    str_sort(&a, char_compare);
                    std::sort(b.begin(), b.end());
                    break;
                }
                case COPY:
                {
                    str ca = str_copy(&a);
                    std::string cb = b;
                    test_equal(&ca, cb);
                    str_free(&ca);
                    break;
                }
                case ASSIGN:
                {
                    const char value = rand() % TEST_ALPHA_LETTERS;
                    size_t assign_size = rand() % a.size;
                    if(assign_size == 0)
                        assign_size = 1;
                    str_assign(&a, assign_size, value);
                    b.assign(assign_size, value);
                    break;
                }
                case SWAP:
                {
                    str aa = str_copy(&a);
                    str aaa;
                    std::string cb = b;
                    std::string bbb;
                    str_swap(&aaa, &aa);
                    std::swap(cb, bbb);
                    test_equal(&aaa, bbb);
                    str_free(&aaa);
                    break;
                }
            }
            test_equal(&a, b);
            str_free(&a);
            free(base);
        }
    }
    TEST_PASS(__FILE__);
}
