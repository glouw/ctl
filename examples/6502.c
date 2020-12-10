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
#include <stk.h>

#define T str
#include <vec.h>

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t width;
    size_t addr;
    vec_str params;
    int is_fun;
    int is_type;
}
token;

token
token_init(char* type, char* name, size_t size, size_t width, size_t addr, int is_fun, int is_type)
{
    return (token) {
        str_init(type),
        str_init(name),
        size,
        width,
        addr,
        vec_str_init(),
        is_fun,
        is_type,
    };
}

token
token_copy(token* self)
{
    return (token) {
        str_copy(&self->type),
        str_copy(&self->name),
        self->size,
        self->width,
        self->addr,
        vec_str_copy(&self->params),
        self->is_fun,
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
    stk_char feed;
    set_token tokens;
    lst_str assem;
    vec_str var_stack;
    str line;
    size_t addr;
    size_t label;
}
global;

void
global_init(void)
{
    global.feed = stk_char_init();
    global.tokens = set_token_init(token_key_compare);
    global.assem = lst_str_init();
    global.var_stack = vec_str_init();
    global.line = str_init("");
    global.addr = 0;
    global.label = 0;
}

void
global_free(void)
{
    stk_char_free(&global.feed);
    set_token_free(&global.tokens);
    lst_str_free(&global.assem);
    vec_str_free(&global.var_stack);
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
var(token tok)
{
    insert(tok);
    vec_str_push_back(&global.var_stack, str_copy(&tok.name));
}

void
param(token* fun_tok, token tok)
{
    var(tok);
    vec_str_push_back(&fun_tok->params, str_copy(&tok.type));
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
    size_t width = tok->width;
    size_t addr = tok->addr;
    size_t params_size = tok->params.size;
    int is_fun = tok->is_fun;
    int is_type = tok->is_type;
    write("; %12s %12s %4lu %4lu %4lu %4lu %4d %4d", type, name, size, width, addr, params_size, is_fun, is_type);
}

void
var_stack_pop(size_t count)
{
    if(count > 0)
    {
        write("; %12s %12s %4s %4s %4s %4s %4s %4s", "T", "N", "S", "W", "A", "P", "F?", "T?");
        lst_str reversed = lst_str_init();
        // Reverses stack pop and erase / prints in order.
        while(count)
        {
            str* name = vec_str_back(&global.var_stack);
            lst_str_push_front(&reversed, str_copy(name));
            vec_str_pop_back(&global.var_stack);
            count -= 1;
        }
        foreach(lst_str, &reversed, it,
            token* found = find(*it.ref);
            token_write(found);
            erase(*it.ref);
        )
        lst_str_free(&reversed);
    }
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
        stk_char_push(&global.feed, c);
        if(!str_empty(&global.line))
            str_pop_back(&global.line);
        str_pop_back(text);
    }
}

int
is_pointer(str* s)
{
    return *str_back(s) == '*';
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
        || c == '$'
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
    return *stk_char_top(&global.feed);
}

void
pop(void)
{
    char c;
    if((c = peek()) != '\n')
        str_push_back(&global.line, c);
    stk_char_pop(&global.feed);
}

void
trim(str* s)
{
    while(!str_empty(s) && is_space(*str_back(s)))
        str_pop_back(s);
    while(!str_empty(s) && is_space(*str_front(s)))
        str_erase(s, 0);
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
        if(stk_char_empty(&global.feed))
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
    char c = next();
    if(is_digit(c))
        quit("identifier may not start with digit; received '%c'", c);
    str s = read(is_ident);
    return s;
}

str
digit(void)
{
    char c = next();
    if(!is_digit(c))
        quit("digit must start with a number; received '%c'", c);
    str d = read(is_digit);
    int value = atoi(d.value);
    if(value > 255)
        quit("value '%s' exceeds max 'u8' value of '255'", d.value);
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
        quit("expected '%c' but received '%c'", c, n);
    pop();
}

void
copy(size_t to, size_t from, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        write("\tLDA %d", from + i);
        write("\tSTA %d", to + i);
    }
}

token*
get_token(str* name)
{
    if(str_empty(name))
        quit("<<< INTERNAL COMPILER ERROR >>> name '%s' empty with token lookup\n", name->value);
    token* tok = find(*name);
    if(!tok)
        quit("token '%s' is not a valid identifier or type\n", name->value);
    return tok;
}

size_t
get_size(str* type)
{
    return get_token(type)->size;
}

void
label(void)
{
    write("L%lu:", global.label);
    global.label += 1;
}

