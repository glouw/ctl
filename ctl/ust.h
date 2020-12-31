#ifndef T
#error "Template type T undefined for <ust.h>"
#endif

#include <ctl.h>

#define A JOIN(ust, T)
#define B JOIN(A, node)
#define I JOIN(A, it)

static inline int
JOIN(A, __is_prime)(size_t n)
{
    if(n == 0 || n == 1)
        return 0;
    for(size_t i = 2; i * i <= n; i++)
        if(n % i == 0)
            return 0;
    return 1;
}

static inline int
JOIN(A, __next_prime)(size_t n)
{
    while(n)
    {
        if(JOIN(A, __is_prime)(n))
            break;
        n += 1;
    }
    return n;
}

#undef T
#undef A
#undef B
#undef I

//typedef struct node
//{
//    T value;
//    struct node* next;
//}
//node;
//
//node*
//node_init(T value)
//{
//    node* n = malloc(sizeof(*n));
//    n->value = value;
//    n->next = NULL;
//    return n;
//}
//
//node*
//node_push(node* bucket, node* n)
//{
//    n->next = bucket;
//    return n;
//}
//
//typedef struct ust
//{
//    void (*free)(T*);
//    T (*copy)(T*);
//    size_t (*hash)(T*);
//    int (*equal)(T*, T*);
//    node** bucket;
//    size_t size;
//    size_t bucket_count;
//}
//ust;
//
//int
//ust_empty(ust* self)
//{
//    return self->size == 0;
//}
//
//static inline node**
//ust_bucket(ust* self, T value)
//{
//    return &self->bucket[self->hash(&value) % self->bucket_count];
//}
//
//node*
//ust_begin(ust* self)
//{
//    return self->bucket[0];
//}
//
//node*
//ust_end(ust* self)
//{
//    return NULL;
//}
//
//void
//ust_free_node(ust* self, node* n)
//{
//    if(self->free)
//        self->free(&n->value);
//    free(n);
//}
//
//static float
//ust_load_factor(ust* self)
//{
//    return (float) self->size / (float) self->bucket_count;
//}
//
//static void
//ust_reserve(ust* self, size_t desired_count)
//{
//    self->bucket_count = __next_prime(desired_count);
//    self->bucket = calloc(self->bucket_count, sizeof(*self->bucket));
//}
//
//static inline ust
//ust_init(size_t desired_count, size_t (*_hash)(T*), int (*_equal)(T*, T*))
//{
//    static ust zero;
//    ust self = zero;
//    self.hash = _hash;
//    self.equal = _equal;
//    ust_reserve(&self, desired_count);
//    return self;
//}
//
//static inline void
//ust_insert(ust* self, T value)
//{
//    node** bucket = ust_bucket(self, value);
//    for(node* n = *bucket; n; n = n->next)
//        if(self->equal(&value, &n->value))
//        {
//            if(self->free)
//                self->free(&value);
//            return;
//        }
//    *bucket = node_push(*bucket, node_init(value));
//}
//
//static void
//ust_rehash(ust* self, size_t desired_count)
//{
//    ust rehashed = ust_init(desired_count, self->hash, self->equal);
//    for(size_t i = 0; i < self->bucket_count; i++)
//        for(node* n = self->bucket[i]; n; n = n->next)
//        {
//            node** bucket = ust_bucket(&rehashed, n->value);
//            *bucket = node_push(*bucket, n);
//        }
//    free(self->bucket);
//    *self = rehashed;
//}
//
//static inline void
//ust_free(ust* self)
//{
//    for(size_t i = 0; i < self->bucket_count; i++)
//    {
//        node* n = self->bucket[i];
//        while(n)
//        {
//            node* next = n->next;
//            ust_free_node(self, n);
//            n = next;
//        }
//    }
//    free(self->bucket);
//}
//
//static inline size_t
//ust_bucket_size(node* self)
//{
//    size_t size = 0;
//    for(node* n = self; n; n = n->next)
//        size += 1;
//    return size;
//}
//
//static inline node*
//ust_find(ust* self, T value)
//{
//    node** bucket = ust_bucket(self, value);
//    for(node* n = *bucket; n; n = n->next)
//        if(self->equal(&value, &n->value))
//            return n;
//    return NULL;
//}
//
//static inline size_t
//ust_count(ust* self, T value)
//{
//    size_t count = 0;
//    for(size_t i = 0; i < self->bucket_count; i++)
//    {
//        node* n = self->bucket[i];
//        while(n)
//        {
//            node* next = n->next;
//            if(self->equal(&n->value, &value))
//                count += 1;
//            n = next;
//        }
//    }
//    return count;
//}
//
//static inline void
//ust_remove(ust* self, T value)
//{
//    node** bucket = ust_bucket(self, value);
//    node* prev = NULL;
//    for(node* n = *bucket; n; n = n->next)
//    {
//        if(self->equal(&value, &n->value))
//        {
//            node* next = n->next;
//            ust_free_node(self, n);
//            if(prev)
//                prev->next = next;
//        }
//        prev = n;
//    }
//}
