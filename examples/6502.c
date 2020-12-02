//
// -- SIMPLE 6502 COMPILER --
//

#include <stdarg.h>
#include <stdio.h>
#include <str.h>

#define P
#define T char
#include <deq.h>

size_t line = 0;

void
quit(char* message, ...)
{
    va_list args;
    va_start(args, message);
    printf("error: line %lu: ", line);
    vprintf(message, args);
    putchar('\n');
    va_end(args);
    exit(1);
}

typedef enum
{
    LOCAL, GLOBAL, ARRAY, STRUCTURE, FUNCTION, RESERVED
}
fam_t;

typedef struct
{
    str name; // NEEDS TYPE.
    size_t offset;
}
offsetof_t;

offsetof_t
offsetof_t_copy(offsetof_t* self)
{
    return (offsetof_t)
    {
        str_copy(&self->name),
        self->offset
    };
}

void
offsetof_t_free(offsetof_t* self)
{
    str_free(&self->name);
}

int
offsetof_t_key_compare(offsetof_t* a, offsetof_t* b)
{
    return str_key_compare(&a->name, &b->name);
}

#define T offsetof_t
#include <set.h>

typedef struct
{
    str type;
    size_t size;
    set_offsetof_t offset;
}
sizeof_t; // NEEDS SET.

typedef struct
{
    str name;
    str type;
    fam_t fam;
}
id_t;

id_t
id_t_reserve(char* name)
{
    return (id_t)
    {
        str_init(name),
        str_init(""),
        RESERVED
    };
}

id_t
id_t_copy(id_t* self)
{
    return (id_t)
    {
        str_copy(&self->type),
        str_copy(&self->name),
        self->fam
    };
}

void
id_t_free(id_t* self)
{
    str_free(&self->name);
}

int
id_t_key_compare(id_t* a, id_t* b)
{
    return str_key_compare(&a->name, &b->name);
}

#define T id_t
#include <set.h>

deq_char
extract(str* code)
{
    deq_char feed = deq_char_init();
    foreach(str, code, it,
        deq_char_push_back(&feed, *it.ref);
    )
    return feed;
}

int
is_space(char c)
{
    return c == ' ' || c == '\n';
}

int
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int
is_alpha(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z');
}

int
is_alnum(char c)
{
    return is_alpha(c) || is_digit(c);
}

int
is_ident(char c)
{
    return is_alnum(c) || c == '_';
}

char
peek(deq_char* feed)
{
    return *deq_char_front(feed);
}

void
pop(deq_char* feed)
{
    if(peek(feed) == '\n')
        line += 1;
    deq_char_pop_front(feed);
}

void
send_back(deq_char* feed, str* s)
{
    while(s->size > 0)
    {
        deq_char_push_front(feed, *str_back(s));
        str_pop_back(s);
    }
    str_free(s);
}

char
next(deq_char* feed)
{
    while(is_space(peek(feed)))
        pop(feed);
    return peek(feed);
}

void
match(deq_char* feed, char c)
{
    if(next(feed) != c)
        quit("expected %c", c);
    pop(feed);
}

str
identifier(deq_char* feed)
{
    if(!is_ident(next(feed)))
        quit("identfier must start with letter or underscore");
    str s = str_init("");
    while(is_ident(peek(feed)))
    {
        str_push_back(&s, peek(feed));
        pop(feed);
    }
    return s;
}

void
structure(deq_char* feed)
{
    str temp = identifier(feed);
    if(!str_match(&temp, "struct"))
        quit("missing keyword 'struct'");
    str type = identifier(feed);
    match(feed, '{');
    while(next(feed) != '}')
    {
        str type = identifier(feed); // INSERT TO SIZEOF SET
        str name = identifier(feed);
        match(feed, ';');
    }
    match(feed, '}');
    str_free(&temp);
}

set_id_t
reserve(void)
{
    set_id_t ids = set_id_t_init(id_t_key_compare);
    char* reserved[] = {
        "struct",
        "u8",
        "void"
    };
    for(size_t i = 0; i < len(reserved); i++)
    {
        id_t id = id_t_reserve(reserved[i]);
        set_id_t_insert(&ids, id);
    }
    return ids;
}

void
program(deq_char* feed)
{
    set_id_t ids = reserve();
    str s = identifier(feed);
    if(str_match(&s, "struct"))
    {
        send_back(feed, &s);
        structure(feed);
    }
    set_id_t_free(&ids);
    str_free(&s);
}

int
main(void)
{
    str code = str_init(
        "                 \n"
        "   struct person \n"
        "   {             \n"
        "       u8 a;     \n"
        "       u8 b;     \n"
        "   }             \n"
    );
    deq_char feed = extract(&code);
    program(&feed);
    deq_char_free(&feed);
    str_free(&code);
}
