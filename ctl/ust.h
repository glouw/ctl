//
// Unordered Set
//

#ifndef T
#error "Template type T undefined for <ust.h>"
#endif

#include <ctl.h>

#define A JOIN(ust, T)
#define B JOIN(A, node)
#define Z JOIN(A, it)

typedef struct B
{
    T key;
    struct B* next;
}
B;

typedef struct A
{
    void (*free)(T*);
    T (*copy)(T*);
    size_t (*hash)(T*);
    int (*equal)(T*, T*);
    B** bucket;
    size_t size;
    size_t bucket_count;
}
A;

typedef struct Z
{
    void (*step)(struct Z*);
    B* end;
    B* node;
    T* ref;
    B* next;
    A* container;
    size_t index;
    int done;
}
Z;

static inline B*
JOIN(A, begin)(A* self)
{
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        B* node = self->bucket[i];
        if(node)
            return node;
    }
    return NULL;
}

static inline B*
JOIN(A, end)(A* self)
{
    (void) self;
    return NULL;
}

static inline size_t
JOIN(Z, index)(A* self, T value)
{
    return self->hash(&value) % self->bucket_count;
}

static inline void
JOIN(Z, update)(Z* self)
{
    self->node = self->next;
    self->ref = &self->node->key;
    self->next = self->node->next;
}

static inline int
JOIN(Z, scan)(Z* self)
{
    for(size_t i = self->index + 1; i < self->container->bucket_count; i++)
    {
        self->next = self->container->bucket[i];
        if(self->next)
        {
            self->index = i;
            JOIN(Z, update)(self);
            return 1;
        }
    }
    return 0;
}

static inline void
JOIN(Z, step)(Z* self)
{
    if(self->next == JOIN(A, end)(self->container))
    {
        if(!JOIN(Z, scan)(self))
            self->done = 1;
    }
    else
        JOIN(Z, update)(self);
}

static inline Z
JOIN(Z, range)(A* container, B* begin, B* end)
{
    static Z zero;
    Z self = zero;
    if(begin)
    {
        self.step = JOIN(Z, step);
        self.node = begin;
        self.ref = &self.node->key;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.index = JOIN(Z, index)(container, *self.ref);
    }
    else
        self.done = 1;
    return self;
}

