//
// -- SIMPLE 6502 COMPILER --
//
// For implementation details:
//     https://compilers.iecc.com/crenshaw/

#include <stdarg.h>
#include <stdio.h>
#include <str.h>

#define P
#define T char
#include <deq.h>

#define T str
#include <vec.h>

#define POINTER_SIZE (sizeof(uint16_t))

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

#define BINDS X(TYPE) X(VARIABLE) X(REFERENCE) X(INTERNAL) X(FUNCTION) X(STRUCTURE) X(RETURN)

typedef enum
{
#define X(name) name,
BINDS
#undef X
}
bind_t;

char*
printable(bind_t bind)
{
    return (char* [])
    {
#define X(name) #name,
BINDS
#undef X
    }
    [bind];
}

typedef struct
{
    bind_t bind;
    str type;
    str name; // KEY.
    size_t size;
    size_t offset;
}
var_t;

void
tab(size_t n)
{
    while(n--)
        printf("\t");
}

void
var_t_print(var_t* var, size_t tabs)
{
    tab(tabs); printf("name : %s\n", var->name.value);
    tab(tabs); printf("\tbind : %s\n", printable(var->bind));
    tab(tabs); printf("\ttype : %s\n", var->type.value);
    tab(tabs); printf("\tsize : %lu\n", var->size);
    tab(tabs); printf("\toffs : %lu\n", var->offset);
}

var_t
var_t_init(str type, str name, bind_t bind, size_t size, size_t offset)
{
    return (var_t)
    {
        .type = type,
        .name = name,
        .bind = bind,
        .size = size,
        .offset = offset,
    };
}

var_t
var_t_copy(var_t* self)
{
    return (var_t)
    {
        .type = str_copy(&self->name),
        .bind = self->bind,
        .name = str_copy(&self->type),
        .size = self->size,
        .offset = self->offset,
    };
}

void
var_t_free(var_t* self)
{
    str_free(&self->type);
    str_free(&self->name);
    *self = (var_t) { 0 };
}

int
var_t_key_compare(var_t* a, var_t* b)
{
    return str_key_compare(&a->name, &b->name);
}

var_t
var_t_key(str name)
{
    var_t key;
    key.name = name;
    return key;
}

#define T var_t
#include <set.h>

void
set_var_t_put(set_var_t* self, var_t var)
{
    if(set_var_t_contains(self, var))
        quit("'%s' already defined", var.name.value);
    set_var_t_insert(self, var);
}

var_t*
set_var_t_get(set_var_t* self, str name)
{
    set_var_t_node* node = set_var_t_find(self, var_t_key(name));
    if(!node)
        quit("'%s' not defined", name.value);
    return &node->key;
}

typedef struct
{
    var_t var;
    set_var_t member; // FOR STRUCTURES.
}
tok_t;

void
tok_t_print(tok_t* tok)
{
    var_t_print(&tok->var, 0);
    foreach(set_var_t, &tok->member, it,
        var_t_print(it.ref, 2);
    )
}

tok_t
tok_t_init(str type, str name, bind_t bind, size_t size, size_t offset)
{
    return (tok_t)
    {
        .var = var_t_init(type, name, bind, size, offset),
        .member = set_var_t_init(var_t_key_compare)
    };
}

tok_t
tok_t_copy(tok_t* self)
{
    return (tok_t)
    {
        .var = var_t_copy(&self->var),
        .member = set_var_t_copy(&self->member)
    };
}

void
tok_t_free(tok_t* self)
{
    var_t_free(&self->var);
    set_var_t_free(&self->member);
    *self = (tok_t) { 0 };
}

int
tok_t_key_compare(tok_t* a, tok_t* b)
{
    return var_t_key_compare(&a->var, &b->var);
}

tok_t
tok_t_key(str name)
{
    tok_t key;
    key.var = var_t_key(name);
    return key;
}

#define T tok_t
#include <set.h>

void
set_tok_t_put(set_tok_t* self, tok_t tok)
{
    if(set_tok_t_contains(self, tok))
        quit("'%s' already defined", tok.var.name.value);
    set_tok_t_insert(self, tok);
}

