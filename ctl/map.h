// For implementation details:
//     https://web.archive.org/web/20140328232325/http://en.literateprograms.org/Red-black_tree_(C)

#ifdef MAKE_SET
#ifndef T
#error "Template type T undefined for <set.h>"
#endif
#else
#ifndef T
#error "Template type T undefined for <map.h>"
#endif
#ifndef U
#error "Template type U undefined for <map.h>"
#endif
#endif

#include <ctl.h>

#ifdef MAKE_SET
#define A JOIN(set, T)
#else
#define A JOIN(map, JOIN(T, U))
#endif

#define B JOIN(A, node)
#define I JOIN(A, it)

typedef struct B
{
    struct B* l;
    struct B* r;
    struct B* p;
#ifdef MAKE_SET
    T key;
#else
    T first;
    U second;
#endif
    int color; // RED 0, BLK 1.
}
B;

typedef struct
{
    B* root;
    int (*compare)(T*, T*);
#ifdef MAKE_SET
    void (*free)(T*);
    T (*copy)(T*);
#else
    void (*free)(T*, U*);
    void (*copy)(T*, T*, U*, U*);
#endif
    size_t size;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    B* end;
    B* node;
    B* next;
    int done;
}
I;

static inline B*
JOIN(A, begin)(A* self)
{
    return self->root;
}

