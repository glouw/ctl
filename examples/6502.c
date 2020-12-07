//
// -- A 6502 COMPILER --
//

#include <stdio.h>
#include <stdarg.h>
#include <str.h>

#define quit(msg, ...) { printf(msg, __VA_ARGS__); exit(1); }

#define P
#define T char
#include <deq.h>

#define T str
#include <vec.h>

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t offset;
    vec_str args;
    int is_function;
}
token;

token
token_init(char* type, char* name, size_t size, size_t offset)
{
    return (token) {
        str_init(type),
        str_init(name),
        size,
        offset,
        vec_str_init(),
        .is_function = 0,
    };
}

token
token_copy(token* self)
{
    return (token) {
        str_copy(&self->type),
        str_copy(&self->name),
        self->size,
        self->offset,
        vec_str_copy(&self->args),
        self->is_function,
    };
}

void
token_free(token* self)
{
    str_free(&self->type);
    str_free(&self->name);
    vec_str_free(&self->args);
}

int
token_key_compare(token* a, token* b)
{
    return str_key_compare(&a->name, &b->name);
}

int
token_is_type(token* self)
{
    return self->size > 0;
}

#define T token
#include <set.h>

#define T str
#include <lst.h>

struct
{
    deq_char feed;
    set_token tokens;
    lst_str assem;
    vec_str local_stack;
    size_t addr;
}
global;

void
global_init(void)
{
    global.feed = deq_char_init();
    global.tokens = set_token_init(token_key_compare);
    global.assem = lst_str_init();
    global.local_stack = vec_str_init();
    global.addr = 0;
}

void
global_free(void)
{
    deq_char_free(&global.feed);
    set_token_free(&global.tokens);
    lst_str_free(&global.assem);
    vec_str_free(&global.local_stack);
}

token*
find(str name)
{
    token key;
    key.name = name;
    set_token_node* node;
    if((node = set_token_find(&global.tokens, key)))
        return &node->key;
    return NULL;
}

void
insert(token tok)
{
    set_token_insert(&global.tokens, tok);
}

void
erase(str name)
{
    token key;
    key.name = name;
    set_token_erase(&global.tokens, key);
}

void
write(char* format, ...)
{
    va_list args;
    va_list copy;
    va_start(args, format);
    va_copy(copy, args);
    str s = str_init("");
    size_t size = vsnprintf(NULL, 0, format, args);
    str_resize(&s, size + 1, '\0');
    vsprintf(s.value, format, copy);
    va_end(args);
    va_end(copy);
    lst_str_push_back(&global.assem, s);
}

void
token_write(token* tok)
{
    char* type = tok->type.value;
    char* name = tok->name.value;
    size_t size = tok->size;
    size_t offset = tok->offset;
    size_t args_size = tok->args.size;
    int function = tok->is_function;
    write("; %12s %12s %4lu %4lu %4lu %4d", type, name, size, offset, args_size, function);
}

void
global_info(void)
{
    write("; GLOBAL INFO");
    foreach(set_token, &global.tokens, it,
        token_write(it.ref);
    )
}

void
stack_info(void)
{
    write("; STACK INFO");
    foreach(vec_str, &global.local_stack, it,
        token* found = find(*it.ref);
        token_write(found);
    )
}

void
stack_clear(void)
{
    foreach(vec_str, &global.local_stack, it,
        token* found = find(*it.ref);
        erase(found->name);
    )
    vec_str_clear(&global.local_stack);
}

void
save(void)
{
    foreach(lst_str, &global.assem, it,
        char* s = it.ref->value;
        fprintf(stdout, "%s\n", s);
    )
}

void
prime(str* text)
{
    while(text->size)
    {
        deq_char_push_front(&global.feed, *str_back(text));
        str_pop_back(text);
    }
}

int
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int
is_operator(char c)
{
    return c == '+'
        || c == '-'
        || c == '&';
}

int
is_ident(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c == '_')
        || is_digit(c);
}

int
is_space(char c)
{
    return c == ' '
        || c == '\t'
        || c == '\n'
        || c == '\r';
}

char
peek(void)
{
    return *deq_char_front(&global.feed);
}

void
pop(void)
{
    deq_char_pop_front(&global.feed);
}

int
next(void)
{
    char c;
    while(is_space(c = peek()))
    {
        pop();
        if(deq_char_empty(&global.feed))
            return EOF;
    }
    return c;
}

str
read(int clause(char c))
{
    next();
    str s = str_init("");
    char c;
    while(clause(c = peek()))
    {
        str_push_back(&s, c);
        pop();
    }
    return s;
}

str
ident(void)
{
    str s = read(is_ident);
    if(is_digit(s.value[0]))
        quit("identifier '%s' may not start with a digit", s.value);
    return s;
}

str
digit(void)
{
    str d = read(is_digit);
    return d;
}

str
operator(void)
{
    str o = read(is_operator);
    return o;
}

void
match(char c)
{
    char n;
    if((n = next()) != c)
        quit("expected '%c', got '%c'", c, n);
    pop();
}

void
call(void)
{
    str name = ident();
    match('(');
    match(')');
    str_free(&name);
}

str
expression(void);

