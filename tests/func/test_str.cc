#include "../test.h"

#include <str.h>
#include "digi.hh"

#include <string>
#include <algorithm>

#define MIN_STR_SIZE  (30) // NO SUPPORT FOR SMALL STRINGS.
#define ALPHA_LETTERS (23)

#define CHECK(_x, _y) {                              \
    assert(strcmp(str_c_str(&_x), _y.c_str()) == 0); \
    assert(_x.capacity == _y.capacity());            \
    assert(_x.size == _y.size());                    \
    assert(str_empty(&_x) == _y.empty());            \
    if(_x.size > 0) {                                \
        assert(_y.front() == *str_front(&_x));       \
        assert(_y.back() == *str_back(&_x));         \
    }                                                \
    std::string::iterator _iter = _y.begin();        \
    foreach(str, &_x, _it) {                         \
        assert(*_it.ref == *_iter);                  \
        _iter++;                                     \
    }                                                \
    str_it _it = str_it_each(&_x);                   \
    for(auto& _d : _y) {                             \
        assert(*_it.ref == _d);                      \
        _it.step(&_it);                              \
    }                                                \
    for(size_t i = 0; i < _y.size(); i++)            \
        assert(_y.at(i) == *str_at(&_x, i));         \
}

static char*
create_test_string(size_t size)
{
    char* temp = (char*) malloc(size + 1);
    for(size_t i = 0; i < size; i++)
    {
        temp[i] = 'a' + TEST_RAND(ALPHA_LETTERS);
        if(TEST_RAND(2))
            temp[i] = toupper(temp[i]);
    }
    temp[size] = '\0';
    return temp;
}

static int
char_compare(char* a, char* b)
{
    return *a > *b;
}

