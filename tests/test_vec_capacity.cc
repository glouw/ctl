#include "test.h"

#include <stdint.h>

#define CTL_POD
#define CTL_T uint8_t
#include <vec.h>

#define CTL_POD
#define CTL_T uint16_t
#include <vec.h>

#define CTL_POD
#define CTL_T uint32_t
#include <vec.h>

#define CTL_POD
#define CTL_T uint64_t
#include <vec.h>

#define CTL_POD
#define CTL_T float
#include <vec.h>

#define CTL_POD
#define CTL_T double
#include <vec.h>

#include <vector>

#define ASSERT_EQUAL_SIZE(x, y) (assert(x.size() == y.size))

#define ASSERT_EQUAL_CAP(x, y) (assert(x.capacity() == y.capacity))

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t loop = 0; loop < loops; loop++)
    {
        uint8_t value = TEST_RAND(UINT8_MAX); // SMALLEST SIZE.
        size_t size = TEST_RAND(TEST_MAX_SIZE);
        enum
        {
            MODE_DIRECT,
            MODE_GROWTH,
            MODE_TOTAL
        };
        for(size_t mode = MODE_DIRECT; mode < MODE_TOTAL; mode++)
        {
            std::vector<uint8_t>  a;
            std::vector<uint16_t> b;
            std::vector<uint32_t> c;
            std::vector<uint64_t> d;
            std::vector<float>    e;
            std::vector<double>   f;
            vec_uint8_t  aa = vec_uint8_t_init();
            vec_uint16_t bb = vec_uint16_t_init();
            vec_uint32_t cc = vec_uint32_t_init();
            vec_uint64_t dd = vec_uint64_t_init();
            vec_float    ee = vec_float_init();
            vec_double   ff = vec_double_init();
            if(mode == MODE_DIRECT)
            {
                a.resize (size);
                b.resize (size);
                c.resize (size);
                d.resize (size);
                e.resize (size);
                f.resize (size);
                vec_uint8_t_resize  (&aa, size);
                vec_uint16_t_resize (&bb, size);
                vec_uint32_t_resize (&cc, size);
                vec_uint64_t_resize (&dd, size);
                vec_float_resize    (&ee, size);
                vec_double_resize   (&ff, size);
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    a.push_back (value);
                    b.push_back (value);
                    c.push_back (value);
                    d.push_back (value);
                    e.push_back (value);
                    f.push_back (value);
                    vec_uint8_t_push_back  (&aa, value);
                    vec_uint16_t_push_back (&bb, value);
                    vec_uint32_t_push_back (&cc, value);
                    vec_uint64_t_push_back (&dd, value);
                    vec_float_push_back    (&ee, value);
                    vec_double_push_back   (&ff, value);
                }
            }
            ASSERT_EQUAL_SIZE (a, aa);
            ASSERT_EQUAL_SIZE (b, bb);
            ASSERT_EQUAL_SIZE (c, cc);
            ASSERT_EQUAL_SIZE (d, dd);
            ASSERT_EQUAL_SIZE (e, ee);
            ASSERT_EQUAL_SIZE (f, ff);
            ASSERT_EQUAL_CAP  (a, aa);
            ASSERT_EQUAL_CAP  (b, bb);
            ASSERT_EQUAL_CAP  (c, cc);
            ASSERT_EQUAL_CAP  (d, dd);
            ASSERT_EQUAL_CAP  (e, ee);
            ASSERT_EQUAL_CAP  (f, ff);
            vec_uint8_t_free  (&aa);
            vec_uint16_t_free (&bb);
            vec_uint32_t_free (&cc);
            vec_uint64_t_free (&dd);
            vec_float_free    (&ee);
            vec_double_free   (&ff);
        }
    }
    TEST_PASS(__FILE__);
}