str // Returns decal string for type information.
term(void)
{
    if(next() == '(')
    {
        match('(');
        str type = expression();
        match(')');
        return type;
    }
    else
    if(is_digit(next()))
    {
        str d = digit();
        str_free(&d);
        return str_init("u8");
    }
    else
    if(is_ident(next()))
    {
        str n = ident();
        token* tok = find(n);
        if(!tok)
            quit("unable to infer term type '%s'\n", n.value);
        str_free(&n);
        return str_copy(&tok->type);
    }
    else
        quit("unknown term; char was '%c'", next());
}

str
expression(void)
{
    str type_a = term();
    while(is_operator(next()))
    {
        str o = operator();
        str type_b = term();
        if(str_key_compare(&type_a, &type_b) != 0)
            quit("type '%s' and '%s' mismatch", type_a.value, type_b.value);
        str_free(&o);
        str_free(&type_b);
    }
    return type_a;
}

str
decal(void)
{
    str type = ident();
    if(next() == '*')
    {
        pop();
        str_append(&type, "*");
    }
    if(!find(type))
        quit("type '%s' not defined", type.value);
    return type;
}

void
function_args(token* fun_tok)
{
    match('(');
    while(1)
    {
        if(next() == ')')
            break;
        str type = decal();
        str name = ident();
        vec_str_push_back(&global.local_stack, str_copy(&name));
        vec_str_push_back(&fun_tok->args, str_copy(&type));
        token* type_tok = find(type);
        global.addr += type_tok->size;
        str_free(&type);
        str_free(&name);
        if(next() == ',')
            match(',');
        else
            break;
    }
    match(')');
}

void
assign(void)
{
    str name = ident();
    token* name_tok = find(name);
    if(!name_tok)
        quit("'%s' not defined\n", name.value);
    match('=');
    str type_computed = expression();
    if(str_key_compare(&type_computed, &name_tok->type) != 0)
        quit("initializing type '%s' with computed type '%s'", name_tok->type.value, type_computed.value);
    str_free(&name);
    str_free(&type_computed);
}

void
initialize(void)
{
    str type = decal();
    token* type_tok = find(type);
    if(!type_tok)
        quit("unknown type '%s'\n", type.value);
    str name = ident();
    token* name_tok = find(name);
    if(name_tok)
        quit("'%s' already defined\n", name.value);
    match('=');
    str type_computed = expression();
    if(str_key_compare(&type_computed, &type) != 0)
        quit("initializing type '%s' with computed type '%s'", type.value, type_computed.value);
    vec_str_push_back(&global.local_stack, str_copy(&name));
    insert(token_init(type.value, name.value, type_tok->size, global.addr));
    global.addr += type_tok->size;
    str_free(&type_computed);
    str_free(&type);
    str_free(&name);
}

void
block(void)
{
    match('{');
    while(1)
    {
        if(next() == '}')
            break;
        if(is_digit(next()) || next() == '(')
        {
            // Expression starts with a digit or parenthesis
            str type = expression();
            str_free(&type);
        }
        else
        {
            str name = ident();
            token* tok = find(name);
            if(!tok)
                quit("token '%s' not defined\n", name.value);
            char n = next();
            str_append(&name, " ");
            prime(&name);
            if(n == '(')
            {
                // Function is called.
                // Note that functions are not terms since they do not return values.
                if(!tok->is_function)
                    quit("token '%s' is not a function", tok->name.value);
                call();
            }
            else
            if(is_operator(n))
            {
                // Expression starts with a variable.
                str type = expression();
                str_free(&type);
            }
            else
            if(n == '=')
            {
                // Expression follows an equal sign
                assign();
            }
            else
            if(token_is_type(tok))
            {
                // Expression follows a type, a variable, and an equal sign.
                initialize();
            }
            else
                quit("unknown '%s' in block statement", name.value);
            str_free(&name);
        }
        match(';');
    }
    match('}');
}

void
function(void)
{
    global.addr = 0;
    str name = ident();
    if(find(name))
        quit("function '%s' already defined", name.value);
    token fun_tok = token_init("void", name.value, 0, 0);
    function_args(&fun_tok);
    fun_tok.is_function = 1;
    insert(fun_tok);
    block();
    stack_info();
    stack_clear();
    str_free(&name);
}

void
program(void)
{
    while(1)
    {
        function();
        if(next() == EOF)
            break;
    }
    global_info();
}

void
init_keywords(void)
{
    insert(token_init("u8",   "u8",   1, 0));
    insert(token_init("u8*",  "u8*",  2, 0));
    insert(token_init("void", "void", 0, 0));
}

void
compile(str* text)
{
    global_init();
    init_keywords();
    prime(text);
    program();
    save();
    global_free();
}

int
main(void)
{
    str text = str_init(
        " test()                          \n"
        " {                               \n"
        "     u8 a = 1;                   \n"
        " }                               \n"
        "                                 \n"
        " main()                          \n"
        " {                               \n"
        "     u8 a = 1;                   \n"
        "     u8 b = 1;                   \n"
        "     b = 1 + 1;                  \n"
        "     u8 c = 1;                   \n"
        "     u8 d = 1;                   \n"
        "     (1 + 1);                    \n"
        "     (1 + 2);                    \n"
        "     u8 e = 1;                   \n"
        " }                               \n"
        "                                 \n"
        "                                 \n");
    if(text.size != 0)
        compile(&text);
    str_free(&text);
}
