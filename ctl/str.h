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
        size_t size = self->size + other->size;
        if(size > self->capacity)
        {
            size_t capacity = 2 * self->capacity;
            if(size > capacity)
                capacity = size;
            str_reserve(self, capacity);
        }
        for(size_t i = 0; i < other->size; i++)
            str_push_back(self, other->value[i]);
    }
}

static inline char*
str_c_str(str* self)
{
    char* c_str = (char*) malloc(self->size + 1);
    for(size_t i = 0; i < self->size; i++)
        c_str[i] = self->value[i];
    c_str[self->size] = '\0';
    return c_str;
}