void
readdress_zero_page_pointer(void)
{
    size_t lsb = global.addr + 0;
    size_t msb = global.addr + 1;
    write("; re-addressing zero page pointer");
    write("\tLDA %d", msb);
    write("\tBNE L%lu", global.label);
    write("; 1. MSB = 1");
    write("\tLDA #1");
    write("\tSTA %d", msb);
    write("; 2. LSB = X - LSB ");
    write("\tTSX");
    write("\tTXA");
    write("\tCLC");
    write("\tADC #1");
    write("\tADC %d", lsb);
    write("\tSTA %d", lsb);
    write("; finished re-addressing zero page pointer");
    label();
}

str
expression(void);

void
call_params(token* fun_tok)
{
    write("; setting up function arguments");
    match('(');
    size_t index = 0;
    size_t bytes = 0;
    while(1)
    {
        if(next() == ')')
            break;
        str type_computed = expression();
        if(index < fun_tok->params.size)
        {
            str* expected_type = &fun_tok->params.value[index];
            if(str_key_compare(&type_computed, expected_type) != 0)
                quit("argument type '%s' does not match computed type '%s'", expected_type->value, type_computed.value);
            size_t size = get_size(expected_type);
            if(is_pointer(expected_type))
                readdress_zero_page_pointer();
            copy(bytes, global.addr, size);
            bytes += size;
        }
        if(next() == ',')
            match(',');
        index += 1;
        str_free(&type_computed);
    }
    if(index != fun_tok->params.size)
        quit("function '%s' expected '%lu' arguments but received '%lu' arguments", fun_tok->name.value, fun_tok->params.size, index);
    match(')');
}

void
push_stack(size_t size)
{
    write("; pushing stack frame");
    while(size)
    {
        size -= 1;
        write("\tLDA %d", size);
        write("\tPHA");
    }
}

void
pop_stack(size_t size)
{
    write("; popping stack frame");
    for(size_t i = 0; i < size; i++)
    {
        write("\tPLA");
        write("\tSTA %d", i);
    }
}

void
jsr(char* name)
{
    write("; call '%s'", name);
    write("\tJSR %s", name);
}

void
rts(void)
{
    write("; function end");
    write("\tRTS");
}

void
call(void)
{
    str name = ident();
    token* fun_tok = get_token(&name);
    size_t size = global.addr;
    push_stack(size);
    call_params(fun_tok);
    jsr(name.value);
    pop_stack(size);
    str_free(&name);
}

str
deref_decal(str* pointer)
{
    str type = str_copy(pointer);
    if(*str_back(&type) == '*')
        str_pop_back(&type);
    else
        quit("unable to dereference '%s'\n", type.value);
    return type;
}

str
type_decal(void)
{
    str type = ident();
    while(next() == '*')
    {
        match('*');
        str_append(&type, "*");
    }
    get_token(&type);
    return type;
}

str
unary(void)
{
    char n = next();
    if(n == '&')
    {
        match(n);
        str name = ident();
        token* tok = get_token(&name);
        str type = str_copy(&tok->type);
        str_append(&type, "*");
        write("\tLDA #%d", tok->addr & 0xFF);
        write("\tSTA %d", global.addr + 0);
        write("\tLDA #%d", (tok->addr >> 8) & 0xFF);
        write("\tSTA %d", global.addr + 1);
        str_free(&name);
        return type;
    }
    else
    if(n == '$')
    {
        match(n);
        str name = ident();
        write("\tLDA #%d", get_token(&name)->width);
        write("\tSTA %d", global.addr);
        str_free(&name);
        return str_init("u8");
    }
    else
    if(n == '*')
    {
        match(n);
        str pointer_name;
        token* pointer_tok;
        if(next() == '(')
        {
            match('(');
            pointer_name = ident();
            pointer_tok = get_token(&pointer_name);
            match('+');
            str type = expression();
            if(str_compare(&type, "u8") != 0)
                quit("pointers may only be indexed by type 'u8' but received type '%s'", type.value);
            write("\tLDA #0");
            str deref_type = deref_decal(&pointer_tok->type);
            token* deref_tok = get_token(&deref_type);
            // Implicit size offseting.
            for(size_t i = 0; i < deref_tok->size; i++)
            {
                write("\tCLC");
                write("\tADC %d", global.addr);
            }
            str_free(&type);
            match(')');
        }
        else
        {
            write("\tLDA #0");
            pointer_name = ident();
            pointer_tok = get_token(&pointer_name);
        }
        write("\tTAY");
        write("\tLDA ($%02X),Y", pointer_tok->addr);
        write("\tSTA %d", global.addr);
        str_free(&pointer_name);
        return str_init("u8");
    }
    else
        quit("unary operator '%c' not supported\n", n);
}