tok_t*
set_tok_t_get(set_tok_t* self, str name)
{
    set_tok_t_node* node = set_tok_t_find(self, tok_t_key(name));
    if(!node)
        quit("'%s' not defined", name.value);
    return &node->key;
}

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
is_operator(char c)
{
    return c == '+' || c == '-' || c == '.';
}

int
is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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
advance(deq_char* feed)
{
    while(!deq_char_empty(feed) && is_space(peek(feed)))
        pop(feed);
}

char
next(deq_char* feed)
{
    advance(feed);
    return peek(feed);
}

void
match(deq_char* feed, char c)
{
    if(next(feed) != c)
        quit("expected %c", c);
    pop(feed);
}

tok_t
tok_t_keyword(str name, bind_t bind, size_t size)
{
    return tok_t_init(str_init(""), name, bind, size, 0);
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

str
digit(deq_char* feed)
{
    str s = str_init("");
    while(is_digit(peek(feed)))
    {
        str_push_back(&s, peek(feed));
        pop(feed);
    }
    return s;
}

str
operator(deq_char* feed)
{
    str s = str_init("");
    while(is_operator(peek(feed)))
    {
        str_push_back(&s, peek(feed));
        pop(feed);
    }
    return s;
}

set_tok_t
setup_keywords(void)
{
    set_tok_t toks = set_tok_t_init(tok_t_key_compare);
    char* internals[] = {
        "if",
        "else",
        "while",
    };
    for(size_t i = 0; i < len(internals); i++)
    {
        tok_t tok = tok_t_keyword(str_init(internals[i]), INTERNAL, 0);
        set_tok_t_put(&toks, tok);
    }
    struct
    {
        char* a;
        size_t b;
    }
    types[] = {
        { "u8",   1 },
        { "void", 0 },
    };
    for(size_t i = 0; i < len(types); i++)
    {
        tok_t tok = tok_t_keyword(str_init(types[i].a), TYPE, types[i].b);
        set_tok_t_put(&toks, tok);
    }
    struct
    {
        char* a;
        bind_t b;
    }
    mixed[] = {
        { "struct", STRUCTURE },
        { "return", RETURN    },
    };
    for(size_t i = 0; i < len(mixed); i++)
    {
        tok_t tok = tok_t_keyword(str_init(mixed[i].a), mixed[i].b, 0);
        set_tok_t_put(&toks, tok);
    }
    return toks;
}

void
structure(deq_char* feed, set_tok_t* toks)
{
    tok_t tok = tok_t_init(str_init("struct"), identifier(feed), TYPE, 0, 0);
    match(feed, '{');
    size_t offset = 0;
    while(next(feed) != '}')
    {
        str type = identifier(feed);
        str name = identifier(feed);
        size_t size = set_tok_t_get(toks, type)->var.size;
        var_t var = var_t_init(type, name, VARIABLE, size, offset);
        set_var_t_put(&tok.member, var);
        offset += size;
        match(feed, ';');
    }
    tok.var.size = offset;
    set_tok_t_put(toks, tok);
    match(feed, '}');
}

str
slide(deq_char* feed, set_tok_t* toks, str name)
{
    str full = str_copy(&name);
    var_t* var = &set_tok_t_get(toks, name)->var;
    tok_t* tok = set_tok_t_get(toks, var->type);
    while(next(feed) == '.')
    {
        match(feed, '.');
        str_push_back(&full, '.');
        tok_t* internal = set_tok_t_get(toks, tok->var.type);
        if(internal->var.bind != STRUCTURE)
            quit("dot operator used on element that is not a structure");
        str n = identifier(feed);
        str_append(&full, n.value);
        var_t* sub = set_var_t_get(&tok->member, n);
        tok = set_tok_t_get(toks, sub->type);
        str_free(&n);
    }
    return full;
}

void
expression(deq_char* feed, set_tok_t* toks);

void
call(deq_char* feed, set_tok_t* toks)
{
}

void
term(deq_char* feed, set_tok_t* toks)
{
    if(next(feed) == '(')
    {
        match(feed, '(');
        expression(feed, toks);
        match(feed, ')');
    }
    else
    if(is_digit(next(feed)))
    {
        str s = digit(feed);
        str_free(&s);
    }
    else
    if(is_ident(next(feed)))
    {
        str s = identifier(feed);
        var_t* var = &set_tok_t_get(toks, s)->var;
        if(var->bind == FUNCTION)
        {
            match(feed, '(');
            size_t args = 0;
            while(next(feed) != ')')
            {
                expression(feed, toks);
                args += 1;
                if(next(feed) == ')')
                    break;
                match(feed, ',');
            }
            if(args != var->size)
                quit("argument mismatch");
            match(feed, ')');
            call(feed, toks);
        }
        else
        if(var->bind == VARIABLE || var->bind == REFERENCE)
        {
            str full = slide(feed, toks, s);
            str_free(&full);
        }
        else
            quit("unknown token bind '%s'", printable(var->bind));
        str_free(&s);
    }
    else
        quit("unable to parse expression");
}

void
expression(deq_char* feed, set_tok_t* toks)
{
    term(feed, toks);
    while(is_operator(next(feed)))
    {
        str o = operator(feed);
        if(str_compare(&o, "+") == 0)
        {
        }
        else
        if(str_compare(&o, "-") == 0)
        {
        }
        else
            quit("unsupported operator");
        term(feed, toks);
        str_free(&o);
    }
}

void
local(str* type, str* name, size_t size, set_tok_t* toks, int* sp, vec_str* locals, bind_t bind)
{
    tok_t tok = tok_t_init(str_copy(type), str_copy(name), bind, size, *sp);
    *sp += size;
    set_tok_t_put(toks, tok);
    vec_str_push_back(locals, str_copy(name));
}

void
ret(deq_char* feed, set_tok_t* toks)
{
    expression(feed, toks);
    match(feed, ';');
}

int
tok_t_is_structure(tok_t* tok)
{
    return tok->member.size > 0;
}

void
brace_initializer(deq_char* feed, set_tok_t* toks, tok_t* tok)
{
    match(feed, '{');
    while(next(feed) != '}')
    {
        match(feed, '.');
        str name = identifier(feed);
        match(feed, '=');
        expression(feed, toks);
        set_var_t_get(&tok->member, name); // JUST TO ENSURE.
        str_free(&name);
        if(next(feed) == '}')
            break;
        match(feed, ',');
    }
    match(feed, '}');
    match(feed, ';');
}

void
backup(deq_char* feed, str* s)
{
    while(!str_empty(s))
    {
        char c = *str_back(s);
        deq_char_push_front(feed, c);
        str_pop_back(s);
    }
}

void
block(deq_char* feed, set_tok_t* toks, int* sp, vec_str* locals)
{
    match(feed, '{');
    while(next(feed) != '}')
    {
        if(next(feed) == '(' || is_digit(next(feed)))
        {
            expression(feed, toks);
            match(feed, ';');
        }
        else
        {
            str ident = identifier(feed);
            var_t* var = &set_tok_t_get(toks, ident)->var;
            // INITIALIZE.
            if(var->bind == TYPE)
            {
                int is_ref = 0;
                str type = str_copy(&ident);
                if(next(feed) == '&')
                {
                    is_ref = 1;
                    match(feed, '&');
                }
                str name = identifier(feed);
                if(is_ref)
                    local(&type, &name, POINTER_SIZE, toks, sp, locals, REFERENCE);
                else
                    local(&type, &name, var->size, toks, sp, locals, VARIABLE);
                match(feed, '=');
                tok_t* tok = set_tok_t_get(toks, type);
                // BY REFERENCE.
                if(is_ref)
                {
                    str other = identifier(feed);
                    var_t* a = &set_tok_t_get(toks, name)->var;
                    var_t* b = &set_tok_t_get(toks, other)->var;
                    if(str_key_compare(&a->type, &b->type) != 0)
                        quit("reference type mismatch (`%s` and `%s`)", a->type.value, b->type.value);
                    str_free(&other);
                    match(feed, ';');
                }
                // BY STRUCTURE BRACE.
                else
                if(tok_t_is_structure(tok))
                    brace_initializer(feed, toks, tok);
                // BY EXPRESSION.
                else
                {
                    expression(feed, toks);
                    match(feed, ';');
                }
                str_free(&type);
                str_free(&name);
            }
            else
            if(var->bind == RETURN)
                ret(feed, toks);
            // ASSIGN, OR BACKUP AND EXECUTE EXPRESSION.
            else
            {
                str temp = slide(feed, toks, ident);
                if(next(feed) == '=')
                    match(feed, '=');
                else
                    backup(feed, &temp);
                expression(feed, toks);
                match(feed, ';');
                str_free(&temp);
            }
            str_free(&ident);
        }
    }
    match(feed, '}');
}

void
function(deq_char* feed, set_tok_t* toks, int* sp, str ret_type)
{
    vec_str locals = vec_str_init();
    str fun_name = identifier(feed);
    match(feed, '(');
    size_t size = 0;
    while(next(feed) != ')')
    {
        int is_ref = 0;
        str type = identifier(feed);
        var_t* var = &set_tok_t_get(toks, type)->var;
        if(next(feed) == '&')
        {
            is_ref = 1;
            match(feed, '&');
        }
        else
        {
            tok_t* info = set_tok_t_get(toks, type);
            tok_t* internal = set_tok_t_get(toks, info->var.type);
            if(internal->var.bind == STRUCTURE)
                quit("structures may only be passed to functions as references");
        }
        str name = identifier(feed);
        if(var->bind != TYPE)
            quit("unknown type '%s'", type.value);
        size += 1;
        if(is_ref)
            local(&type, &name, POINTER_SIZE, toks, sp, &locals, REFERENCE);
        else
            local(&type, &name, var->size, toks, sp, &locals, VARIABLE);
        str_free(&type);
        str_free(&name);
        if(next(feed) == ')')
            break;
        match(feed, ',');
    }
    match(feed, ')');
    set_tok_t_put(toks, tok_t_init(ret_type, fun_name, FUNCTION, size, 0));
    block(feed, toks, sp, &locals);
    foreach(vec_str, &locals, it,
        set_tok_t_node* node = set_tok_t_find(toks, tok_t_key(*it.ref));
        var_t* var = &node->key.var;
        //var_t_print(var, 0);
        *sp -= var->size;
        set_tok_t_erase_node(toks, node);
    )
    vec_str_free(&locals);
}

void
program(deq_char* feed, set_tok_t* toks, int* sp)
{
    while(!deq_char_empty(feed))
    {
        str ident = identifier(feed);
        var_t* var = &set_tok_t_get(toks, ident)->var;
        if(var->bind == STRUCTURE)
            structure(feed, toks);
        else
        if(var->bind == TYPE)
            function(feed, toks, sp, str_copy(&ident));
        str_free(&ident);
        advance(feed);
    }
}

int
main(void)
{
    str code = str_init(
        "   struct point                    \n"
        "   {                               \n"
        "       u8 x;                       \n"
        "       u8 y;                       \n"
        "   }                               \n"
        "   void test(u8& a)                \n"
        "   {                               \n"
        "   }                               \n"
        "   void add(point& a, point& b)    \n"
        "   {                               \n"
        "       a.x = a.x + b.x;            \n"
        "       a.y = a.y + b.y;            \n"
        "   }                               \n"
        "   u8 one()                        \n"
        "   {                               \n"
        "       return 1;                   \n"
        "   }                               \n"
        "   u8 main()                       \n"
        "   {                               \n"
        "       point a = {.x = 1, .y = 2}; \n"
        "       point b = {.x = 1, .y = 2}; \n"
        "       u8 c = 4 - (one() + 3);     \n"
        "       test(c);                    \n"
        "       add(a, b);                  \n"
        "   }                               \n"
    );
    int sp = 0;
    set_tok_t toks = setup_keywords();
    deq_char feed = extract(&code);
    program(&feed, &toks, &sp);
#if 1
    printf("\n -- FULL DUMP -- \n\n");
    foreach(set_tok_t, &toks, it, tok_t_print(it.ref);)
#endif
    set_tok_t_free(&toks);
    deq_char_free(&feed);
    str_free(&code);
}
