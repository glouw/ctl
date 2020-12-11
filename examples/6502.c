//
// -- A 6502 compiler specalizing in zero page 16-bit integer math and loop unroling --
//

#include <str.h>
#include <stdio.h>
#include <stdarg.h>

#define quit(msg, ...) { printf("error: internal %d: "msg, __LINE__, __VA_ARGS__); exit(1); }

#define P
#define T char
#include <stk.h>

#define T str
#include <lst.h>

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t addr;
    int is_type;
    int is_fun;
    int is_array;
}
token;

token
token_init(char* type, char* name, size_t size, size_t addr, int is_type, int is_fun, int is_array)
{
    return (token)
    {
        str_init(type),
        str_init(name),
        size,
        addr,
        is_type,
        is_fun,
        is_array,
    };
}

token
token_copy(token* self)
{
    return token_init(
            self->type.value,
            self->name.value,
            self->size,
            self->addr,
            self->is_type,
            self->is_fun,
            self->is_array);
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
    return str_key_compare(&a->name, &b->name);
}

#define T token
#include <set.h>

struct
{
    str comment;
    stk_char feed;
    set_token tokens;
    lst_str assem;
    lst_str variables;
    size_t addr;
    // Scoped brace matching.
    int l;
    int r;
}
global;

void
global_init(void)
{
    global.comment = str_init("");
    global.feed = stk_char_init();
    global.tokens = set_token_init(token_key_compare);
    global.assem = lst_str_init();
    global.variables = lst_str_init();
    global.addr = global.l = global.r = 0;
}

void
global_free(void)
{
    str_free(&global.comment);
    stk_char_free(&global.feed);
    set_token_free(&global.tokens);
    lst_str_free(&global.assem);
    lst_str_free(&global.variables);
}

token*
find(str* name)
{
    token key;
    key.name = *name;
    set_token_node* node;
    if((node = set_token_find(&global.tokens, key)))
        return &node->key;
    return NULL;
}

token*
get(str* name)
{
    token* tok = find(name);
    if(!tok)
        quit("token '%s' not defined", name->value);
    return tok;
}

void
insert(token tok)
{
    set_token_insert(&global.tokens, tok);
}

str
stringify(char* fmt, va_list args)
{
    va_list copy;
    va_copy(copy, args);
    str s = str_init("");
    size_t size = vsnprintf(NULL, 0, fmt, args);
    str_resize(&s, size + 1, '\0');
    vsprintf(s.value, fmt, copy);
    va_end(copy);
    return s;
}

str
format(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    str s = stringify(fmt, args);
    va_end(args);
    return s;
}

void
write(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    str s = stringify(fmt, args);
    va_end(args);
    lst_str_push_back(&global.assem, s);
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

int
is_digit(char c)
{
    return (c >= '0' && c <= '9') || c == '-' || c == '+';
}

int
is_operator(char c)
{
    return c == '+' || c == '-';
}

int
is_valid_operator(str* s)
{
    return str_compare(s, "+")
        || str_compare(s, "-");
}

int
is_ident(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || is_digit(c);
}

int
is_macro_token(char c)
{
    return c == '$' || is_ident(c);
}

int
is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int
is_scoped(char c)
{
    int diff = global.l - global.r;
    if(c == '}' && diff == 0)
    {
        global.l = 0;
        global.r = 0;
        return 0;
    }
    if(c == '{') global.l += 1;
    if(c == '}') global.r += 1;
    return 1;
}

void
tidy(str* s)
{
    while(!str_empty(s) && is_space(*str_back(s)))
        str_pop_back(s);
}

char
top(void)
{
    if(stk_char_empty(&global.feed))
        quit("<< INTERNAL COMPILER ERROR >> Accessed global feed of size '%lu'", global.feed.size);
    return *stk_char_top(&global.feed);
}

void
pop(void)
{
    char c = top();
    if(c == '\n')
    {
        tidy(&global.comment);
        if(!str_empty(&global.comment))
            write("; %s", global.comment.value);
        str_clear(&global.comment);
    }
    else
        str_push_back(&global.comment, c);
    stk_char_pop(&global.feed);
}

int
next(void)
{
    char c;
    while(is_space(c = top()))
    {
        pop();
        if(stk_char_empty(&global.feed))
            return EOF;
    }
    return c;
}

str
buffer(int clause(char c))
{
    str s = str_init("");
    char c;
    while(clause(c = top()))
    {
        str_push_back(&s, c);
        pop();
    }
    return s;
}

str
read(int clause(char c))
{
    next();
    return buffer(clause);
}

str
digit(void)
{
    if(!is_digit(next()))
        quit("expected digit; received '%c'", next());
    return read(is_digit);
}

str
ident(void)
{
    if(is_digit(next()))
        quit("expected identifier; received '%c'", next());
    return read(is_ident);
}

str
macro_token(void)
{
    if(next() != '$')
        quit("expected identifier; received '%c'", next());
    return read(is_macro_token);
}

str
operator(void)
{
    if(!is_operator(next()))
        quit("expected operator; received '%c'", next());
    return read(is_operator);
}

void
match(char c)
{
    if(next() != c)
        quit("expected '%c' but received '%c'", c, next());
    pop();
}

void
declare(char* type, char* name, size_t size, int is_fun, int is_array)
{
    size_t addr = is_fun ? 0 : global.addr;
    insert(token_init(type, name, size, addr, 0, is_fun, is_array));
    lst_str_push_back(&global.variables, str_init(name));
    if(!is_fun)
        global.addr += size;
}

void
keyword(char* name, size_t size, int is_type)
{
    insert(token_init(name, name, size, 0, is_type, 0, 0));
    lst_str_push_back(&global.variables, str_init(name));
}

void
copy(size_t from, size_t to, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        write("\tLDA %d", i + from);
        write("\tSTA %d", i + to);
    }
}