str
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
        write("\tLDA #%s", d.value);
        write("\tSTA %d", global.addr);
        str_free(&d);
        type = str_init("u8");
    }
    else
    if(is_ident(next()))
    {
        str n = ident();
        token* tok = get_token(&n);
        copy(global.addr, tok->addr, tok->size);
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

void
operate(str* o)
{
    if(str_compare(o, "+") == 0)
    {
        write("\tCLC");
        write("\tLDA %d", global.addr - 2);
        write("\tADC %d", global.addr - 1);
        write("\tSTA %d", global.addr - 2);
    }
    else
    if(str_compare(o, "-") == 0)
    {
        write("\tSEC");
        write("\tLDA %d", global.addr - 2);
        write("\tSBC %d", global.addr - 1);
        write("\tSTA %d", global.addr - 2);
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

void
function_params(token* fun_tok)
{
    match('(');
    while(1)
    {
        if(next() == ')')
            break;
        str type = type_decal();
        str name = ident();
        size_t size = get_size(&type);
        param(fun_tok, token_init(type.value, name.value, size, size, global.addr, 0, 0));
        global.addr += size;
        str_free(&type);
        str_free(&name);
        if(next() == ',')
            match(',');
    }
    match(')');
}

void
deref(void)
{
    str name;
    str index_type = str_init("u8");
    size_t index_size = get_size(&index_type);
    // eg. *(p + 1);
    if(next() == '(')
    {
        match('(');
        name = ident();
        match('+');
        str type = expression();
        if(str_key_compare(&type, &index_type) != 0)
            quit("pointers may only be indexed by type '%s' but received type '%s'", index_type.value, type.value);
        str_free(&type);
        match(')');
    }
    // eg. *p;
    else
    {
        write("\tLDA #0");
        write("\tSTA %d", global.addr);
        name = ident();
    }
    token* pointer_tok = get_token(&name);
    str deref_type = deref_decal(&pointer_tok->type);
    token* deref_tok = get_token(&deref_type);
    // eg. *(p + 1) = expression; or *p = expression;
    if(next() == '=')
    {
        global.addr += index_size;
        match('=');
        str type_computed = expression();
        if(str_key_compare(&type_computed, &deref_type) != 0)
            quit("initializing type '%s' with computed type '%s'", deref_type.value, type_computed.value);
        size_t addr = pointer_tok->addr;
        write("\tLDA #0");
        // Implicit size offseting.
        for(size_t i = 0; i < deref_tok->size; i++)
        {
            write("\tCLC");
            write("\tADC %d", global.addr - index_size);
        }
        write("\tTAY");
        write("\tLDA %d", global.addr);
        write("\tSTA ($%02X),Y", addr);
        str_free(&type_computed);
        global.addr -= index_size;
    }
    // eg. *(p + 1) + expression; or *p + expression;
    else
    if(is_operator(next()))
    {
        pop();
        str type_computed = expression();
        if(str_key_compare(&type_computed, &deref_type) != 0)
            quit("incompatible type '%s' with computed type '%s'", deref_type.value, type_computed.value);
        str_free(&type_computed);
    }
    str_free(&deref_type);
    str_free(&name);
    str_free(&index_type);
}

void
value_assign(void)
{
    str name = ident();
    token* name_tok = get_token(&name);
    match('=');
    str type_computed = expression();
    str type = str_copy(&name_tok->type);
    if(str_key_compare(&type_computed, &type) != 0)
        quit("initializing type '%s' with computed type '%s'", type.value, type_computed.value);
    write("\tLDA %d", global.addr);
    write("\tSTA %d", name_tok->addr);
    str_free(&name);
    str_free(&type);
    str_free(&type_computed);
}

void
value_initialize(str* type, size_t type_size)
{
    str type_computed = expression();
    if(str_key_compare(&type_computed, type) != 0)
        quit("initializing type '%s' with computed type '%s'", type->value, type_computed.value);
    global.addr += type_size;
    str_free(&type_computed);
}

size_t
block_initialize(str* type, size_t type_size)
{
    if(is_pointer(type))
        quit("pointer arrays (see type '%s') are impossible to dereference", type->value);
    size_t width = 0;
    match('{');
    if(next() == '[')
    {
        match('[');
        str d = digit();
        width = atoi(d.value);
        for(size_t i = 0; i < width; i++)
        {
            write("\tLDA #0");
            write("\tSTA %d", global.addr);
            global.addr += type_size;
        }
        str_free(&d);
        match(']');
    }
    else
    while(1)
    {
        if(next() == '}')
            break;
        value_initialize(type, type_size);
        width += 1;
        if(next() == '}')
            break;
        match(',');
    }
    if(width == 0)
        quit("block initialization of type '%s' has width of '0'", type->value);
    match('}');
    return width;
}

void
initialize(void)
{
    str type = type_decal();
    size_t type_size = get_size(&type);
    str name = ident();
    token* name_tok = find(name);
    if(name_tok)
        quit("'%s' already defined: type: '%s', name '%s'", name.value, name_tok->type.value, name_tok->name.value);
    match('=');
    size_t addr = global.addr;
    size_t width = 0;
    if(next() == '{')
        width = block_initialize(&type, type_size);
    else
    {
        value_initialize(&type, type_size);
        width = 1;
    }
    var(token_init(type.value, name.value, type_size, width, addr, 0, 0));
    str_free(&type);
    str_free(&name);
}

void
general(void)
{
    str name = ident();
    int read = name.size > 0;
    if(read)
    {
        token* tok = get_token(&name);
        char n = next();
        str_append(&name, " ");
        prime(&name);
        // eg: a();
        // Note: Fuctions purposely have no return values, and therefore functions are not terms.
        if(n == '(')
        {
            if(!tok->is_fun)
                quit("token '%s' is not a function", tok->name.value);
            call();
        }
        // eg: a = expression;
        else
        if(n == '=')
            value_assign();
        // eg: u8 a = expression;
        else
        if(tok->is_type)
            initialize();
        // eg. a + expression;
        else
        {
            str type = expression();
            str_free(&type);
        }
    }
    // eg: expression;
    else
    {
        str type = expression();
        str_free(&type);
    }
    str_free(&name);
}

int
star_deref(void)
{
    if(next() == '*')
    {
        match('*');
        return 1;
    }
    return 0;
}

void
statement(void)
{
    if(star_deref())
        deref();
    else
        general();
}

void
block(void)
{
    size_t start = global.var_stack.size;
    match('{');
    while(1)
    {
        if(next() == '}')
            break;
        statement();
        match(';');
    }
    match('}');
    size_t end = global.var_stack.size;
    var_stack_pop(end - start);
}

void
function(void)
{
    global.addr = 0;
    str name = ident();
    if(find(name))
        quit("function '%s' already defined", name.value);
    write("%s:", name.value);
    var(token_init("void", name.value, 0, 0, 0, 1, 0));
    token* found = find(name);
    size_t start = global.var_stack.size;
    function_params(found);
    size_t end = global.var_stack.size;
    block();
    rts();
    var_stack_pop(end - start);
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
    var_stack_pop(global.var_stack.size);
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
        var(token_init(types[i].type, types[i].type, types[i].size, types[i].size, 0, 0, 1));
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
        " C(u8* p, u8 index, u8 value)    \n"
        " {                               \n"
        "     u8 a = 255;                 \n"
        "     u8 b = 255;                 \n"
        "     u8 c = 255;                 \n"
        "     *(p + index) = value;       \n"
        " }                               \n"
        " B(u8* p, u8 index, u8 value)    \n"
        " {                               \n"
        "     C(p, index, value);         \n"
        " }                               \n"
        " A(u8* p, u8 index, u8 value)    \n"
        " {                               \n"
        "     B(p, index, value);         \n"
        " }                               \n"
        " set(u8* p, u8 index, u8 value)  \n"
        " {                               \n"
        "     A(p, index, value);         \n"
        " }                               \n"
        " recurse()                       \n"
        " {                               \n"
        "     recurse();                  \n"
        " }                               \n"
        " main()                          \n"
        " {                               \n"
        "     u8 a  = 254;                \n"
        "     u8 b  = 254;                \n"
        "     u8 c  = 254;                \n"
        "     u8 d  = 254;                \n"
        "     u8 e = {                    \n"
        "         a + 1,                  \n"
        "         b + 1,                  \n"
        "         c + 1,                  \n"
        "     };                          \n"
        "     u8 f  = 255;                \n"
        "     set(&e, 1, 1);              \n"
        " }                               \n"
        "                                 \n");
    if(text.size != 0)
        compile(&text);
    str_free(&text);
}
