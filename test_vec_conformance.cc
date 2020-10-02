#include <stdio.h>
#include <assert.h>

// CTL
#define T int
#include <vec.h>

// STL
#include <vector>

int main()
{
    const int A = 11;
    const int B = 22;
    const int C = 33;

    // CTL Setup.
    vec_int a = vec_int_construct(NULL, NULL);
    vec_int_push_back(&a, A);
    vec_int_push_back(&a, B);
    vec_int_push_back(&a, C);
    vec_int_push_back(&a, A);
    vec_int_push_back(&a, B);
    vec_int_push_back(&a, C);
    vec_int_push_back(&a, A);
    vec_int_push_back(&a, B);
    vec_int_push_back(&a, C);

    // STL Setup.
    std::vector<int> b;
    b.push_back(A);
    b.push_back(B);
    b.push_back(C);
    b.push_back(A);
    b.push_back(B);
    b.push_back(C);
    b.push_back(A);
    b.push_back(B);
    b.push_back(C);

    // TEST
    // CTL_FOREACH vs STL at().
    {
        vec_int_it it = vec_int_it_construct(&a, 0, a.size, 1);
        CTL_FOR(it, {
            assert(*it.value == b.at(it.index));
        })
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL `for` vs CTL at().
    {
        size_t index = 0;
        for(auto& value : b)
        {
            assert(value == *vec_int_at(&a, index));
            index += 1;
        }
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL front() and back vs CTL front() and back().
    {
        assert(*vec_int_front(&a) == b.front());
        assert(*vec_int_back(&a) == b.back());
        assert(a.size == b.size() && a.capacity == b.capacity());
    }

    // TEST
    // STL vs CTL Memory Management
    {
        {
            size_t size = 1;
            b.resize(size);
            vec_int_resize(&a, size);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t capacity = 10;
            b.reserve(capacity);
            vec_int_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t capacity = 128;
            b.reserve(capacity);
            vec_int_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t capacity = 65536;
            b.resize(capacity);
            vec_int_resize(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
            vec_int_push_back(&a, A);
            vec_int_push_back(&a, B);
            vec_int_push_back(&a, C);
            b.push_back(A);
            b.push_back(B);
            b.push_back(C);
            for(size_t i = 0; i < capacity; i++)
                assert(*vec_int_at(&a, i) == b.at(i));
        }{
            size_t size = 0;
            b.resize(size);
            vec_int_resize(&a, size);
            assert(b.empty());
            assert(vec_int_empty(&a));
        }{
            size_t size = 1;
            b.resize(size);
            vec_int_resize(&a, size);
            assert(!b.empty());
            assert(!vec_int_empty(&a));
        }{
            size_t capacity = 4096;
            b.reserve(capacity);
            vec_int_reserve(&a, capacity);
            assert(a.size == b.size() && a.capacity == b.capacity());
            b.shrink_to_fit();
            vec_int_shrink_to_fit(&a);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            vec_int_push_back(&a, A);
            b.push_back(A);
            b.pop_back();
            vec_int_pop_back(&a);
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t index = 0;
            int value = 42;
            vec_int_push_back(&a, A);
            b.push_back(A);
            b[index] = value;
            vec_int_set(&a, index, value);
            assert(*vec_int_at(&a, index) == b.at(index));
            assert(a.size == b.size() && a.capacity == b.capacity());
        }{
            size_t index = 1;
            vec_int_push_back(&a, A);
            vec_int_push_back(&a, B);
            vec_int_push_back(&a, C);
            b.push_back(A);
            b.push_back(B);
            b.push_back(C);
            b.erase(b.begin() + index);
            vec_int_erase(&a, index);
            for(size_t i = 0; i < b.size(); i++)
                assert(*vec_int_at(&a, i) == b.at(i));
        }{
            vec_int c = vec_int_copy(&a);
            std::vector<int> d = b;
            for(size_t i = 0; i < b.size(); i++)
                assert(*vec_int_at(&c, i) == d.at(i));
            vec_int_destruct(&c);
        }{
            size_t index = 2;
            size_t value = 100;
            vec_int_push_back(&a, A);
            vec_int_push_back(&a, B);
            vec_int_push_back(&a, C);
            b.push_back(A);
            b.push_back(B);
            b.push_back(C);
            b.insert(b.begin() + index, value);
            vec_int_insert(&a, index, value);
            for(size_t i = 0; i < b.size(); i++)
                assert(*vec_int_at(&a, i) == b.at(i));
        }{
            std::vector<int> w;
            w.push_back(A);
            w.push_back(B);
            w.push_back(C);
            std::vector<int> x;
            x.push_back(C);
            x.push_back(B);
            x.push_back(A);
            vec_int y = vec_int_construct(NULL, NULL);
            vec_int_push_back(&y, A);
            vec_int_push_back(&y, B);
            vec_int_push_back(&y, C);
            vec_int z = vec_int_construct(NULL, NULL);
            vec_int_push_back(&z, C);
            vec_int_push_back(&z, B);
            vec_int_push_back(&z, A);
            w.swap(x);
            vec_int_swap(&y, &z);
            for(size_t i = 0; i < w.size(); i++) assert(*vec_int_at(&y, i) == w.at(i));
            for(size_t i = 0; i < x.size(); i++) assert(*vec_int_at(&z, i) == x.at(i));
            vec_int_destruct(&y);
            vec_int_destruct(&z);
        }
    }

    // CTL Cleanup.
    vec_int_destruct(&a);
    printf("%s: PASSED\n", __FILE__);
}
