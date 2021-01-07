//
// Unordered Set
//

#ifndef T
#error "Template type T undefined for <ust.h>"
#endif

#include <ctl.h>

#define A JOIN(ust, T)
#define B JOIN(A, node)
#define I JOIN(A, it)

typedef struct B
{
    T value;
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
    float max_load_factor;
    int saturated;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    B* end;
    B* node;
    T* ref;
    B* next;
    A* container;
    size_t bucket;
    int done;
}
I;

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

static inline void
JOIN(A, max_load_factor)(A* self, float f)
{
    self->max_load_factor = f;
}

static inline size_t
JOIN(I, index)(A* self, T value)
{
    return self->hash(&value) % self->bucket_count;
}

static inline void
JOIN(I, update)(I* self)
{
    self->node = self->next;
    self->ref = &self->node->value;
    self->next = self->node->next;
    self->bucket = JOIN(I, index)(self->container, *self->ref);
}

static inline void
JOIN(I, step)(I* self)
{
    if(self->next == JOIN(A, end)(self->container))
    {
        for(size_t i = self->bucket + 1; i < self->container->bucket_count; i++)
            if((self->next = self->container->bucket[i]))
            {
                JOIN(I, update)(self);
                return;
            }
        self->done = 1;
    }
    else
        JOIN(I, update)(self);
}

static inline I
JOIN(I, range)(A* container, B* begin, B* end)
{
    static I zero;
    I self = zero;
    if(begin)
    {
        self.step = JOIN(I, step);
        self.node = begin;
        self.ref = &self.node->value;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.bucket = JOIN(I, index)(container, *self.ref);
    }
    else
        self.done = 1;
    return self;
}

static inline B**
JOIN(A, bucket)(A* self, T value)
{
    size_t index = JOIN(I, index)(self, value);
    return &self->bucket[index];
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

static inline I
JOIN(I, each)(A* a)
{
    return JOIN(A, empty)(a)
         ? JOIN(I, range)(a, NULL, NULL)
         : JOIN(I, range)(a, JOIN(A, begin)(a), JOIN(A, end)(a));
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
            if(self->equal(&value, &n->value))
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

static const size_t
JOIN(A, primes)[] = {
    13, 29, 59, 127, 257, 541, 1109, 2357, 5087, 10273, 20753,
    42043, 85229, 172933, 351061, 712697, 1447153, 2938679,
    5967347, 12117689, 24607243, 49969847, 101473717
};

static const size_t
JOIN(A, primes_size) = len(JOIN(A, primes));

static const size_t
JOIN(A, last_prime) = JOIN(A, primes)[JOIN(A, primes_size) - 1];

static inline int
JOIN(A, __next_prime)(size_t n)
{
    for(size_t i = 0; i < JOIN(A, primes_size); i++)
    {
        size_t p = JOIN(A, primes)[i];
        if(n < p)
            return p;
    }
    return JOIN(A, last_prime);
}

static inline B*
JOIN(B, init)(T value)
{
    B* n = (B*) malloc(sizeof(B));
    n->value = value;
    n->next = NULL;
    return n;
}

static inline B*
JOIN(B, push)(B* bucket, B* n)
{
    n->next = bucket;
    return n;
}

static inline size_t
JOIN(A, bucket_size)(A* self, size_t index)
{
    size_t size = 0;
    for(B* n = self->bucket[index]; n; n = n->next)
        size += 1;
    return size;
}

static inline void
JOIN(A, free_node)(A* self, B* n)
{
    if(self->free)
        self->free(&n->value);
    free(n);
    self->size -= 1;
}

static inline float
JOIN(A, load_factor)(A* self)
{
    return (float) self->size / (float) self->bucket_count;
}

static inline void
JOIN(A, reserve)(A* self, size_t desired_count)
{
    self->bucket_count = JOIN(A, __next_prime)(desired_count);
    if(self->bucket_count == JOIN(A, last_prime))
        self->saturated = 1;
    free(self->bucket);
    self->bucket = (B**) calloc(self->bucket_count, sizeof(B*));
}

static inline A
JOIN(A, init)(size_t (*_hash)(T*), int (*_equal)(T*, T*))
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
    JOIN(A, max_load_factor)(&self, 1.0f);
    return self;
}

static inline void
JOIN(A, rehash)(A* self, size_t desired_count)
{
    if(!self->saturated)
    {
        A rehashed = JOIN(A, init)(self->hash, self->equal);
        JOIN(A, reserve)(&rehashed, desired_count);
        foreach(A, self, it)
        {
            B** bucket = JOIN(A, bucket)(&rehashed, it.node->value);
            *bucket = JOIN(B, push)(*bucket, it.node);
        }
        rehashed.size = self->size;
        free(self->bucket);
        *self = rehashed;
    }
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
        *bucket = JOIN(B, push)(*bucket, JOIN(B, init)(value));
        self->size += 1;
        if(JOIN(A, load_factor)(self) > self->max_load_factor)
            JOIN(A, rehash)(self, 2 * self->bucket_count);
    }
}

static inline size_t
JOIN(A, count)(A* self, T value)
{
    return JOIN(A, find)(self, value) ? 1 : 0;
}

static inline void
JOIN(A, erase)(A* self, T value)
{
    B** bucket = JOIN(A, bucket)(self, value);
    B* prev = NULL;
    B* n = *bucket;
    while(n)
    {
        B* next = n->next;
        if(self->equal(&n->value, &value))
        {
            JOIN(A, free_node)(self, n);
            if(prev)
                prev->next = next;
            else
                *bucket = next;
        }
        prev = n;
        n = next;
    }
}

static inline void
JOIN(A, remove_if)(A* self, int (*_match)(T*))
{
    foreach(A, self, it)
        if(_match(it.ref))
            JOIN(A, free_node)(self, it.node);
}

static inline A
JOIN(A, copy)(A* self)
{
    A other = JOIN(A, init)(self->hash, self->equal);
    JOIN(A, reserve)(&other, self->bucket_count);
    foreach(A, self, it)
        JOIN(A, insert)(&other, self->copy(it.ref));
    return other;
}

static inline A
JOIN(A, intersection)(A* a, A* b)
{
    A self = JOIN(A, init)(a->hash, a->equal);
    foreach(A, a, i)
        if(JOIN(A, find)(b, *i.ref))
            JOIN(A, insert)(&self, self.copy(i.ref));
    return self;
}

static inline A
JOIN(A, union)(A* a, A* b)
{
    A self = JOIN(A, init)(a->hash, a->equal);
    foreach(A, a, i) JOIN(A, insert)(&self, self.copy(i.ref));
    foreach(A, b, i) JOIN(A, insert)(&self, self.copy(i.ref));
    return self;
}

static inline A
JOIN(A, difference)(A* a, A* b)
{
    A self = JOIN(A, init)(a->hash, a->equal);
    foreach(A, b, i) JOIN(A, erase)(&self, *i.ref);
    return self;
}

static inline A
JOIN(A, symmetric_difference)(A* a, A* b)
{
    A self = JOIN(A, union)(a, b);
    A intersection = JOIN(A, intersection)(a, b);
    foreach(A, &intersection, i) JOIN(A, erase)(&self, *i.ref);
    JOIN(A, free)(&intersection);
    return self;
}

#undef T
#undef A
#undef B
#undef I
