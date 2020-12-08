//
// -- A 6502 COMPILER --
//

#include <stdio.h>
#include <stdarg.h>
#include <str.h>

size_t line = 1;

#define quit(msg, ...) { printf("error: line %lu: internal line %d: "msg, line, __LINE__, __VA_ARGS__); exit(1); }

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
    size_t addr;
    vec_str params;
    int is_function;
    int is_type;
}
token;

token
token_init(char* type, char* name, size_t size, size_t addr)
{
    return (token) {
        str_init(type),
        str_init(name),
        size,
        addr,
        vec_str_init(),
        .is_function = 0,
        .is_type = 0,
    };
}

token
token_copy(token* self)
{
    return (token) {
        str_copy(&self->type),
        str_copy(&self->name),
        self->size,
        self->addr,
        vec_str_copy(&self->params),
        self->is_function,
        self->is_type,
    };
}

void
token_free(token* self)
{
    str_free(&self->type);
    str_free(&self->name);
    vec_str_free(&self->params);
}

int
token_key_compare(token* a, token* b)
{
    return str_key_compare(&a->name, &b->name);
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
    str line;
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
    global.line = str_init("");
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
    size_t addr = tok->addr;
    size_t params_size = tok->params.size;
    int is_function = tok->is_function;
    int is_type = tok->is_type;
    write("; %12s %12s %4lu %4lu %4lu %4d %4d", type, name, size, addr, params_size, is_function, is_type);
}

void
info_header()
{
    write("; %12s %12s %4s %4s %4s %4s %4s", "T", "N", "S", "A", "P", "F?", "T?");
}

void
global_info(void)
{
    write("; -- end of program --");
    write("; global info");
    info_header();
    foreach(set_token, &global.tokens, it,
        token_write(it.ref);
    )
}

void
stack_info(char* name)
{
    if(global.local_stack.size > 0)
    {
        write("; '%s' stack info", name);
        info_header();
        foreach(vec_str, &global.local_stack, it,
            token* found = find(*it.ref);
            token_write(found);
        )
    }
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
    FILE* out = fopen("out.asm", "w");
    foreach(lst_str, &global.assem, it,
        char* s = it.ref->value;
        fprintf(out, "%s\n", s);
    )
    fclose(out);
}