static inline B*
JOIN(A, end)(A* self)
{
    (void) self;
    return NULL;
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

#ifdef MAKE_SET
static inline T
JOIN(A, implicit_copy)(T* self)
{
    return *self;
}
#else
static inline void
JOIN(A, implicit_copy)(T* a, T* b, U* c, U* d)
{
    *a = *b;
    *c = *d;
}
#endif

static inline A
JOIN(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
    self.copy = JOIN(A, implicit_copy);
#else
#ifdef MAKE_SET
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#else
    self.free = JOIN(T, JOIN(U, free));
    self.copy = JOIN(T, JOIN(U, copy));
#endif
#endif
    return self;
}

static inline A
JOIN(A, create)(int compare(T*, T*))
{
    A self = JOIN(A, init)();
    self.compare = compare;
    return self;
}

static inline A
JOIN(A, init_default)(void)
{
    return JOIN(A, init)();
}

static inline void
JOIN(A, free_node)(A* self, B* node)
{
    if(self->free)
#ifdef MAKE_SET
        self->free(&node->key);
#else
        self->free(&node->first, &node->second);
#endif
    free(node);
}

static inline int
JOIN(B, color)(B* self)
{
    return self ? self->color : 1;
}

static inline int
JOIN(B, is_blk)(B* self)
{
    return JOIN(B, color)(self) == 1;
}

static inline int
JOIN(B, is_red)(B* self)
{
    return JOIN(B, color)(self) == 0;
}

static inline B*
JOIN(B, min)(B* self)
{
    while(self->l)
        self = self->l;
    return self;
}

static inline B*
JOIN(B, max)(B* self)
{
    while(self->r)
        self = self->r;
    return self;
}

static inline B*
JOIN(B, grandfather)(B* self)
{
    return self->p->p;
}

static inline B*
JOIN(B, sibling)(B* self)
{
    if(self == self->p->l)
        return self->p->r;
    else
        return self->p->l;
}

static inline B*
JOIN(B, uncle)(B* self)
{
    return JOIN(B, sibling)(self->p);
}

static inline B*
#ifdef MAKE_SET
JOIN(B, init)(T key, int color)
#else
JOIN(B, init)(T key, U value, int color)
#endif
{
    B* self = (B*) malloc(sizeof(B));
#ifdef MAKE_SET
    self->key = key;
#else
    self->first = key;
    self->second = value;
#endif
    self->color = color;
    self->l = self->r = self->p = NULL;
    return self;
}

static inline B*
JOIN(A, find)(A* self, T key)
{
    B* node = self->root;
    while(node)
    {
#ifdef MAKE_SET
        int diff = self->compare(&key, &node->key);
#else
        int diff = self->compare(&key, &node->first);
#endif
        if(diff == 0)
            return node;
        else
        if(diff < 0)
            node = node->l;
        else
            node = node->r;
    }
    return node;
}

#ifdef MAKE_SET
// NO AT() OPERATION FOR SET.
#else
static inline U*
JOIN(A, at)(A* self, T key)
{
    return &JOIN(A, find)(self, key)->second;
}
#endif

static inline int
JOIN(A, count)(A* self, T key)
{
    return JOIN(A, find)(self, key) ? 1 : 0;
}

static inline int
JOIN(A, contains)(A* self, T key)
{
    return JOIN(A, count)(self, key) == 1;
}

static inline void
JOIN(B, replace)(A* self, B* a, B* b)
{
    if(a->p)
    {
        if(a == a->p->l)
            a->p->l = b;
        else
            a->p->r = b;
    }
    else
        self->root = b;
    if(b)
        b->p = a->p;
}

#ifdef USE_INTERNAL_VERIFY

#include <assert.h>

static inline void
JOIN(B, verify_property_1)(B* self)
{
    assert(JOIN(B, is_red)(self) || JOIN(B, is_blk)(self));
    if(self)
    {
        JOIN(B, verify_property_1)(self->l);
        JOIN(B, verify_property_1)(self->r);
    }
}

static inline void
JOIN(B, verify_property_2)(B* self)
{
    assert(JOIN(B, is_blk)(self));
}

static inline void
JOIN(B, verify_property_4)(B* self)
{
    if(JOIN(B, is_red)(self))
    {
        assert(JOIN(B, is_blk)(self->l));
        assert(JOIN(B, is_blk)(self->r));
        assert(JOIN(B, is_blk)(self->p));
    }
    if(self)
    {
        JOIN(B, verify_property_4)(self->l);
        JOIN(B, verify_property_4)(self->r);
    }
}

static inline void
JOIN(B, count_blk)(B* self, int nodes, int* in_path)
{
    if(JOIN(B, is_blk)(self))
        nodes += 1;
    if(self)
    {
        JOIN(B, count_blk)(self->l, nodes, in_path);
        JOIN(B, count_blk)(self->r, nodes, in_path);
    }
    else
    {
        if(*in_path == -1)
            *in_path = nodes;
        else
            assert(nodes == *in_path);
    }
}

static inline void
JOIN(B, verify_property_5)(B* self)
{
    int in_path = -1;
    JOIN(B, count_blk)(self, 0, &in_path);
}

static inline void
JOIN(A, verify)(A* self)
{
    JOIN(B, verify_property_1)(self->root); // PROPERTY 1: EACH NODE IS EITHER RED OR BLACK.
    JOIN(B, verify_property_2)(self->root); // PROPERTY 2: THE ROOT NODE IS BLACK.
    /* IMPLICIT */                          // PROPERTY 3: LEAVES ARE COLORED BLACK
    JOIN(B, verify_property_4)(self->root); // PROPERTY 4: EVERY RED NODE HAS TWO BLACK NDOES.
    JOIN(B, verify_property_5)(self->root); // PROPERTY 5: ALL PATHS FROM A NODE HAVE THE SAME NUMBER OF BLACK NODES.
}

#endif

static inline void
JOIN(A, rotate_l)(A* self, B* node)
{
    B* r = node->r;
    JOIN(B, replace)(self, node, r);
    node->r = r->l;
    if(r->l)
        r->l->p = node;
    r->l = node;
    node->p = r;
}

static inline void
JOIN(A, rotate_r)(A* self, B* node)
{
    B* l = node->l;
    JOIN(B, replace)(self, node, l);
    node->l = l->r;
    if(l->r)
        l->r->p = node;
    l->r = node;
    node->p = l;
}

static inline void
JOIN(A, insert_1)(A*, B*),
JOIN(A, insert_2)(A*, B*),
JOIN(A, insert_3)(A*, B*),
JOIN(A, insert_4)(A*, B*),
JOIN(A, insert_5)(A*, B*);

static inline void
#ifdef MAKE_SET
JOIN(A, insert)(A* self, T key)
#else
JOIN(A, insert)(A* self, T key, U value)
#endif
{
#ifdef MAKE_SET
    B* insert = JOIN(B, init)(key, 0);
#else
    B* insert = JOIN(B, init)(key, value, 0);
#endif
    if(self->root)
    {
        B* node = self->root;
        while(1)
        {
#ifdef MAKE_SET
            int diff = self->compare(&key, &node->key);
#else
            int diff = self->compare(&key, &node->first);
#endif
            if(diff == 0)
            {
                JOIN(A, free_node)(self, insert);
                return;
            }
            else
            if(diff < 0)
            {
                if(node->l)
                    node = node->l;
                else
                {
                    node->l = insert;
                    break;
                }
            }
            else
            {
                if(node->r)
                    node = node->r;
                else
                {
                    node->r = insert;
                    break;
                }
            }
        }
        insert->p = node;
    }
    else
        self->root = insert;
    JOIN(A, insert_1)(self, insert);
    self->size += 1;
#ifdef USE_INTERNAL_VERIFY
    JOIN(A, verify)(self);
#endif
}

static inline void
JOIN(A, insert_1)(A* self, B* node)
{
    if(node->p)
        JOIN(A, insert_2)(self, node);
    else
        node->color = 1;
}

static inline void
JOIN(A, insert_2)(A* self, B* node)
{
    if(JOIN(B, is_blk)(node->p))
        return;
    else
       JOIN(A, insert_3)(self, node);
}

static inline void
JOIN(A, insert_3)(A* self, B* node)
{
    if(JOIN(B, is_red)(JOIN(B, uncle)(node)))
    {
        node->p->color = 1;
        JOIN(B, uncle)(node)->color = 1;
        JOIN(B, grandfather)(node)->color = 0;
        JOIN(A, insert_1)(self, JOIN(B, grandfather)(node));
    }
    else
        JOIN(A, insert_4)(self, node);
}

static inline void
JOIN(A, insert_4)(A* self, B* node)
{
    if(node == node->p->r && node->p == JOIN(B, grandfather)(node)->l)
    {
        JOIN(A, rotate_l)(self, node->p);
        node = node->l;
    }
    else
    if(node == node->p->l && node->p == JOIN(B, grandfather)(node)->r)
    {
        JOIN(A, rotate_r)(self, node->p);
        node = node->r;
    }
    JOIN(A, insert_5)(self, node);
}

static inline void
JOIN(A, insert_5)(A* self, B* node)
{
    node->p->color = 1;
    JOIN(B, grandfather)(node)->color = 0;
    if(node == node->p->l && node->p == JOIN(B, grandfather)(node)->l)
        JOIN(A, rotate_r)(self, JOIN(B, grandfather)(node));
    else
        JOIN(A, rotate_l)(self, JOIN(B, grandfather)(node));
}

static inline void
JOIN(A, erase_1)(A*, B*),
JOIN(A, erase_2)(A*, B*),
JOIN(A, erase_3)(A*, B*),
JOIN(A, erase_4)(A*, B*),
JOIN(A, erase_5)(A*, B*),
JOIN(A, erase_6)(A*, B*);

static inline void
JOIN(A, erase_node)(A* self, B* node)
{
    if(node->l && node->r)
    {
        B* pred = JOIN(B, max)(node->l);
#ifdef MAKE_SET
        SWAP(T, &node->key, &pred->key);
#else
        SWAP(T, &node->first, &pred->first);
        SWAP(U, &node->second, &pred->second);
#endif
        node = pred;
    }
    B* child = node->r ? node->r : node->l;
    if(JOIN(B, is_blk)(node))
    {
        node->color = JOIN(B, color)(child);
        JOIN(A, erase_1)(self, node);
    }
    JOIN(B, replace)(self, node, child);
    if(node->p == NULL && child)
        child->color = 1;
    JOIN(A, free_node)(self, node);
    self->size -= 1;
#ifdef USE_INTERNAL_VERIFY
    JOIN(A, verify)(self);
#endif
}

static inline void
JOIN(A, erase)(A* self, T key)
{
    B* node = JOIN(A, find)(self, key);
    if(node)
        JOIN(A, erase_node)(self, node);
}

static inline void
JOIN(A, erase_1)(A* self, B* node)
{
    if(node->p)
        JOIN(A, erase_2)(self, node);
}

static inline void
JOIN(A, erase_2)(A* self, B* node)
{
    if(JOIN(B, is_red)(JOIN(B, sibling)(node)))
    {
        node->p->color = 0;
        JOIN(B, sibling)(node)->color = 1;
        if(node == node->p->l)
            JOIN(A, rotate_l)(self, node->p);
        else
            JOIN(A, rotate_r)(self, node->p);
    }
    JOIN(A, erase_3)(self, node);
}

static inline void
JOIN(A, erase_3)(A* self, B* node)
{
    if(JOIN(B, is_blk)(node->p)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node))
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->l)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->r))
    {
        JOIN(B, sibling)(node)->color = 0;
        JOIN(A, erase_1)(self, node->p);
    }
    else
        JOIN(A, erase_4)(self, node);
}

