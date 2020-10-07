#include <_ctl_open.h>

#include <string.h>

#define vec_char str
#define T char
#include <vec.h>

static inline void
_str_stamp(str* self)
{
    self->value[self->size] = '\0';
}

static inline str
str_create(const char* cstr)
{
    str self = str_construct(NULL, NULL, NULL);
    str_reserve(&self, strlen(cstr) + 1);
    for(const char* s = cstr; *s; s++)
        str_push_back(&self, *s);
    _str_stamp(&self);
    return self;
}

static inline void
str_append(str* self, const char* cstr)
{
    const size_t len = strlen(cstr);
    str_reserve(self, self->size + len + 1);
    for(size_t i = 0; i < len; i++)
        str_push_back(self, cstr[i]);
    _str_stamp(self);
}

static inline void
str_replace(str* self, size_t index, size_t size, const char* cstr)
{
    const size_t len = strlen(cstr);
    str_reserve(self, self->size + len + 1);
    for(size_t i = 0; i < size; i++)
        str_erase(self, index);
    size_t i = len;
    while(i != 0)
    {
        i -= 1;
        str_insert(self, index, cstr[i]);
    }
    _str_stamp(self);
}

static inline size_t
str_find(str* self, const char* str)
{
    const char* found = strstr(self->value, str);
    if(found)
        return found - self->value;
    return -1;
}

static inline size_t
str_rfind(str* self, const char* str)
{
    size_t i = self->size;
    while(i != 0)
    {
        i -= 1;
        const char* found = strstr(&self->value[i], str);
        if(found)
            return found - self->value;
    }
    return -1;
}

static inline size_t
str_find_first_of(str* self, const char c)
{
    const char* found = strchr(self->value, c);
    if(found)
        return found - self->value;
    return -1;
}

static inline size_t
str_find_last_of(str* self, const char c)
{
    const char* found = strrchr(self->value, c);
    if(found)
        return found - self->value;
    return -1;
}

static inline size_t
str_find_first_not_of(str* self, const char* other)
{
    (void) self;
    (void) other;
    return -1; // TODO
}

static inline size_t
str_find_last_not_of(str* self, const char* other)
{
    (void) self;
    (void) other;
    return -1; // TODO
}

static inline str
str_substr(str* self, size_t index, size_t len)
{
    str other = str_construct(NULL, NULL, NULL);
    str_reserve(&other, len + 1);
    for(size_t i = index; i < len; i++)
        str_push_back(&other, self->value[i]);
    _str_stamp(&other);
    return other;
}

static inline size_t
str_compare(const char* self, const char* other, size_t index, size_t size)
{
    return strncmp(self + index, other + index, size);
}

#include <_ctl_close.h>
