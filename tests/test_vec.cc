#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>

typedef char* str;

static void
str_destruct(str* s)
{
    free(*s);
}

static str
str_construct(const char* s)
{
    return strcpy((char*) malloc(strlen(s) + 1), s);
}

static str
str_copy(str* s)
{
    return str_construct(*s);
}

// CTL
#define T str
#include <vec.h>

// STL
#include <vector>

static int
str_cmp(const void* a, const void* b)
{
    return strcmp((str) a, (str) b);
}

int
main(void)
{
    const char* const A = "A";
    const char* const B = "B";
    const char* const C = "C";

    // CTL Setup.
    vec_str a = vec_str_construct(str_destruct, str_copy);
    vec_str_push_back(&a, str_construct(A));
    vec_str_push_back(&a, str_construct(B));
    vec_str_push_back(&a, str_construct(C));
    vec_str_push_back(&a, str_construct(A));
    vec_str_push_back(&a, str_construct(B));
    vec_str_push_back(&a, str_construct(C));
    vec_str_push_back(&a, str_construct(A));
    vec_str_push_back(&a, str_construct(B));
    vec_str_push_back(&a, str_construct(C));

    // STL Setup.
    std::vector<std::string> b;
    b.push_back(std::string(A));
    b.push_back(std::string(B));
    b.push_back(std::string(C));
    b.push_back(std::string(A));
    b.push_back(std::string(B));
    b.push_back(std::string(C));
    b.push_back(std::string(A));
    b.push_back(std::string(B));
    b.push_back(std::string(C));

    // TEST
    // CTL_FOR vs STL at().
    {
        vec_str_it it = vec_str_it_construct(&a, 0, a.size, 1);
        CTL_FOR(it, {
            assert(strcmp(*it.value, b.at(it.index).c_str()) == 0);
        })
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL `for` vs CTL at().
    {
        size_t index = 0;
        for(auto& value : b)
        {
            assert(strcmp(value.c_str(), *vec_str_at(&a, index)) == 0);
            index += 1;
        }
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL front() and back vs CTL front() and back().
    {
        assert(strcmp(*vec_str_front(&a), b.front().c_str()) == 0);
        assert(strcmp(*vec_str_back(&a), b.back().c_str()) == 0);
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL vs CTL Memory Management
    {
        {
            size_t size = 1;
            b.resize(size);
            vec_str_resize(&a, size);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t capacity = 10;
            b.reserve(capacity);
            vec_str_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t capacity = 128;
            b.reserve(capacity);
            vec_str_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t size = 65536;
            b.resize(size);
            vec_str_resize(&a, size);
            assert(a.size == b.size() && a.capacity == b.capacity());
            vec_str_push_back(&a, str_construct(A));
            vec_str_push_back(&a, str_construct(B));
            vec_str_push_back(&a, str_construct(C));
            b.push_back(std::string(A));
            b.push_back(std::string(B));
            b.push_back(std::string(C));
            for(size_t i = 0; i < a.size; i++)
            {
                str s = *vec_str_at(&a, i); // VECTOR RESIZES INITIALIZE VALUES TO NULL.
                if(s)
                    assert(strcmp(s, b.at(i).c_str()) == 0);
            }
        }{
            size_t size = 0;
            b.resize(size);
            vec_str_resize(&a, size);
            assert(b.empty());
            assert(vec_str_empty(&a));
        }{
            size_t size = 1;
            b.resize(size);
            vec_str_resize(&a, size);
            assert(!b.empty());
            assert(!vec_str_empty(&a));
        }{
            size_t capacity = 4096;
            b.reserve(capacity);
            vec_str_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
            b.shrink_to_fit();
            vec_str_shrink_to_fit(&a);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            vec_str_push_back(&a, str_construct(A));
            b.push_back(std::string(A));
            b.pop_back();
            vec_str_pop_back(&a);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t index = 0;
            const char* value = "TEST";
            vec_str_push_back(&a, str_construct(A));
            b.push_back(std::string(A));
            b[index] = std::string(value);
            vec_str_set(&a, index, str_construct(value));
            assert(*vec_str_at(&a, index) == b.at(index));
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t index = 1;
            vec_str_push_back(&a, str_construct(A));
            vec_str_push_back(&a, str_construct(B));
            vec_str_push_back(&a, str_construct(C));
            b.push_back(std::string(A));
            b.push_back(std::string(B));
            b.push_back(std::string(C));
            b.erase(b.begin() + index);
            vec_str_erase(&a, index);
            for(size_t i = 0; i < b.size(); i++)
                assert(strcmp(*vec_str_at(&a, i), b.at(i).c_str()) == 0);
        }{
            vec_str c = vec_str_copy(&a);
            std::vector<std::string> d = b;
            for(size_t i = 0; i < b.size(); i++)
                assert(*vec_str_at(&c, i) == d.at(i));
            vec_str_destruct(&c);
        }{
            size_t index = 2;
            const char* value = "ANOTHER TEST";
            vec_str_push_back(&a, str_construct(A));
            vec_str_push_back(&a, str_construct(B));
            vec_str_push_back(&a, str_construct(C));
            b.push_back(std::string(A));
            b.push_back(std::string(B));
            b.push_back(std::string(C));
            b.insert(b.begin() + index, std::string(value));
            vec_str_insert(&a, index, str_construct(value));
            for(size_t i = 0; i < b.size(); i++)
                assert(*vec_str_at(&a, i) == b.at(i));
        }{
            std::vector<std::string> w;
            w.push_back(std::string(A));
            w.push_back(std::string(B));
            w.push_back(std::string(C));
            std::vector<std::string> x;
            x.push_back(std::string(C));
            x.push_back(std::string(B));
            x.push_back(std::string(A));
            vec_str y = vec_str_construct(str_destruct, str_copy);
            vec_str_push_back(&y, str_construct(A));
            vec_str_push_back(&y, str_construct(B));
            vec_str_push_back(&y, str_construct(C));
            vec_str z = vec_str_construct(str_destruct, str_copy);
            vec_str_push_back(&z, str_construct(C));
            vec_str_push_back(&z, str_construct(B));
            vec_str_push_back(&z, str_construct(A));
            w.swap(x);
            vec_str_swap(&y, &z);
            for(size_t i = 0; i < w.size(); i++) assert(strcmp(*vec_str_at(&y, i), w.at(i).c_str()) == 0);
            for(size_t i = 0; i < x.size(); i++) assert(strcmp(*vec_str_at(&z, i), x.at(i).c_str()) == 0);
            vec_str_sort(&y, str_cmp);
            vec_str_destruct(&y);
            vec_str_destruct(&z);
        }{
            size_t size = 65536;
            std::vector<str> x; // NULL POINTER TEST.
            vec_str y = vec_str_construct(str_destruct, str_copy);
            x.resize(size);
            vec_str_resize(&y, size);
            for(size_t i = 0; i < size; i++)
            {
                assert(x.at(i) == NULL);
                assert(*vec_str_at(&y, i) == NULL);
            }
            vec_str_destruct(&y);
        }
    }
    vec_str_destruct(&a);
    printf("%s: PASSED\n", __FILE__);
}
