#pragma once

#include <string.h>

#define vec_char str
#define T char
#include <vec.h>
#undef vec_char

static inline str
str_create(const char* cstr)
{
    str self = str_construct(NULL, NULL, NULL);
    size_t len = strlen(cstr);
    size_t min = 15;
    str_reserve(&self, len < min ? min : len);
    for(const char* s = cstr; *s; s++)
        str_push_back(&self, *s);
    return self;
}

static inline void
str_append(str* self, str* other)
{
    if(other->size > 0)
    {
        size_t start = self->size;
        str_resize(self, self->size + other->size);
        for(size_t i = 0; i < other->size; i++)
            self->value[start + i] = other->value[i];
    }
}

static inline void
str_insert_str(str* self, size_t index, str* other)
{
    if(other->size > 0)
    {
        size_t temp = self->size;
        str_resize(self, self->size + other->size);
        self->size = temp;
        size_t where = other->size;
        while(where != 0)
        {
            where -= 1;
            str_insert(self, index, other->value[where]);
        }
    }
}

static inline void
str_replace(str* self, size_t index, size_t size, str* other)
{
    size_t end = index + size;
    if(end >= self->size)
        end = self->size;
    for(size_t i = index; i < end; i++)
        str_erase(self, index);
    str_insert_str(self, index, other);
}

static inline const char*
str_c_str(str* self)
{
    return str_data(self);
}