static inline void
JOIN(A, erase_4)(A* self, B* node)
{
    if(JOIN(B, is_red)(node->p)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node))
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->l)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->r))
    {
        JOIN(B, sibling)(node)->color = 0;
        node->p->color = 1;
    }
    else
        JOIN(A, erase_5)(self, node);
}

static inline void
JOIN(A, erase_5)(A* self, B* node)
{
    if(node == node->p->l
    && JOIN(B, is_blk)(JOIN(B, sibling)(node))
    && JOIN(B, is_red)(JOIN(B, sibling)(node)->l)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->r))
    {
        JOIN(B, sibling)(node)->color = 0;
        JOIN(B, sibling)(node)->l->color = 1;
        JOIN(A, rotate_r)(self, JOIN(B, sibling)(node));
    }
    else
    if(node == node->p->r
    && JOIN(B, is_blk)(JOIN(B, sibling)(node))
    && JOIN(B, is_red)(JOIN(B, sibling)(node)->r)
    && JOIN(B, is_blk)(JOIN(B, sibling)(node)->l))
    {
        JOIN(B, sibling)(node)->color = 0;
        JOIN(B, sibling)(node)->r->color = 1;
        JOIN(A, rotate_l)(self, JOIN(B, sibling)(node));
    }
    JOIN(A, erase_6)(self, node);
}

