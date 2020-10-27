#include <ctl.h>

#include <stdio.h>

#define CTL_A  CTL_TEMP(CTL_T, deq)
#define CTL_B  CTL_IMPL(CTL_A, bucket)
#define CTL_I  CTL_IMPL(CTL_A, it)

#define CTL_DEQ_BUCKET_SIZE (32)

typedef struct CTL_B
{
    struct CTL_B* prev;
    struct CTL_B* next;
    size_t a;
    size_t b;
    CTL_T value[CTL_DEQ_BUCKET_SIZE];
}
CTL_B;

typedef struct
{
    CTL_T (*init_default)(void);
    void (*free)(CTL_T*);
    CTL_T (*copy)(CTL_T*);
    CTL_B* head;
    CTL_B* tail;
    size_t size;
    size_t buckets;
}
CTL_A;

typedef struct CTL_I
{
    void (*step)(struct CTL_I*);
    CTL_T* ref;
    CTL_T* begin;
    CTL_T* node;
    CTL_T* next;
    CTL_T* end;
    CTL_B* bucket;
    CTL_B* bucket_next;
    CTL_B* bucket_end;
    size_t step_size;
    bool done;
}
CTL_I;

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return self->size == 0;
}

static inline CTL_B*
CTL_IMPL(CTL_A, inside)(CTL_A* self, CTL_T* address)
{
    for(CTL_B* bucket = self->head; bucket; bucket = bucket->next)
    {
        CTL_T* begin = &bucket->value[0];
        CTL_T* end = &bucket->value[CTL_DEQ_BUCKET_SIZE - 1];
        if(address >= begin && address <= end)
            return bucket;
    }
    return NULL;
}

static inline CTL_B*
CTL_IMPL(CTL_A, r_inside)(CTL_A* self, CTL_T* address)
{
    for(CTL_B* bucket = self->tail; bucket; bucket = bucket->prev)
    {
        CTL_T* begin = &bucket->value[0];
        CTL_T* end = &bucket->value[CTL_DEQ_BUCKET_SIZE - 1];
        if(address >= begin && address <= end)
            return bucket;
    }
    return NULL;
}

static inline CTL_T*
CTL_IMPL(CTL_A, at)(CTL_A* self, size_t index)
{
    CTL_B* head = self->head;
    CTL_B* tail = self->tail;
    if(head == NULL || tail == NULL)
        return NULL;
    index += head->a;
    size_t bucket_index = index / CTL_DEQ_BUCKET_SIZE;
    size_t cut = index - CTL_DEQ_BUCKET_SIZE * bucket_index;
    if(index < self->size / 2)
    {
        for(size_t i = 0; i < bucket_index; i++)
            head = head->next;
        return &head->value[cut];
    }
    else
    {
        for(size_t i = bucket_index; i < self->buckets - 1; i++)
            tail = tail->prev;
        return &tail->value[cut];
    }
}

static inline CTL_T*
CTL_IMPL(CTL_A, front)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, 0);
}

static inline CTL_T*
CTL_IMPL(CTL_A, back)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, self->size - 1);
}

static inline CTL_T*
CTL_IMPL(CTL_A, begin)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, front)(self);
}

static inline CTL_T*
CTL_IMPL(CTL_A, end)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, back)(self) + 1;
}

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A zero;
    CTL_A self = zero;
#ifdef CTL_P
#undef CTL_P
#else
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
#endif
    return self;
}

static inline void
CTL_IMPL(CTL_B, dangle)(CTL_B* self)
{
    self->prev = self->next = NULL;
}

static inline CTL_B*
CTL_IMPL(CTL_B, init)(size_t cut)
{
    static CTL_B zero;
    CTL_B* self = (CTL_B*) malloc(sizeof(CTL_B));
    *self = zero;
    self->a = self->b = cut;
    CTL_IMPL(CTL_B, dangle)(self);
    return self;
}

static inline CTL_B*
CTL_IMPL(CTL_A, disconnect)(CTL_A* self, CTL_B* bucket)
{
    if(bucket == self->tail) self->tail = self->tail->prev;
    if(bucket == self->head) self->head = self->head->next;
    if(bucket->prev) bucket->prev->next = bucket->next;
    if(bucket->next) bucket->next->prev = bucket->prev;
    CTL_IMPL(CTL_B, dangle)(bucket);
    self->buckets -= 1;
    return bucket;
}