void
prime(str* text)
{
    while(text->size)
    {
        char c = *str_back(text);
        deq_char_push_front(&global.feed, c);
        if(!str_empty(&global.line))
            str_pop_back(&global.line);
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
is_unary(char c)
{
    return c == '&'
        || c == '*'
        || c == '-'
        || c == '+';
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
    char c;
    if((c = peek()) != '\n')
        str_push_back(&global.line, c);
    deq_char_pop_front(&global.feed);
}

void
trim(str* s)
{
    while(!str_empty(s) && is_space(*str_back(s)))
        str_pop_back(s);
    while(!str_empty(s) && is_space(*str_front(s)))
        str_erase(s, str_begin(s));
}

int
next(void)
{
    char c;
    while(is_space(c = peek()))
    {
        if(c == '\n')
        {
            line += 1;
            trim(&global.line);
            write("; %s", global.line.value);
            str_clear(&global.line);
        }
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
    int value = atoi(d.value);
    if(value > 255)
        quit("value '%s' too large", d.value);
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

str
expression(void);

void
call_params(token* fun_tok)
{
    match('(');
    size_t index = 0;
    while(1)
    {
        if(next() == ')')
            break;
        index += 1;
        if(index > fun_tok->params.size)
            break;
        str type_computed = expression();
        str* expected_type = &fun_tok->params.value[index - 1];
        if(str_key_compare(&type_computed, expected_type) != 0)
            quit("argument type '%s' does not match computed type '%s'", expected_type->value, type_computed.value);
        str_free(&type_computed);
        if(next() == ',')
            match(',');
    }
    if(index != fun_tok->params.size)
        quit("function '%s' expected '%lu' arguments but received '%lu' arguments", fun_tok->name.value, fun_tok->params.size, index);
    match(')');
}

token*
get_token(str* name)
{
    if(str_empty(name))
        quit("<<< INTERNAL COMPILER ERROR >>> name '%s' empty with token lookup\n", name->value);
    token* tok = find(*name);
    if(!tok)
        quit("type '%s' not defined\n", name->value);
    return tok;
}

void
push_stack(size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        write("\tLDA  %3d", i);
        write("\tPHA");
    }
}

void
pop_stack(size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        write("\tPLA");
        write("\tSTA  %3d", size - i - 1);
    }
}

void
call(void)
{
    str name = ident();
    token* fun_tok = get_token(&name);
    size_t size = global.addr;
    push_stack(size);
    call_params(fun_tok);
    write("\tJSR %s", name.value);
    pop_stack(size);
    str_free(&name);
}

str
unary(void)
{
    char n = next();
    if(n == '&')
    {
        pop();
        str name = ident();
        token* tok = get_token(&name);
        str type = str_copy(&tok->type);
        str_append(&type, "*");
        write("\tLDA #%3d", (tok->addr >> 0) & 0xFF);
        write("\tSTA  %3d", global.addr + 0);
        write("\tLDA #%3d", (tok->addr >> 8) & 0xFF);
        write("\tSTA  %3d", global.addr + 1);
        str_free(&name);
        return type;
    }
    else
    if(n == '*')
    {
        pop();
        str name = ident();
        token* tok = get_token(&name);
        write("\tLDY #%3d", 0);
        write("\tLDA ($%02X),Y", tok->addr);
        write("\tSTA  %3d", global.addr);
        str_free(&name);
        return str_init("u8");
    }
    else
        quit("unary operator '%c' not supported\n", n);
}

size_t
get_size(str* type)
{
    return get_token(type)->size;
}

str // Returns decal string for type information.
term(void)
{
    str type;
    if(next() == '(')
    {
        match('(');
        type = expression();
        match(')');
    }
    else
    if(is_unary(next()))
        type = unary();
    else
    if(is_digit(next()))
    {
        str d = digit();
        write("\tLDA #%3s", d.value);
        write("\tSTA  %3d", global.addr);
        str_free(&d);
        type = str_init("u8");
    }
    else
    if(is_ident(next()))
    {
        str n = ident();
        token* tok = get_token(&n);
        for(size_t i = 0; i < tok->size; i++)
        {
            write("\tLDA  %3d", tok->addr + i);
            write("\tSTA  %3d", global.addr + i);
        }
        str_free(&n);
        type = str_copy(&tok->type);
    }
    else
    {
        type = str_init("");
        quit("unknown term; char was '%c'", next());
    }
    global.addr += get_size(&type);
    return type;
}

int
is_pointer(str* s)
{
    return *str_back(s) == '*';
}

void
operate(str* o)
{
    if(str_compare(o, "+") == 0)
    {
        write("\tCLC");
        write("\tLDA  %3d", global.addr - 1);
        write("\tADC  %3d", global.addr - 2);
        write("\tSTA  %3d", global.addr - 2);
    }
    else
    if(str_compare(o, "-") == 0)
    {
        write("\tSEC");
        write("\tLDA  %3d", global.addr - 2);
        write("\tSBC  %3d", global.addr - 1);
        write("\tSTA  %3d", global.addr - 2);
    }
    else
        quit("operator '%s' not supported", o->value);
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
            quit("operator '%s' cannot operate on types '%s' and '%s'", o.value, type_a.value, type_b.value);
        if(is_pointer(&type_a) && is_pointer(&type_b))
            quit("operator '%s' cannot operate on two pointer types '%s' and '%s'", o.value, type_a.value, type_b.value);
        operate(&o);
        global.addr -= get_size(&type_b);
        str_free(&o);
        str_free(&type_b);
    }
    global.addr -= get_size(&type_a);
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
    get_token(&type);
    return type;
}

void
function_params(token* fun_tok)
{
    match('(');
    while(1)
    {
        if(next() == ')')
            break;
        str type = decal();
        str name = ident();
        vec_str_push_back(&global.local_stack, str_copy(&name));
        vec_str_push_back(&fun_tok->params, str_copy(&type));
        token* type_tok = get_token(&type);
        insert(token_init(type.value, name.value, type_tok->size, global.addr));
        global.addr += type_tok->size;
        str_free(&type);
        str_free(&name);
        if(next() == ',')
            match(',');
    }
    match(')');
}

void
assign(void)
{
    int deref = 0;
    if(next() == '*')
    {
        deref = 1;
        pop();
    }
    str name = ident();
    token* name_tok = get_token(&name);
    match('=');
    str type_computed = expression();
    str type = str_copy(&name_tok->type);
    if(deref)
        str_pop_back(&type);
    if(str_key_compare(&type_computed, &type) != 0)
        quit("initializing type '%s' with computed type '%s'", type.value, type_computed.value);
    if(deref)
    {
        write("\tLDA  %3d", global.addr);
        write("\tLDY #%3d", 0);
        write("\tSTA ($%02X),Y", name_tok->addr);
    }
    else
    {
        write("\tLDA  %3d", global.addr);
        write("\tSTA  %3d", name_tok->addr);
    }
    str_free(&name);
    str_free(&type);
    str_free(&type_computed);
}

void
initialize(void)
{
    str type = decal();
    token* type_tok = get_token(&type);
    str name = ident();
    token* name_tok = find(name);
    if(name_tok)
        quit("'%s' already defined", name.value);
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
        int deref = 0;
        if(next() == '*')
        {
            deref = 1;
            pop();
        }
        str name = ident();
        token* tok = get_token(&name);
        char n = next();
        str_append(&name, " ");
        prime(&name);
        if(deref)
        {
            str temp = str_init("*");
            prime(&temp);
            str_free(&temp);
        }
        // a()
        if(n == '(')
        {
            if(!tok->is_function)
                quit("token '%s' is not a function", tok->name.value);
            call();
        }
        // a + 1
        else
        if(is_operator(n))
        {
            str type = expression();
            str_free(&type);
        }
        // a = 1
        else
        if(n == '=')
            assign();
        // u8 a = 1
        else
        if(tok->is_type)
            initialize();
        // *a;
        // 1;
        // (1);
        // (a);
        else
        {
            str type = expression();
            str_free(&type);
        }
        str_free(&name);
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
    write("%s:", name.value);
    token fun_tok = token_init("void", name.value, 0, 0);
    function_params(&fun_tok);
    fun_tok.is_function = 1;
    insert(fun_tok);
    block();
    write("\tRTS");
    stack_info(name.value);
    stack_clear();
    str_free(&name);
}

void
program(void)
{
    write("JMP main");
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
    struct
    {
        char* type;
        size_t size;
    }
    types[] = {
        { "u8",   1 },
        { "u8*",  2 },
        { "void", 0 },
    };
    for(size_t i = 0; i < len(types); i++)
    {
        token tok = token_init(types[i].type, types[i].type, types[i].size, 0);
        tok.is_type = 1;
        insert(tok);
    }
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
        " main()                          \n"
        " {                               \n"
        "     u8 a = 3;                   \n"
        "     u8 b = 4;                   \n"
        "     u8 c = 5;                   \n"
        "     u8 d = 6;                   \n"
        "     u8 e = 7;                   \n"
        "     u8* A = &c;                   \n"
        "     u8* B = A;                   \n"
        "     B = &e;                     \n"
        " }                               \n"
        "                                 \n"
        "                                 \n");
    if(text.size != 0)
        compile(&text);
    str_free(&text);
}