static void
test_zero_size_copy(void)
{
    str a = str_init("");
    str b = str_copy(&a);
    std::string aa = "";
    std::string bb = aa;
    assert(aa.size() == a.size);
    assert(bb.size() == b.size);
    assert(aa.capacity() == a.capacity);
    assert(bb.capacity() == b.capacity);
    str_free(&a);
    str_free(&b);
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    test_zero_size_copy();
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t loop = 0; loop < loops; loop++)
    {
        size_t str_size = TEST_RAND(TEST_MAX_SIZE);
        if(str_size < MIN_STR_SIZE)
            str_size = MIN_STR_SIZE;
        enum
        {
            MODE_DIRECT,
            MODE_GROWTH,
            MODE_TOTAL
        };
        for(size_t mode = MODE_DIRECT; mode < MODE_TOTAL; mode++)
        {
            char* base = create_test_string(str_size);
            str a = str_init("");
            std::string b;
            if(mode == MODE_DIRECT)
            {
                str_free(&a);
                a = str_init(base);
                b = base;
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t i = 0; i < str_size; i++)
                {
                    str_push_back(&a, base[i]);
                    b.push_back(base[i]);
                }
            }
            enum
            {
                TEST_PUSH_BACK,
                TEST_POP_BACK,
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
                TEST_COUNT,
                TEST_TOTAL
            };
            int which = TEST_RAND(TEST_TOTAL);
            switch(which)
            {
                case TEST_PUSH_BACK:
                {
                    const char value = TEST_RAND(ALPHA_LETTERS);
                    b.push_back(value);
                    str_push_back(&a, value);
                    CHECK(a, b);
                    break;
                }
                case TEST_POP_BACK:
                {
                    if(a.size > 0)
                    {
                        b.pop_back();
                        str_pop_back(&a);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_APPEND:
                {
                    char* temp = create_test_string(TEST_RAND(256));
                    str_append(&a, temp);
                    b.append(temp);
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_C_STR:
                {
                    assert(strlen(str_c_str(&a)));
                    assert(str_c_str(&a) == str_data(&a));
                    CHECK(a, b);
                    break;
                }
                case TEST_REPLACE:
                {
                    char* temp = create_test_string(TEST_RAND(a.size));
                    const size_t index = TEST_RAND(a.size);
                    const size_t size = TEST_RAND(a.size);
                    str_replace(&a, index, size, temp);
                    b.replace(index, size, temp);
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND:
                {
                    const size_t size = TEST_RAND(3);
                    char* temp = create_test_string(size);
                    assert(str_find(&a, temp) == b.find(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_RFIND:
                {
                    char* temp = create_test_string(TEST_RAND(3));
                    assert(str_rfind(&a, temp) == b.rfind(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND_FIRST_OF:
                {
                    const size_t size = TEST_RAND(3);
                    char* temp = create_test_string(size);
                    assert(str_find_first_of(&a, temp) == b.find_first_of(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND_LAST_OF:
                {
                    const size_t size = TEST_RAND(3);
                    char* temp = create_test_string(size);
                    assert(str_find_last_of(&a, temp) == b.find_last_of(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND_FIRST_NOT_OF:
                {
                    const size_t size = TEST_RAND(192);
                    char* temp = create_test_string(size);
                    assert(str_find_first_not_of(&a, temp) == b.find_first_not_of(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND_LAST_NOT_OF:
                {
                    const size_t size = TEST_RAND(192);
                    char* temp = create_test_string(size);
                    assert(str_find_last_not_of(&a, temp) == b.find_last_not_of(temp));
                    free(temp);
                    CHECK(a, b);
                    break;
                }
                case TEST_SUBSTR:
                {
                    const size_t index = TEST_RAND(a.size);
                    const size_t size = TEST_RAND(a.size - index);
                    if(size > MIN_STR_SIZE)
                    {
                        str substr1 = str_substr(&a, index, size);
                        std::string substr2 = b.substr(index, size);
                        CHECK(substr1, substr2);
                        str_free(&substr1);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_COMPARE:
                {
                    size_t size = TEST_RAND(512);
                    char* _ta = create_test_string(size);
                    char* _tb = create_test_string(size);
                    str _a = str_init(_ta);
                    str _b = str_init(_tb);
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
                    CHECK(a, b);
                    break;
                }
                case TEST_CLEAR:
                {
                    str_clear(&a);
                    b.clear();
                    CHECK(a, b);
                    break;
                }
                case TEST_ERASE:
                {
                    const size_t index = TEST_RAND(a.size);
                    b.erase(b.begin() + index);
                    str_erase(&a, index);
                    CHECK(a, b);
                    break;
                }
                case TEST_INSERT:
                {
                    size_t letters = TEST_RAND(512);
                    for(size_t count = 0; count < letters; count++)
                    {
                        const char value = TEST_RAND(ALPHA_LETTERS);
                        const size_t index = TEST_RAND(a.size);
                        b.insert(b.begin() + index, value);
                        str_insert(&a, index, value);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_RESIZE:
                {
                    const size_t resize = 3 * TEST_RAND(a.size);
                    b.resize(resize);
                    str_resize(&a, resize, '\0');
                    CHECK(a, b);
                    break;
                }
                case TEST_SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    str_shrink_to_fit(&a);
                    CHECK(a, b);
                    break;
                }
                case TEST_SORT:
                {
                    str_sort(&a, char_compare);
                    std::sort(b.begin(), b.end());
                    CHECK(a, b);
                    break;
                }
                case TEST_COPY:
                {
                    str ca = str_copy(&a);
                    std::string cb = b;
                    CHECK(ca, cb);
                    str_free(&ca);
                    CHECK(a, b);
                    break;
                }
                case TEST_ASSIGN:
                {
                    const char value = TEST_RAND(ALPHA_LETTERS);
                    size_t assign_size = TEST_RAND(a.size);
                    str_assign(&a, assign_size, value);
                    b.assign(assign_size, value);
                    CHECK(a, b);
                    break;
                }
                case TEST_SWAP:
                {
                    str aa = str_copy(&a);
                    str aaa = str_init("");
                    std::string cb = b;
                    std::string bbb;
                    str_swap(&aaa, &aa);
                    std::swap(cb, bbb);
                    CHECK(aaa, bbb);
                    str_free(&aaa);
                    str_free(&aa);
                    CHECK(a, b);
                    break;
                }
                case TEST_COUNT:
                {
                    const char value = TEST_RAND(ALPHA_LETTERS);
                    assert(count(b.begin(), b.end(), value) == str_count(&a, value));
                    CHECK(a, b);
                    break;
                }
            }
            CHECK(a, b);
            str_free(&a);
            free(base);
        }
    }
    TEST_PASS(__FILE__);
}
