#ifndef T
#error "Template type T undefined for <map>"
#endif

#ifndef U
#error "Template type U undefined for <map>"
#endif

#include <ctl.h>

#define A TEMP(U, TEMP(T, map))
#define B IMPL(A, node)

typedef struct B
{
    struct B* l;
    struct B* r;
    struct B* p;
    T first;
    U second;
    int color; // RED 0, BLK 1
}
B;

typedef struct
{
    B* root;
    int (*compare)(T*, T*);
    void (*free)(T*, U*);
    T (*copy_first)(T*);
    U (*copy_second)(U*);
    size_t size;
}
A;

static inline int
IMPL(A, empty)(A* self)
{
    return self->size == 0;
}

static inline T
IMPL(A, implicit_copy_first)(T* self)
{
    return *self;
}

static inline U
IMPL(A, implicit_copy_second)(U* self)
{
    return *self;
}

static inline A
IMPL(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
    self.copy_first = IMPL(A, implicit_copy_first);
    self.copy_second = IMPL(A, implicit_copy_second);
#else
    self.free = IMPL(T, free);
    self.copy_first = IMPL(T, copy_first);
    self.copy_second = IMPL(T, copy_second);
#endif
    return self;
}

static inline A
IMPL(A, create)(int compare(T*, T*))
{
    A self = IMPL(A, init)();
    self.compare = compare;
    return self;
}

static inline A
IMPL(A, init_default)(void)
{
    return IMPL(A, init)();
}

static inline int
IMPL(B, color)(B* self)
{
    return self ? self->color : 1;
}

static inline int
IMPL(B, is_blk)(B* self)
{
    return IMPL(B, color)(self) == 1;
}

static inline int
IMPL(B, is_red)(B* self)
{
    return IMPL(B, color)(self) == 0;
}

static inline B*
IMPL(B, maximum)(B* self)
{
    while(self->r)
        self = self->r;
    return self;
}

static inline B*
IMPL(B, grandfather)(B* self)
{
    return self->p->p;
}

static inline B*
IMPL(B, sibling)(B* self)
{
    if(self == self->p->l)
        return self->p->r;
    else
        return self->p->l;
}

static inline B*
IMPL(B, uncle)(B* self)
{
    return IMPL(B, sibling)(self->p);
}

static inline B*
IMPL(B, init)(T key, U value, int color)
{
    B* self = (B*) malloc(sizeof(*self));
    self->first = key;
    self->second = value;
    self->color = color;
    self->l = self->r = self->p = NULL;
    return self;
}

