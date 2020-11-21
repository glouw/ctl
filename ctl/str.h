#ifndef __STR__H__
#define __STR__H__

#ifdef T
#error "Template type T defined for <str>"
#endif

#define vec_char str
#define P
#define T char
#define str_init_default str___INIT_DEFAULT
#include <vec.h>
#undef str_init_default
#undef vec_char

#include <stdint.h>
#include <string.h>

static inline str
str_create(const char* c_str)
{
    str self = str_init();
    size_t len = strlen(c_str);
    size_t min = 15;
    str_reserve(&self, len < min ? min : len);
    for(const char* s = c_str; *s; s++)
        str_push_back(&self, *s);
    return self;
}

static inline str
str_init_default(void)
{
    return str_create("");
}

static inline void
str_append(str* self, const char* s)
{
    size_t start = self->size;
    size_t len = strlen(s);
    str_resize(self, self->size + len);
    for(size_t i = 0; i < len; i++)
        self->value[start + i] = s[i];
}

static inline void
str_insert_str(str* self, size_t index, const char* s)
{
    size_t start = self->size;
    size_t len = strlen(s);
    str_resize(self, self->size + len);
    self->size = start;
    while(len != 0)
    {
        len -= 1;
        str_insert(self, str_begin(self) + index, s[len]);
    }
}

static inline void
str_replace(str* self, size_t index, size_t size, const char* s)
{
    size_t end = index + size;
    if(end >= self->size)
        end = self->size;
    for(size_t i = index; i < end; i++)
        str_erase(self, str_begin(self) + index);
    str_insert_str(self, index, s);
}

static inline char*
str_c_str(str* self)
{
    return str_data(self);
}

static inline size_t
str_find(str* self, const char* s)
{
    char* c_str = str_c_str(self);
    char* found = strstr(c_str, s);
    if(found)
        return found - c_str;
    return SIZE_MAX;
}

static inline size_t
str_rfind(str* self, const char* s)
{
    char* c_str = str_c_str(self);
    for(size_t i = self->size; i != SIZE_MAX; i--)
    {
        char* found = strstr(&c_str[i], s);
        if(found)
            return found - c_str;
    }
    return SIZE_MAX;
}

static inline size_t
str_find_first_of(str* self, const char* s)
{
    for(size_t i = 0; i < self->size; i++)
    for(const char* p = s; *p; p++)
        if(self->value[i] == *p)
            return i;
    return SIZE_MAX;
}

static inline size_t
str_find_last_of(str* self, const char* s)
{
    for(size_t i = self->size; i != SIZE_MAX; i--)
    for(const char* p = s; *p; p++)
        if(self->value[i] == *p)
            return i;
    return SIZE_MAX;
}

static inline size_t
str_find_first_not_of(str* self, const char* s)
{
    for(size_t i = 0; i < self->size; i++)
    {
        size_t count = 0;
        for(const char* p = s; *p; p++)
            if(self->value[i] == *p)
                count += 1;
        if(count == 0)
            return i;
    }
    return SIZE_MAX;
}

static inline size_t
str_find_last_not_of(str* self, const char* s)
{
    for(size_t i = self->size - 1; i != SIZE_MAX; i--)
    {
        size_t count = 0;
        for(const char* p = s; *p; p++)
            if(self->value[i] == *p)
                count += 1;
        if(count == 0)
            return i;
    }
    return SIZE_MAX;
}

static inline str
str_substr(str* self, size_t index, size_t size)
{
    str substr = str_create("");
    str_resize(&substr, size);
    for(size_t i = 0; i < size; i++)
        substr.value[i] = self->value[index + i];
    return substr;
}

static inline int
str_compare(str* self, const char* s)
{
    return strcmp(str_c_str(self), s);
}

#endif
