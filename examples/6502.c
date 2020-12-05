//
// -- SIMPLE 6502 COMPILER --
//

#include <stdio.h>
#include <stdarg.h>
#include <str.h>
#include <assert.h>

#define P
#define T char
#include <deq.h>

deq_char feed;

typedef char* charp;

int
charp_key_compare(char** a, char** b)
{
    return strcmp(*a, *b);
}

#define P
#define T charp
#include <set.h>

struct
{
    int line;
    int addr;
    str function;
}
global;

#define quit(message, ...) { printf("error: line %d: "message, global.line, __VA_ARGS__); exit(1); }

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t addr;
    size_t args; // For functions.
}
token;

token
token_copy(token* self)
{
    return (token) {
        str_copy(&self->type),
        str_copy(&self->name),
        self->size,
        self->addr,
        self->args,
    };
}

void
token_free(token* self)
{
    str_free(&self->type);
    str_free(&self->name);
}

int
token_key_compare(token* a, token* b)
{
    return strcmp(a->name.value, b->name.value);
}

token
identifier(char* type, char* name, size_t size, size_t addr, size_t args)
{
    return (token) {
        str_init(type),
        str_init(name),
        size,
        addr,
        args,
    };
}

token
keyword(char* name, size_t size)
{
    return identifier(name, name, size, 0, 0);
}

// Table column width.
#define W "8"

void
token_print(token* self)
{
    printf("%"W"s %"W"s %"W"lu %"W"lu %"W"lu\n",
        self->name.value,
        self->type.value,
        self->size,
        self->addr,
        self->args);
}

#define T token
#include <set.h>

set_token tokens;

void
dump(void)
{
    printf("%"W"s %"W"s %"W"s %"W"s %"W"s\n", "NAME", "VALUE", "SIZE", "ADDR", "ARGS");
    foreach(set_token, &tokens, it, token_print(it.ref);)
}

token*
find(const str name)
{
    token key;
    key.name = name;
    set_token_node* node = set_token_find(&tokens, key);
    if(node)
        return &node->key;
    return NULL;
}

void
insert(token t)
{
    set_token_insert(&tokens, t);
}

