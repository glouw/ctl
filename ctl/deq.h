#include <ctl.h>

#define CTL_A  CTL_TEMP(CTL_T, deq)
#define CTL_B  CTL_IMPL(CTL_A, node)

#ifdef CTL_U
#define CTL_DEQ_BUF_SIZE CTL_U
#else
#define CTL_DEQ_BUF_SIZE (512)
#endif

typedef struct CTL_B
{
    struct CTL_B* prev;
    struct CTL_B* next;
    size_t size;
    CTL_T value[CTL_DEQ_BUF_SIZE];
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
}
CTL_A;

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A zero;
    CTL_A self = zero;
#ifndef CTL_POD
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
#else
#undef CTL_POD
#endif
    return self;
}

static inline CTL_B*
CTL_IMPL(CTL_B, init)(void)
{
    CTL_B* self = (CTL_B*) malloc(sizeof(CTL_B));
    self->size = 0;
    self->prev = self->next = NULL;
    return self;
}

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return self->size == 0;
}

#include <shared/link.h>

#undef CTL_T
#undef CTL_U
#undef CTL_A
#undef CTL_B