static inline B*
IMPL(A, find)(A* self, T key)
{
    B* node = self->root;
    while(node)
    {
        int diff = self->compare(&key, &node->first);
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

static inline void
IMPL(B, replace)(A* self, B* a, B* b)
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

    static inline void
    IMPL(B, verify_property_1)(B* self)
    {
        assert(IMPL(B, is_red)(self) || IMPL(B, is_blk)(self));
        if(self)
        {
            IMPL(B, verify_property_1)(self->l);
            IMPL(B, verify_property_1)(self->r);
        }
    }

    static inline void
    IMPL(B, verify_property_2)(B* self)
    {
        assert(IMPL(B, is_blk)(self));
    }

    static inline void
    IMPL(B, verify_property_4)(B* self)
    {
        if(IMPL(B, is_red)(self))
        {
            assert(IMPL(B, is_blk)(self->l));
            assert(IMPL(B, is_blk)(self->r));
            assert(IMPL(B, is_blk)(self->p));
        }
        if(self)
        {
            IMPL(B, verify_property_4)(self->l);
            IMPL(B, verify_property_4)(self->r);
        }
    }

    static inline void
    IMPL(B, count_blk)(B* self, int nodes, int* in_path)
    {
        if(IMPL(B, is_blk)(self))
            nodes += 1;
        if(self)
        {
            IMPL(B, count_blk)(self->l, nodes, in_path);
            IMPL(B, count_blk)(self->r, nodes, in_path);
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
    IMPL(B, verify_property_5)(B* self)
    {
        int in_path = -1;
        IMPL(B, count_blk)(self, 0, &in_path);
    }

    static inline void
    IMPL(A, verify)(A* self)
    {
        IMPL(B, verify_property_1)(self->root); // EACH NODE IS EITHER RED OR BLACK.
        IMPL(B, verify_property_2)(self->root); // THE ROOT NODE IS BLACK.
        IMPL(B, verify_property_4)(self->root); // EVERY RED NODE HAS TWO BLACK NDOES.
        IMPL(B, verify_property_5)(self->root); // ALL PATHS FROM A NODE HAVE THE SAME NUMBER OF BLACK NODES.
        /* PROPERTY 3: LEAVES ARE COLORED BLACK; IMPLICITLY GUAREENTEED VIA NODE_COLOR. */
    }

#endif

static inline void
IMPL(A, rotate_l)(A* self, B* node)
{
    B* r = node->r;
    IMPL(B, replace)(self, node, r);
    node->r = r->l;
    if(r->l)
        r->l->p = node;
    r->l = node;
    node->p = r;
}

static inline void
IMPL(A, rotate_r)(A* self, B* node)
{
    B* l = node->l;
    IMPL(B, replace)(self, node, l);
    node->l = l->r;
    if(l->r)
        l->r->p = node;
    l->r = node;
    node->p = l;
}

static inline void
IMPL(A, insert_1)(A*, B*),
IMPL(A, insert_2)(A*, B*),
IMPL(A, insert_3)(A*, B*),
IMPL(A, insert_4)(A*, B*),
IMPL(A, insert_5)(A*, B*);

static inline void
IMPL(A, insert)(A* self, T key, U value)
{
    B* insert = IMPL(B, init)(key, value, 0);
    if(self->root)
    {
        B* node = self->root;
        while(1)
        {
            int diff = self->compare(&key, &node->first);
            if(diff == 0)
            {
                free(insert);
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
    IMPL(A, insert_1)(self, insert);
    self->size += 1;
#ifdef USE_INTERNAL_VERIFY
    IMPL(A, verify)(self);
#endif
}

static inline void
IMPL(A, insert_1)(A* self, B* node)
{
    if(node->p)
        IMPL(A, insert_2)(self, node);
    else
        node->color = 1;
}

static inline void
IMPL(A, insert_2)(A* self, B* node)
{
    if(IMPL(B, is_blk)(node->p))
        return;
    else
       IMPL(A, insert_3)(self, node);
}

static inline void
IMPL(A, insert_3)(A* self, B* node)
{
    if(IMPL(B, is_red)(IMPL(B, uncle)(node)))
    {
        node->p->color = 1;
        IMPL(B, uncle)(node)->color = 1;
        IMPL(B, grandfather)(node)->color = 0;
        IMPL(A, insert_1)(self, IMPL(B, grandfather)(node));
    }
    else
        IMPL(A, insert_4)(self, node);
}

static inline void
IMPL(A, insert_4)(A* self, B* node)
{
    if(node == node->p->r && node->p == IMPL(B, grandfather)(node)->l)
    {
        IMPL(A, rotate_l)(self, node->p);
        node = node->l;
    }
    else
    if(node == node->p->l && node->p == IMPL(B, grandfather)(node)->r)
    {
        IMPL(A, rotate_r)(self, node->p);
        node = node->r;
    }
    IMPL(A, insert_5)(self, node);
}

static inline void
IMPL(A, insert_5)(A* self, B* node)
{
    node->p->color = 1;
    IMPL(B, grandfather)(node)->color = 0;
    if(node == node->p->l && node->p == IMPL(B, grandfather)(node)->l)
        IMPL(A, rotate_r)(self, IMPL(B, grandfather)(node));
    else
        IMPL(A, rotate_l)(self, IMPL(B, grandfather)(node));
}

static inline void
IMPL(A, erase_1)(A*, B*),
IMPL(A, erase_2)(A*, B*),
IMPL(A, erase_3)(A*, B*),
IMPL(A, erase_4)(A*, B*),
IMPL(A, erase_5)(A*, B*),
IMPL(A, erase_6)(A*, B*);

static inline void
IMPL(A, erase)(A* self, T key)
{
    B* node = IMPL(A, find)(self, key);
    if(node)
    {
        if(node->l && node->r)
        {
            B* pred = IMPL(B, maximum)(node->l);
            node->first = pred->first;
            node = pred;
        }
        B* child = node->r ? node->r : node->l;
        if(IMPL(B, is_blk)(node))
        {
            node->color = IMPL(B, color)(child);
            IMPL(A, erase_1)(self, node);
        }
        IMPL(B, replace)(self, node, child);
        if(node->p == NULL && child)
            child->color = 1;
        if(self->free)
            self->free(&node->first, &node->second);
        free(node);
        self->size -= 1;
#ifdef USE_INTERNAL_VERIFY
        IMPL(A, verify)(self);
#endif
    }
}

static inline void
IMPL(A, erase_1)(A* self, B* node)
{
    if(node->p)
        IMPL(A, erase_2)(self, node);
}

static inline void
IMPL(A, erase_2)(A* self, B* node)
{
    if(IMPL(B, is_red)(IMPL(B, sibling)(node)))
    {
        node->p->color = 0;
        IMPL(B, sibling)(node)->color = 1;
        if(node == node->p->l)
            IMPL(A, rotate_l)(self, node->p);
        else
            IMPL(A, rotate_r)(self, node->p);
    }
    IMPL(A, erase_3)(self, node);
}

static inline void
IMPL(A, erase_3)(A* self, B* node)
{
    if(IMPL(B, is_blk)(node->p)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node))
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->l)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->r))
    {
        IMPL(B, sibling)(node)->color = 0;
        IMPL(A, erase_1)(self, node->p);
    }
    else
        IMPL(A, erase_4)(self, node);
}

static inline void
IMPL(A, erase_4)(A* self, B* node)
{
    if(IMPL(B, is_red)(node->p)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node))
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->l)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->r))
    {
        IMPL(B, sibling)(node)->color = 0;
        node->p->color = 1;
    }
    else
        IMPL(A, erase_5)(self, node);
}