static inline void
CTL_IMPL(CTL_A, connect)(CTL_A* self, CTL_B* position, CTL_B* bucket, bool before)
{
    if(CTL_IMPL(CTL_A, empty)(self))
        self->head = self->tail = bucket;
    else
    if(before)
    {
        bucket->next = position;
        bucket->prev = position->prev;
        if(position->prev)
            position->prev->next = bucket;
        position->prev = bucket;
        if(position == self->head)
            self->head = bucket;
    }
    else
    {
        bucket->prev = position;
        bucket->next = position->next;
        if(position->next)
            position->next->prev = bucket;
        position->next = bucket;
        if(position == self->tail)
            self->tail = bucket;
    }
    self->buckets += 1;
}

static inline void
CTL_IMPL(CTL_A, set)(CTL_A* self, size_t index, CTL_T value)
{
    CTL_T* ref = CTL_IMPL(CTL_A, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
CTL_IMPL(CTL_A, push_back)(CTL_A* self, CTL_T value)
{
    if(CTL_IMPL(CTL_A, empty)(self) || self->tail->b == CTL_DEQ_BUCKET_SIZE)
    {
        CTL_B* bucket = CTL_IMPL(CTL_B, init)(0);
        CTL_IMPL(CTL_A, connect)(self, self->tail, bucket, false);
    }
    self->tail->value[self->tail->b] = value;
    self->tail->b += 1;
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, push_front)(CTL_A* self, CTL_T value)
{
    if(CTL_IMPL(CTL_A, empty)(self) || self->head->a == 0)
    {
        CTL_B* bucket = CTL_IMPL(CTL_B, init)(CTL_DEQ_BUCKET_SIZE);
        CTL_IMPL(CTL_A, connect)(self, self->head, bucket, true);
    }
    self->head->a -= 1;
    self->size += 1;
    self->head->value[self->head->a] = value;
}

static inline void
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    static CTL_T zero;
    self->tail->b -= 1;
    self->size -= 1;
    CTL_IMPL(CTL_A, set)(self, self->size, zero);
    if(self->tail->b == self->tail->a)
        free(CTL_IMPL(CTL_A, disconnect)(self, self->tail));
}

static inline void
CTL_IMPL(CTL_A, pop_front)(CTL_A* self)
{
    static CTL_T zero;
    CTL_IMPL(CTL_A, set)(self, 0, zero);
    self->head->a += 1;
    self->size -= 1;
    if(self->head->a == self->head->b)
        free(CTL_IMPL(CTL_A, disconnect)(self, self->head));
}

static inline void
CTL_IMPL(CTL_A, resize)(CTL_A* self, size_t size)
{
    static CTL_T zero;
    if(size != self->size)
        while(size != self->size)
            (size < self->size)
                ? CTL_IMPL(CTL_A, pop_back)(self)
                : CTL_IMPL(CTL_A, push_back)(self, self->init_default ? self->init_default() : zero);
}

static inline void
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    while(self->size > 0)
        CTL_IMPL(CTL_A, pop_back)(self);
}

static inline void
CTL_IMPL(CTL_I, step)(CTL_I* self)
{
    // XXX. NEEDS PAST RANGE CHECK.
    self->ref += self->step_size;
    if(self->ref == self->end)
        self->done = true;
    else
    {
        CTL_T* last = &self->bucket->value[CTL_DEQ_BUCKET_SIZE - 1];
        if(self->ref > last)
        {
            self->bucket = self->bucket_next;
            self->bucket_next = self->bucket_next->next;
            self->ref = &self->bucket->value[0];
        }
    }
}

static inline CTL_I
CTL_IMPL(CTL_I, by)(CTL_A* container, CTL_T* begin, CTL_T* end, size_t step_size)
{
    static CTL_I zero;
    CTL_I self = zero;
    if(begin == NULL || end == NULL)
        self.done = true;
    else
    {
        self.step = CTL_IMPL(CTL_I, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + step_size;
        self.node = begin;
        self.ref = begin;
        self.bucket = CTL_IMPL(CTL_A, inside)(container, begin);
        self.bucket_end = CTL_IMPL(CTL_A, r_inside)(container, end);
        self.bucket_next = self.bucket->next;
        self.step_size = step_size;
    }
    return self;
}

static inline CTL_I
CTL_IMPL(CTL_I, each)(CTL_A* a)
{
    return CTL_IMPL(CTL_I, by)(a, CTL_IMPL(CTL_A, begin)(a), CTL_IMPL(CTL_A, end)(a), 1);
}


#undef CTL_DEQ_BUCKET_SIZE

#undef CTL_T
#undef CTL_A
#undef CTL_B
