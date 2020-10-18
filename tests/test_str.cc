#include "test.h"

#include "digi.cc"

#include <str.h>

#include <string>
#include <algorithm>

#define TEST_MIN_STR_SIZE  (30)
#define TEST_ALPHA_LETTERS (23)

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
    const size_t loops = rand() % TEST_MAX_LOOPS;
    for(size_t loop = 0; loop < loops; loop++)
    {
        enum
        {
            MODE_DIRECT,
            MODE_GROWTH,
            MODE_TOTAL
        };
        for(size_t mode = MODE_DIRECT; mode < MODE_TOTAL; mode++)
        {
            size_t str_size = rand() % TEST_MAX_SIZE;
            if(str_size < TEST_MIN_STR_SIZE)
                str_size = TEST_MIN_STR_SIZE;
            char* base = create_test_string(str_size);
            str a;
            std::string b;
            if(mode == MODE_DIRECT)
            {
                a = str_create(base);
                b = base;
            }
            if(mode == MODE_GROWTH)
            {
                a = str_create("");
                for(size_t k = 0; k < str_size; k++)
                {
                    str_push_back(&a, base[k]);
                    b.push_back(base[k]);
                }
            }
            enum
            {
                TEST_APPEND,
                TEST_C_STR,
                TEST_CLEAR,
                TEST_ERASE,
                TEST_RESIZE,
                TEST_RESERVE,
                TEST_SHRINK_TO_FIT,
                TEST_SORT,
                TEST_COPY,
                TEST_SWAP,
                TEST_INSERT,
                TEST_ASSIGN,
                TEST_REPLACE,
                TEST_FIND,
                TEST_RFIND,
                TEST_FIND_FIRST_OF,
                TEST_FIND_LAST_OF,
                TEST_FIND_FIRST_NOT_OF,
                TEST_FIND_LAST_NOT_OF,
                TEST_SUBSTR,
                TEST_COMPARE,
                TEST_TOTAL
            };
            switch(rand() % TEST_TOTAL)
            {
                case TEST_APPEND:
                {
                    char* temp = create_test_string(rand() % 256);
                    str_append(&a, temp);
                    b.append(temp);
                    free(temp);
                    break;
                }
                case TEST_C_STR:
                {
                    assert(strlen(str_c_str(&a)));
                    assert(str_c_str(&a) == str_data(&a));
                    break;
                }
                case TEST_REPLACE:
                {
                    char* temp = create_test_string(rand() % a.size);
                    const size_t index = rand() % a.size;
                    const size_t size = rand() % a.size;
                    str_replace(&a, index, size, temp);
                    b.replace(index, size, temp);
                    free(temp);
                    break;
                }
                case TEST_FIND:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find(&a, temp) == b.find(temp));
                    free(temp);
                    break;
                }
                case TEST_RFIND:
                {
                    char* temp = create_test_string(rand() % 3);
                    assert(str_rfind(&a, temp) == b.rfind(temp));
                    free(temp);
                    break;
                }
                case TEST_FIND_FIRST_OF:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find_first_of(&a, temp) == b.find_first_of(temp));
                    free(temp);
                    break;
                }
                case TEST_FIND_LAST_OF:
                {
                    const size_t size = rand() % 3;
                    char* temp = create_test_string(size);
                    assert(str_find_last_of(&a, temp) == b.find_last_of(temp));
                    free(temp);
                    break;
                }
                case TEST_FIND_FIRST_NOT_OF:
                {
                    const size_t size = rand() % 192;
                    char* temp = create_test_string(size);
                    assert(str_find_first_not_of(&a, temp) == b.find_first_not_of(temp));
                    free(temp);
                    break;
                }
                case TEST_FIND_LAST_NOT_OF:
                {
                    const size_t size = rand() % 192;
                    char* temp = create_test_string(size);
                    assert(str_find_last_not_of(&a, temp) == b.find_last_not_of(temp));
                    free(temp);
                    break;
                }
                case TEST_SUBSTR:
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
                case TEST_COMPARE:
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
                case TEST_CLEAR:
                {
                    str_clear(&a);
                    b.clear();
                    break;
                }
                case TEST_ERASE:
                {
                    const size_t index = rand() % a.size;
                    b.erase(b.begin() + index);
                    str_erase(&a, index);
                    break;
                }
                case TEST_INSERT:
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
                case TEST_RESIZE:
                {
                    const size_t resize = (a.size == 0) ? 0 : (rand() % (a.size * 3));
                    b.resize(resize);
                    str_resize(&a, resize);
                    break;
                }
                case TEST_RESERVE:
                {
                    const size_t capacity = (a.capacity == 0) ? 0 : (rand() % (a.capacity * 2));
                    b.reserve(capacity);
                    str_reserve(&a, capacity);
                    break;
                }
                case TEST_SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    str_shrink_to_fit(&a);
                    break;
                }
                case TEST_SORT:
                {
                    str_sort(&a, char_compare);
                    std::sort(b.begin(), b.end());
                    break;
                }
                case TEST_COPY:
                {
                    str ca = str_copy(&a);
                    std::string cb = b;
                    test_equal(&ca, cb);
                    str_free(&ca);
                    break;
                }
                case TEST_ASSIGN:
                {
                    const char value = rand() % TEST_ALPHA_LETTERS;
                    size_t assign_size = rand() % a.size;
                    if(assign_size == 0)
                        assign_size = 1;
                    str_assign(&a, assign_size, value);
                    b.assign(assign_size, value);
                    break;
                }
                case TEST_SWAP:
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