void
queue(str* code)
{
    for(size_t i = 0; i < code->size; i++)
        deq_char_push_back(&feed, code->value[i]);
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
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int
is_operator(char c)
{
    return !is_space(c) && !is_digit(c) && !is_alpha(c) && c != '(';
}

int
valid_operator(str* s)
{
    return str_compare(s, "+") == 0
        || str_compare(s, "-") == 0;
}

int
is_ident(char c)
{
    return is_digit(c) || is_alpha(c) || c == '_';
}

void
pop(void)
{
    deq_char_pop_front(&feed);
}

int
end(void)
{
    return deq_char_empty(&feed);
}

char
front(void)
{
    return *deq_char_front(&feed);
}

void
space(void)
{
    for(char c; !end() && is_space(c = front());)
    {
        if(c == '\n')
            global.line += 1;
        pop();
    }
}

char
next(void)
{
    space();
    return front();
}

void
match(char c)
{
    if(next() != c)
        quit("expected '%c' but got '%c'\n", c, next());
    pop();
}

str
read(int pred(char))
{
    space();
    str s = str_init("");
    for(char c; pred(c = front());)
    {
        str_push_back(&s, c);
        pop();
    }
    return s;
}

int
is_ref(str* t)
{
    return *str_back(t) == '&';
}

charp
infer(str* t)
{
    charp type;
    if(is_digit(t->value[0]))
        type = "u8";
    else
    {
        int ref = is_ref(t);
        if(ref)
            str_pop_back(t);
        token* tok = find(*t);
        if(tok)
        {
            if(ref)
            {
                str temp = str_copy(&tok->type);
                str_append(&temp, "&");
                token* ref_tok = find(temp);
                if(ref_tok)
                    type = ref_tok->type.value;
                else
                    quit("type '%s' not defined; type inference failed", temp.value);
                str_free(&temp);
            }
            else
                type = tok->type.value;
        }
        else
            quit("type '%s' not defined; type inference failed", t->value);
    }
    return type;
}

str
expression(set_charp* types);

str
term(set_charp* types)
{
    if(next() == '(')
    {
        match('(');
        str out = expression(types);
        match(')');
        return out;
    }
    else
    if(is_digit(next()))
        return read(is_digit);
    else
    if(is_ident(next()))
        return read(is_ident);
    else
    {
        char unary = next();
        if(unary == '&')
        {
            match('&');
            str name = read(is_ident);
            str_append(&name, "&");
            return name;
        }
        else
            quit("unary operator '%c' not supported; term generation failed", unary);
        return str_init("");
    }
}

str
expression(set_charp* types)
{
    str t = term(types);
    set_charp_insert(types, infer(&t));
    while(1)
    {
        if(next() == ')')
            break;
        if(next() == ';')
            break;
        str o = read(is_operator);
        if(str_empty(&o))
            quit("missing operator - expression generation failed - previous term was %s", t.value);
        if(!valid_operator(&o))
            quit("invalid operator '%s'; expression generation failed", o.value);
        str_free(&o);
        str u = term(types);
        set_charp_insert(types, infer(&u));
        str_free(&u);
        if(types->size > 1)
        {
            size_t i = 0;
            foreach(set_charp, types, it,
                printf("%2lu : %s\n", i, *it.ref);
                i += 1;
            )
            quit("type mismatch - expression generation failed - see types above (size %lu)", types->size);
        }
    }
    str_free(&t);
    return str_init(types->root->key);
}

void
evaluate(const str ident)
{
    set_charp types = set_charp_init(charp_key_compare);
    str type = expression(&types);
    if(types.size == 0)
        quit("empty initializer - evaluation failed - see identifier '%s'", ident.value);
    set_charp_free(&types);
    token* defined = find(type);
    if(!defined)
        quit("type '%s' not defined; evaluation failed", type.value);
    token* exists = find(ident);
    if(exists)
    {
        if(str_key_compare(&type, &exists->type) != 0)
            quit("assignment type mismatch - evaluation failed - '%s' and '%s'", type.value, exists->type.value);
    }
    else
    {
        insert(identifier(type.value, ident.value, defined->size, global.addr, 0));
        global.addr += defined->size;
    }
    str_free(&type);
}

void
fallback(const str ident)
{
    for(size_t i = 0; i < ident.size; i++)
        deq_char_push_front(&feed, ident.value[i]);
    set_charp types = set_charp_init(charp_key_compare);
    str type = expression(&types);
    if(types.size > 1)
    {
        size_t i = 0;
        foreach(set_charp, &types, it,
            printf("%2lu : %s\n", i, *it.ref);
            i += 1;
        )
        quit("type mismatch - expression fallback failed - see types above (size %lu)", types.size);
    }
    str_free(&type);
    set_charp_free(&types);
}

void
ret(void)
{
    token* tok = find(global.function);
    if(str_compare(&tok->type, "void") != 0)
    {
        if(next() == ';')
            quit("empty return statement in function '%s' requires type '%s'; return statement failed", global.function.value, tok->type.value);
        set_charp types = set_charp_init(charp_key_compare);
        str type = expression(&types);
        if(str_compare(&tok->type, type.value) != 0)
            quit("computed return statement in function '%s' is '%s' but expected '%s'; return statement failed", global.function.value, type.value, tok->type.value);
        str_free(&type);
        set_charp_free(&types);
    }
    else
    if(next() != ';')
        quit("void function '%s' may not compute a return expression; return statement failed", global.function.value);
    match(';');
}

void
statement(void)
{
    while(1)
    {
        if(next() == ';')
            match(';');
        if(next() == '}')
            break;
        str ident = read(is_ident);
        if(next() == '=')
        {
            match('=');
            if(next() == ';')
                quit("assignments may not be empty - statement failed - see '%s'", ident.value);
            evaluate(ident);
        }
        else
        if(str_compare(&ident, "return") == 0)
            ret();
        else
            fallback(ident);
        str_free(&ident);
    }
}

void
block(void)
{
    match('{');
    statement();
    match('}');
}

str
type(void)
{
    str ident = read(is_ident);
    if(next() == '&')
    {
        str_push_back(&ident, '&');
        pop();
    }
    return ident;
}

void
function(void)
{
    str fn = read(is_ident);
    if(find(fn))
        quit("function '%s' already defined; failed function definition", fn.value);
    str_free(&global.function);
    global.function = str_copy(&fn);;
    match('(');
    size_t args = 0;
    while(1)
    {
        if(next() == ')')
            break;
        str t = type();
        str n = read(is_ident);
        if(find(n))
            quit("function arguments must be unique - failed function definition - see type '%s'", t.value);
        args += 1;
        token* tok = find(t);
        if(!tok)
            quit("unknown type in function paramater list - failed function definition - see type '%s'", t.value);
        size_t size = tok->size;
        insert(identifier(t.value, n.value, size, global.addr, 0));
        global.addr += size;
        str_free(&t);
        str_free(&n);
        if(next() == ',')
            match(',');
    }
    match(')');
    if(next() == '-')
    {
        match('-');
        match('>');
        str t = read(is_ident);
        token* tok = find(t);
        if(!tok)
            quit("unknown function return type; failed function definition - see '%s'", t.value);
        insert(identifier(t.value, fn.value, tok->size, 0, args));
        str_free(&t);
    }
    else
        insert(identifier("void", fn.value, 0, 0, args));
    block();
    str_free(&fn);
}

void
program(void)
{
    while(1)
    {
        function();
        space();
        if(end())
            break;
    }
}

void
setup(void)
{
    feed = deq_char_init();
    tokens = set_token_init(token_key_compare);
    insert(keyword("u8", 1));
    insert(keyword("u8&", 2));
    insert(keyword("void", 0));
    global.line = 0;
    global.addr = 0;
    str_free(&global.function);
}

void
compile(char* text)
{
    setup();
    str code = str_init(text);
    queue(&code);
    program();
    dump();
    str_free(&code);
    deq_char_free(&feed);
    set_token_free(&tokens);
}

void
test_spacing(void)
{
    compile(
        "main()                    \n"
        "{                         \n"
        "    a = 0;                \n"
        "    a  = 1+0+1;           \n"
        "    a   = 1+(0)+1;        \n"
        "    a    = 1+(0+0)+1;     \n"
        "    b =  1 + ( 0+0 ) + 1; \n"
        "    b =  1 + (  0  ) + 1; \n"
        "    b =  1 + (0 + 0) + 1; \n"
        "    0;                    \n"
        "    1+0+1;                \n"
        "    1+(0)+1;              \n"
        "    1+(0+0)+1;            \n"
        "    1 + ( 0+0 ) + 1;      \n"
        "    1 + (  0  ) + 1;      \n"
        "    1 + (0 + 0) + 1;      \n"
        "}                         \n");
}

void
test_pointer_init(void)
{
    compile(
        "fun() -> u8              \n"
        "{                        \n"
        "    a = 1;               \n"
        "    return 1;            \n"
        "}                        \n"
        "nuf()                    \n"
        "{                        \n"
        "    a = 1;               \n"
        "    return 0;            \n"
        "}                        \n"
        "main()                   \n"
        "{                        \n"
        "    a = 1;               \n"
        "}                        \n");
}

int
main(void)
{
    test_spacing();
    test_pointer_init();
}