static inline void
IMPL(A, erase_5)(A* self, B* node)
{
    if(node == node->p->l
    && IMPL(B, is_blk)(IMPL(B, sibling)(node))
    && IMPL(B, is_red)(IMPL(B, sibling)(node)->l)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->r))
    {
        IMPL(B, sibling)(node)->color = 0;
        IMPL(B, sibling)(node)->l->color = 1;
        IMPL(A, rotate_r)(self, IMPL(B, sibling)(node));
    }
    else
    if(node == node->p->r
    && IMPL(B, is_blk)(IMPL(B, sibling)(node))
    && IMPL(B, is_red)(IMPL(B, sibling)(node)->r)
    && IMPL(B, is_blk)(IMPL(B, sibling)(node)->l))
    {
        IMPL(B, sibling)(node)->color = 0;
        IMPL(B, sibling)(node)->r->color = 1;
        IMPL(A, rotate_l)(self, IMPL(B, sibling)(node));
    }
    IMPL(A, erase_6)(self, node);
}

static inline void
IMPL(A, erase_6)(A* self, B* node)
{
    IMPL(B, sibling)(node)->color = IMPL(B, color)(node->p);
    node->p->color = 1;
    if(node == node->p->l)
    {
        IMPL(B, sibling)(node)->r->color = 1;
        IMPL(A, rotate_l)(self, node->p);
    }
    else
    {
        IMPL(B, sibling)(node)->l->color = 1;
        IMPL(A, rotate_r)(self, node->p);
    }
}
static inline void
IMPL(A, clear)(A* self)
{
    while(!IMPL(A, empty)(self))
        IMPL(A, erase)(self, self->root->first);
}

static inline void
IMPL(A, free)(A* self)
{
    IMPL(A, clear)(self);
    *self = IMPL(A, init)();
}

#undef T
#undef A
#undef B
#undef C

#ifdef USE_INTERNAL_VERIFY
#undef USE_INTERNAL_VERIFY
#endif