static inline B**
JOIN(A, bucket)(A* self, T value)
{
    size_t index = JOIN(Z, index)(self, value);
    return &self->bucket[index];
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

static inline Z
JOIN(Z, each)(A* a)
{
    return JOIN(A, empty)(a)
         ? JOIN(Z, range)(a, NULL, NULL)
         : JOIN(Z, range)(a, JOIN(A, begin)(a), JOIN(A, end)(a));
}

static inline T
JOIN(A, implicit_copy)(T* self)
{
    return *self;
}

static inline B*
JOIN(A, find)(A* self, T value)
{
    if(!JOIN(A, empty)(self))
    {
        B** bucket = JOIN(A, bucket)(self, value);
        for(B* n = *bucket; n; n = n->next)
            if(self->equal(&value, &n->key))
                return n;
    }
    return NULL;
}

static inline int
JOIN(A, equal)(A* a, A* b)
{
    size_t count_a = 0;
    size_t count_b = 0;
    foreach(A, a, it) if(JOIN(A, find)(b, *it.ref)) count_a += 1;
    foreach(A, b, it) if(JOIN(A, find)(a, *it.ref)) count_b += 1;
    return count_a == count_b;
}

static inline void
JOIN(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline size_t
JOIN(A, closest_prime)(size_t number)
{
    static uint32_t primes[] = {
        2, 3, 5, 7, 11,
        13, 17, 19, 23, 29, 31,
        37, 41, 43, 47, 53, 59,
        61, 67, 71, 73, 79, 83,
        89, 97, 103, 109, 113, 127,
        137, 139, 149, 157, 167, 179,
        193, 199, 211, 227, 241, 257,
        277, 293, 313, 337, 359, 383,
        409, 439, 467, 503, 541, 577,
        619, 661, 709, 761, 823, 887,
        953, 1031, 1109, 1193, 1289, 1381,
        1493, 1613, 1741, 1879, 2029, 2179,
        2357, 2549, 2753, 2971, 3209, 3469,
        3739, 4027, 4349, 4703, 5087, 5503,
        5953, 6427, 6949, 7517, 8123, 8783,
        9497, 10273, 11113, 12011, 12983, 14033,
        15173, 16411, 17749, 19183, 20753, 22447,
        24281, 26267, 28411, 30727, 33223, 35933,
        38873, 42043, 45481, 49201, 53201, 57557,
        62233, 67307, 72817, 78779, 85229, 92203,
        99733, 107897, 116731, 126271, 136607, 147793,
        159871, 172933, 187091, 202409, 218971, 236897,
        256279, 277261, 299951, 324503, 351061, 379787,
        410857, 444487, 480881, 520241, 562841, 608903,
        658753, 712697, 771049, 834181, 902483, 976369,
        1056323, 1142821, 1236397, 1337629, 1447153, 1565659,
        1693859, 1832561, 1982627, 2144977, 2320627, 2510653,
        2716249, 2938679, 3179303, 3439651, 3721303, 4026031,
        4355707, 4712381, 5098259, 5515729, 5967347, 6456007,
        6984629, 7556579, 8175383, 8844859, 9569143, 10352717,
        11200489, 12117689, 13109983, 14183539, 15345007, 16601593,
        17961079, 19431899, 21023161, 22744717, 24607243, 26622317,
        28802401, 31160981, 33712729, 36473443, 39460231, 42691603,
        46187573, 49969847, 54061849, 58488943, 63278561, 68460391,
        74066549, 80131819, 86693767, 93793069, 101473717, 109783337,
        118773397, 128499677, 139022417, 150406843, 162723577, 176048909,
        190465427, 206062531, 222936881, 241193053, 260944219, 282312799,
        305431229, 330442829, 357502601, 386778277, 418451333, 452718089,
        489790921, 529899637, 573292817, 620239453, 671030513, 725980837,
        785430967, 849749479, 919334987, 994618837, 1076067617, 1164186217,
        1259520799, 1362662261, 1474249943, 1594975441, 1725587117,
    };
    size_t min = primes[0];
    if(number < min)
        return min;
    size_t size = len(primes);
    for(size_t i = 0; i < size - 1; i++)
    {
        size_t a = primes[i + 0];
        size_t b = primes[i + 1];
        if(number >= a && number <= b)
            return number == a ? a : b;
    }
    return primes[size - 1];
}

static inline B*
JOIN(B, init)(T value)
{
    B* n = (B*) malloc(sizeof(B));
    n->key = value;
    n->next = NULL;
    return n;
}

static inline void
JOIN(B, push)(A* self, B** bucket, B* n)
{
    n->next = *bucket;
    self->size += 1;
    *bucket = n;
}

static inline size_t
JOIN(A, bucket_size)(A* self, size_t index)
{
    size_t size = 0;
    for(B* n = self->bucket[index]; n; n = n->next)
        size += 1;
    return size;
}

static inline float
JOIN(A, load_factor)(A* self)
{
    return (float) self->size / (float) self->bucket_count;
}

static inline A
JOIN(A, init)(size_t _hash(T*), int _equal(T*, T*))
{
    static A zero;
    A self = zero;
    self.hash = _hash;
    self.equal = _equal;
#ifdef P
#undef P
    self.copy = JOIN(A, implicit_copy);
#else
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#endif
    return self;
}

static inline void
JOIN(A, rehash)(A* self, size_t desired_count);

static inline void
JOIN(A, reserve)(A* self, size_t desired_count)
{
    if(self->size > 0)
        JOIN(A, rehash)(self, desired_count);
    else
    {
        size_t bucket_count = JOIN(A, closest_prime)(desired_count);
        B** temp = (B**) calloc(bucket_count, sizeof(B*));
        for(size_t i = 0; i < self->bucket_count; i++)
            temp[i] = self->bucket[i];
        free(self->bucket);
        self->bucket = temp;
        self->bucket_count = bucket_count;
    }
}

static inline void
JOIN(A, rehash)(A* self, size_t desired_count)
{
    if(desired_count <= self->size)
        desired_count = self->size + 1;
    size_t expected = JOIN(A, closest_prime)(desired_count);
    if(expected != self->bucket_count)
    {
        A rehashed = JOIN(A, init)(self->hash, self->equal);
        JOIN(A, reserve)(&rehashed, desired_count);
        foreach(A, self, it)
        {
            B** bucket = JOIN(A, bucket)(&rehashed, it.node->key);
            JOIN(B, push)(&rehashed, bucket, it.node);
        }
        free(self->bucket);
        *self = rehashed;
    }
}

static inline void
JOIN(A, free_node)(A* self, B* n)
{
    if(self->free)
        self->free(&n->key);
    free(n);
    self->size -= 1;
}

static inline void
JOIN(A, clear)(A* self)
{
    foreach(A, self, it)
        JOIN(A, free_node)(self, it.node);
    for(size_t i = 0; i < self->bucket_count; i++)
        self->bucket[i] = NULL;
}

static inline void
JOIN(A, free)(A* self)
{
    JOIN(A, clear)(self);
    free(self->bucket);
}

static inline void
JOIN(A, insert)(A* self, T value)
{
    if(JOIN(A, empty)(self))
        JOIN(A, rehash)(self, 12);
    if(JOIN(A, find)(self, value))
    {
        if(self->free)
            self->free(&value);
    }
    else
    {
        B** bucket = JOIN(A, bucket)(self, value);
        JOIN(B, push)(self, bucket, JOIN(B, init)(value));
        if(self->size > self->bucket_count)
            JOIN(A, rehash)(self, 2 * self->bucket_count);
    }
}

static inline size_t
JOIN(A, count)(A* self, T value)
{
    return JOIN(A, find)(self, value) ? 1 : 0;
}

static inline void
JOIN(A, linked_erase)(A* self, B** bucket, B* n, B* prev, B* next)
{
    JOIN(A, free_node)(self, n);
    if(prev)
        prev->next = next;
    else
        *bucket = next;
}

static inline void
JOIN(A, erase)(A* self, T value)
{
    if(!JOIN(A, empty)(self))
    {
        B** bucket = JOIN(A, bucket)(self, value);
        B* prev = NULL;
        B* n = *bucket;
        while(n)
        {
            B* next = n->next;
            if(self->equal(&n->key, &value))
            {
                JOIN(A, linked_erase)(self, bucket, n, prev, next);
                break;
            }
            else
                prev = n;
            n = next;
        }
    }
}

static inline size_t
JOIN(A, remove_if)(A* self, int _match(T*))
{
    size_t erases = 0;
    for(size_t i = 0; i < self->bucket_count; i++)
    {
        B** bucket = &self->bucket[i];
        B* prev = NULL;
        B* n = *bucket;
        while(n)
        {
            B* next = n->next;
            if(_match(&n->key))
            {
                JOIN(A, linked_erase)(self, bucket, n, prev, next);
                erases += 1;
            }
            else
                prev = n;
            n = next;
        }
    }
    return erases;
}

static inline A
JOIN(A, copy)(A* self)
{
    A other = JOIN(A, init)(self->hash, self->equal);
    foreach(A, self, it)
        JOIN(A, insert)(&other, self->copy(it.ref));
    return other;
}

#undef T
#undef A
#undef B
#undef Z