void
load_value(token* tok)
{
    copy(tok->addr, global.addr, tok->size);
    global.addr += tok->size;
}

void
load_digit(void)
{
    str d = digit();
    int value = atoi(d.value);
    write("\tLDA #%d", (value >> 0) & 0xFF), write("\tSTA %d", global.addr + 0);
    write("\tLDA #%d", (value >> 8) & 0xFF), write("\tSTA %d", global.addr + 1);
    str_free(&d);
    global.addr += 2;
}

void
add(void)
{
    global.addr -= 2;
    write("\tCLC");
    write("\tLDA %d", global.addr - 2);
    write("\tADC %d", global.addr - 0);
    write("\tSTA %d", global.addr - 2);
    write("\tLDA %d", global.addr - 1);
    write("\tADC %d", global.addr + 1);
    write("\tSTA %d", global.addr - 1);
}

void
sub(void)
{
    global.addr -= 2;
    write("\tSEC");
    write("\tLDA %d", global.addr - 2);
    write("\tSBC %d", global.addr - 0);
    write("\tSTA %d", global.addr - 2);
    write("\tLDA %d", global.addr - 1);
    write("\tSBC %d", global.addr + 1);
    write("\tSTA %d", global.addr - 1);
}

token
resolve(token* tok)
{
    token copy = token_copy(tok);
    if(next() == '[')
    {
        token* type = find(&tok->type);
        match('[');
        str d = digit();
        int index = atoi(d.value);
        copy.size = type->size;
        copy.is_array = type->is_array;
        copy.addr += index * type->size;
        match(']');
        str_free(&d);
    }
    return copy;
}

void
expression(void);

void
term(void)
{
    if(next() == '(')
    {
        match('(');
        expression();
        match(')');
    }
    else
    if(is_digit(next()))
        load_digit();
    else
    if(is_ident(next()))
    {
        str n = ident();
        token* tok = get(&n);
        token res = resolve(tok);
        if(res.is_type)
            quit("type '%s' cannot be loaded", res.name.value);
        if(res.is_fun)
            quit("function '%s' cannot be loaded", res.name.value);
        if(res.is_array)
            quit("array '%s' cannot be loaded", res.name.value);
        load_value(&res);
        str_free(&n);
        token_free(&res);
    }
    else
        quit("unknown character in term '%c'\n", next());
}

int
end_of_expression(char c)
{
    return c == ';' || c == ')';
}

void
terms(void)
{
    while(!end_of_expression(next()))
    {
        str o = operator();
        term();
        if(str_compare(&o, "+") == 0)
            add();
        else
        if(str_compare(&o, "-") == 0)
            sub();
        else
            quit("unknown operator '%s'", o.value);
        str_free(&o);
    }
}

void
expression(void)
{
    term();
    terms();
}

void
assign(token* tok)
{
    match('=');
    expression();
    global.addr -= tok->size;
    copy(global.addr, tok->addr, tok->size);
}

void
declare_array(char* type, char* name, size_t size)
{
    match('[');
    str d = digit();
    size_t width = atoi(d.value);
    str_free(&d);
    match(']');
    declare(type, name, width * size, 0, 1);
}

void
declarations(token* tok)
{
    str name = ident();
    if(next() == '[')
        declare_array(tok->type.value, name.value, tok->size);
    else
        declare(tok->type.value, name.value, tok->size, 0, 0);
    str_free(&name);
}

