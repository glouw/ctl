//
// Set
//

#ifndef T
#error "Template type T undefined for <set.h>"
#endif

#include <ctl.h>

#define A JOIN(set, T)
#define B JOIN(A, node)
#define Z JOIN(A, it)

typedef struct B
{
    struct B* l;
    struct B* r;
    struct B* p;
    T key;
    int color; // Red = 0, Black = 1
}
B;

typedef struct A
{
    B* root;
    int (*compare)(T*, T*);
    void (*free)(T*);
    T (*copy)(T*);
    size_t size;
}
A;

typedef struct Z
{
    void (*step)(struct Z*);
    B* end;
    B* node;
    T* ref;
    B* next;
    int done;
}
Z;

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
JOIN(Z, step)(Z* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->node = self->next;
        self->ref = &self->node->key;
        self->next = JOIN(B, next)(self->node);
    }
}

static inline Z
JOIN(Z, range)(A* container, B* begin, B* end)
{
    (void) container;
    static Z zero;
    Z self = zero;
    if(begin)
    {
        self.step = JOIN(Z, step);
        self.node = JOIN(B, min)(begin);
        self.ref = &self.node->key;
        self.next = JOIN(B, next)(self.node);
        self.end = end;
    }
    else
        self.done = 1;
    return self;
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

static inline int
JOIN(A, equal)(A* self, A* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    Z a = JOIN(Z, each)(self);
    Z b = JOIN(Z, each)(other);
    while(!a.done && !b.done)
    {
        if(!_equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline void
JOIN(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline A
JOIN(A, init)(int _compare(T*, T*))
{
    static A zero;
    A self = zero;
    self.compare = _compare;
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
JOIN(A, free_node)(A* self, B* node)
{
    if(self->free)
        self->free(&node->key);
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
JOIN(B, init)(T key, int color)
{
    B* self = (B*) malloc(sizeof(B));
    self->key = key;
    self->color = color;
    self->l = self->r = self->p = NULL;
    return self;
}

static inline B*
JOIN(A, lower_bound)(A* self, T key)
{
    B* node = self->root;
    B* result = NULL;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff <= 0)
        {
            result = node;
            node = node->l;
        }
        else
            node = node->r;
    }
    return result;
}

static inline B*
JOIN(A, upper_bound)(A* self, T key)
{
    B* node = self->root;
    B* result = NULL;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff < 0)
        {
            result = node;
            node = node->l;
        }
        else
            node = node->r;
    }
    return result;
}

static inline B*
JOIN(A, find)(A* self, T key)
{
    B* node = self->root;
    while(node)
    {
        int diff = self->compare(&key, &node->key);
        if(diff == 0)
            return node;
        else
        if(diff < 0)
            node = node->l;
        else
            node = node->r;
    }
    return NULL;
}

static inline int
JOIN(A, count)(A* self, T key)
{
    return JOIN(A, find)(self, key) ? 1 : 0;
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
        JOIN(B, verify_property_1)(self->root); // Property 1: Each node is either red or black.
        JOIN(B, verify_property_2)(self->root); // Property 2: The root node is black.
        /* Implicit */                          // Property 3: Leaves are colored black
        JOIN(B, verify_property_4)(self->root); // Property 4: Every red node has two black ndoes.
        JOIN(B, verify_property_5)(self->root); // Property 5: All paths from a node have the same number of black nodes.
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

static inline B*
JOIN(A, insert)(A* self, T key)
{
    B* insert = JOIN(B, init)(key, 0);
    if(self->root)
    {
        B* node = self->root;
        while(1)
        {
            int diff = self->compare(&key, &node->key);
            if(diff == 0)
            {
                JOIN(A, free_node)(self, insert);
                return node;
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
    return insert;
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
        SWAP(T, &node->key, &pred->key);
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
        JOIN(A, erase)(self, self->root->key);
}

static inline void
JOIN(A, free)(A* self)
{
    JOIN(A, clear)(self);
    *self = JOIN(A, init)(self->compare);
}

static inline A
JOIN(A, copy)(A* self)
{
    Z it = JOIN(Z, each)(self);
    A copy =  JOIN(A, init)(self->compare);
    while(!it.done)
    {
        JOIN(A, insert)(&copy, self->copy(&it.node->key));
        it.step(&it);
    }
    return copy;
}

static inline size_t
JOIN(A, remove_if)(A* self, int _match(T*))
{
    size_t erases = 0;
    foreach(A, self, it)
        if(_match(&it.node->key))
        {
            JOIN(A, erase_node)(self, it.node);
            erases += 1;
        }
    return erases;
}

static inline A
JOIN(A, intersection)(A* a, A* b)
{
    A self = JOIN(A, init)(a->compare);
    foreach(A, a, i)
        if(JOIN(A, find)(b, *i.ref))
            JOIN(A, insert)(&self, self.copy(i.ref));
    return self;
}

static inline A
JOIN(A, union)(A* a, A* b)
{
    A self = JOIN(A, init)(a->compare);
    foreach(A, a, i) JOIN(A, insert)(&self, self.copy(i.ref));
    foreach(A, b, i) JOIN(A, insert)(&self, self.copy(i.ref));
    return self;
}

static inline A
JOIN(A, difference)(A* a, A* b)
{
    A self = JOIN(A, copy)(a);
    foreach(A, b, i)
        JOIN(A, erase)(&self, *i.ref);
    return self;
}

static inline A
JOIN(A, symmetric_difference)(A* a, A* b)
{
    A self = JOIN(A, union)(a, b);
    A intersection = JOIN(A, intersection)(a, b);
    foreach(A, &intersection, i)
        JOIN(A, erase)(&self, *i.ref);
    JOIN(A, free)(&intersection);
    return self;
}

#undef T
#undef A
#undef B
#undef Z

#ifdef USE_INTERNAL_VERIFY
#undef USE_INTERNAL_VERIFY
#endif
