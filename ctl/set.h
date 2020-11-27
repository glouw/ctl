#define HOLD
#define MAKE_SET
#include <map.h>

#define A JOIN(set, T)

static inline A
JOIN(A, intersection)(A* a, A* b)
{
    A self = JOIN(A, create)(a->compare);
    foreach(A, a, i,
        if(JOIN(A, find)(b, i.node->key))
            JOIN(A, insert)(&self, self.copy(&i.node->key));
    )
    return self;
}

static inline A
JOIN(A, union)(A* a, A* b)
{
    A self = JOIN(A, create)(a->compare);
    foreach(A, a, i, JOIN(A, insert)(&self, self.copy(&i.node->key));)
    foreach(A, b, i, JOIN(A, insert)(&self, self.copy(&i.node->key));)
    return self;
}

static inline A
JOIN(A, difference)(A* a, A* b)
{
    A self = JOIN(A, copy)(a);
    foreach(A, b, i, JOIN(A, erase)(&self, i.node->key);)
    return self;
}

static inline A
JOIN(A, symmetric_difference)(A* a, A* b)
{
    A self = JOIN(A, union)(a, b);
    A intersection = JOIN(A, intersection)(a, b);
    foreach(A, &intersection, i, JOIN(A, erase)(&self, i.node->key);)
    JOIN(A, free)(&intersection);
    return self;
}

#undef T // SEE HOLD
#undef A