void
index(token* tok)
{
    token res = resolve(tok);
    if(next() == '=')
        assign(&res);
    else
    if(is_operator(next()))
    {
        pop();
        expression();
    }
    token_free(&res);
}

void
general(str* lead)
{
    token* tok = get(lead);
    if(tok->is_type)
        declarations(tok);
    else
    {
        if(next() == '[')
            index(tok);
        else
        if(next() == '=')
            assign(tok);
        else
        {
            load_value(tok);
            terms();
        }
    }
}

void
prime(str* text)
{
    while(text->size)
    {
        char c = *str_back(text);
        stk_char_push(&global.feed, c);
        str_pop_back(text);
    }
}

void
replace(str* s, str* macro, str* with)
{
    size_t index = 0;
    while((index = str_find(s, macro->value)) != SIZE_MAX)
        str_replace(s, index, macro->size, with->value);
}

void
unroll_for(void)
{
    match('(');
    str reftok = macro_token();
    str enumtok = str_init("");
    int enumming = 0;
    if(next() == ',')
    {
        enumming = 1;
        match(',');
        str temp = macro_token();
        str_swap(&enumtok, &temp);
        str_free(&temp);
    }
    match(':');
    str array = ident();
    token* tok = get(&array);
    token* type = get(&tok->type);
    size_t size = tok->size / type->size;
    match(')');
    match('{');
    str meta = buffer(is_scoped);
    puts(meta.value);
    lst_str expanded = lst_str_init();
    for(size_t i = 0; i < size; i++)
    {
        str temp = str_copy(&meta);
        if(enumming)
        {
            str enumer = format("%d", i);
            replace(&temp, &enumtok, &enumer);
            str_free(&enumer);
        }
        str reffer = format("%s[%d]", array.value, i);
        replace(&temp, &reftok, &reffer);
        lst_str_push_front(&expanded, str_copy(&temp));
        str_free(&reffer);
        str_free(&temp);
    }
    match('}');
    foreach(lst_str, &expanded, it,
        prime(it.ref);
    )
    lst_str_free(&expanded);
    str_free(&meta);
    str_free(&reftok);
    str_free(&enumtok);
    str_free(&array);
}

void
inline_asm(void)
{
    match('{');
    str assem = buffer(is_scoped);
    write("%s", assem.value);
    match('}');
    str_free(&assem);
}

void
statement(void)
{
    if(is_digit(next()) || next() == '(')
        expression();
    else
    {
        str lead = ident();
        if(str_compare(&lead, "for") == 0)
            unroll_for();
        else
        if(str_compare(&lead, "asm") == 0)
            inline_asm();
        else
        {
            general(&lead);
            match(';');
        }
        str_free(&lead);
    }
}

void
pop_locals(size_t size)
{
    lst_str reversed = lst_str_init();
    while(size)
    {
        str* local = lst_str_back(&global.variables);
        lst_str_push_front(&reversed, str_copy(local));
        lst_str_pop_back(&global.variables);
        size -= 1;
    }
    write("; %8s %8s %6s %6s %6s %6s %6s", "TYPE", "NAME", "SIZE", "ADDR", "T?", "F?", "A?");
    foreach(lst_str, &reversed, it,
        token* tok = get(it.ref);
        write("; %8s %8s %6d %6d %6d %6d %6d",
            tok->type.value, tok->name.value, tok->size, tok->addr, tok->is_type, tok->is_fun, tok->is_array);
        global.addr -= tok->size;
    )
    lst_str_free(&reversed);
}

void
block(void)
{
    match('{');
    size_t v0 = global.variables.size;
    while(next() != '}')
    {
        if(next() == '{')
            block();
        else
            statement();
    }
    size_t v1 = global.variables.size;
    pop_locals(v1 - v0);
    match('}');
}

void
function(void)
{
    str name = ident();
    declare("void", name.value, 0, 1, 0);
    write("%s:", name.value);
    str_free(&name);
    block();
    write("\tRTS");
}

void
program(void)
{
    write("JMP main");
    function();
    pop_locals(global.variables.size);
}

void
setup(void)
{
    keyword("int", 2, 1);
}

void
compile(char* code)
{
    global_init();
    setup();
    str text = str_init(code);
    prime(&text);
    program();
    save();
    str_free(&text);
    global_free();
}

int
main(void)
{
    compile(
        "main                       \n"
        "{                          \n"
        "    int  x[8];             \n"
        "    int  y[8];             \n"
        "    int vx[8];             \n"
        "    int vy[8];             \n"
        "    for($a, $i : x)        \n"
        "    {                      \n"
        "        $a = 1;            \n"
        "    }                      \n"
        "}                          \n"
    );
}