static inline void
JOIN(A, erase_6)(A* self, B* node)
{
    JOIN(B, sibling)(node)->color = JOIN(B, color)(node->p);
    node->p->color = 1;
    if(node == node->p->l)
    {
        JOIN(B, sibling)(node)->r->color = 1;
        JOIN(A, rotate_l)(self, node->p);
    }
    else
    {
        JOIN(B, sibling)(node)->l->color = 1;
        JOIN(A, rotate_r)(self, node->p);
    }
}

static inline void
JOIN(A, clear)(A* self)
{
    while(!JOIN(A, empty)(self))
#ifdef MAKE_SET
        JOIN(A, erase)(self, self->root->key);
#else
        JOIN(A, erase)(self, self->root->first);
#endif
}

static inline void
JOIN(A, free)(A* self)
{
    JOIN(A, clear)(self);
    *self = JOIN(A, init)();
}

static inline void
JOIN(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline B*
JOIN(B, next)(B* self)
{
    if(self->r)
    {
        self = self->r;
        while(self->l)
            self = self->l;
    }
    else
    {
        B* parent = self->p;
        while(parent && self == parent->r)
        {
            self = parent;
            parent = parent->p;
        }
        self = parent;
    }
    return self;
}

static inline void
JOIN(I, step)(I* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->node = self->next;
        self->next = JOIN(B, next)(self->node);
    }
}

static inline I
JOIN(I, range)(B* begin, B* end)
{
    static I zero;
    I self = zero;
    if(begin)
    {
        self.step = JOIN(I, step);
        self.node = JOIN(B, min)(begin);
        self.next = JOIN(B, next)(self.node);
        self.end = end;
    }
    else
        self.done = 1;
    return self;
}

static inline I
JOIN(I, each)(A* a)
{
    return JOIN(I, range)(JOIN(A, begin)(a), JOIN(A, end)(a));
}

static inline int
#ifdef MAKE_SET
JOIN(A, equal)(A* self, A* other, int equal(T*, T*))
#else
JOIN(A, equal)(A* self, A* other, int equal(T*, T*, U*, U*))
#endif
{
    if(self->size != other->size)
        return 0;
    I a = JOIN(I, each)(self);
    I b = JOIN(I, each)(other);
    while(!a.done && !b.done)
    {
#ifdef MAKE_SET
        if(!equal(&a.node->key, &b.node->key))
#else
        if(!equal(&a.node->first, &b.node->first, &a.node->second, &b.node->second))
#endif
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline A
JOIN(A, copy)(A* self)
{
    I it = JOIN(I, each)(self);
    A copy =  JOIN(A, create)(self->compare);
    while(!it.done)
    {
#ifdef MAKE_SET
        JOIN(A, insert)(&copy, self->copy(&it.node->key));
#else
        T first;
        U second;
        self->copy(&first, &it.node->first, &second, &it.node->second);
        JOIN(A, insert)(&copy, first, second);
#endif
        it.step(&it);
    }
    return copy;
}

static inline size_t
#ifdef MAKE_SET
JOIN(A, remove_if)(A* self, int (*match)(T*))
#else
JOIN(A, remove_if)(A* self, int (*match)(T*, U*))
#endif
{
    size_t erases = 0;
#ifdef MAKE_SET
    foreach(A, self, it,
        if(match(&it.node->key))
        {
            JOIN(A, erase_node)(self, it.node);
            erases += 1;
        }
    );
#else
    foreach(A, self, it,
        if(match(&it.node->first, &it.node->second))
        {
            JOIN(A, erase_node)(self, it.node);
            erases += 1;
        }
    );
#endif
    return erases;
}

#undef T
#undef U
#undef A
#undef B
#undef C
#undef D
#undef I

#ifdef USE_INTERNAL_VERIFY
#undef USE_INTERNAL_VERIFY
#endif

#ifdef MAKE_SET
#undef MAKE_SET
#endif
