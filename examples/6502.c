//
// -- SIMPLE 6502 COMPILER --
//

#include <stdarg.h>
#include <stdio.h>
#include <str.h>

#define P
#define T char
#include <deq.h>

#define T str
#include <vec.h>

#define T str
#include <lst.h>

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t addr;
    int is_function;
    vec_str args;
}
token;

token
token_copy(token* self)
{
    return (token)
    {
        str_copy(&self->type),
        str_copy(&self->name),
        self->size,
        self->addr,
        self->is_function,
        vec_str_copy(&self->args),
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
    return strcmp(a->name.value, b->name.value);
}

token
identifier(char* type, char* name, size_t size, size_t addr)
{
    token tok = {
        str_init(type),
        str_init(name),
        size,
        addr,
        0,
        vec_str_init(),
    };
    return tok;
}

token
keyword(char* name, size_t size)
{
    return identifier(name, name, size, 0);
}

#define T token
#include <set.h>

struct
{
    size_t line;
    size_t column;
    size_t addr;
    str function;
    vec_str stack;
    deq_char feed;
    set_token tokens;
    lst_str assem;
    str comment;
    FILE* save;
}
global;

void
global_init(char* path)
{
    global.line = 0;
    global.column = 0;
    global.addr = 0x0;
    global.function = str_init("");
    global.stack = vec_str_init();
    global.feed = deq_char_init();
    global.tokens = set_token_init(token_key_compare);
    global.assem = lst_str_init();
    global.comment = str_init("");
    global.save = path ? fopen(path, "w") : stdout;
}

void
global_free(void)
{
    str_free(&global.function);
    vec_str_free(&global.stack);
    deq_char_free(&global.feed);
    set_token_free(&global.tokens);
    lst_str_free(&global.assem);
    str_free(&global.comment);
    fclose(global.save);
}

#define quit(message, ...) { \
    printf("error: line %lu: column %lu: %s line %d: "message"\n", \
            global.line, global.column, __FILE__, __LINE__, __VA_ARGS__); \
    exit(1); \
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

token*
find(str* name)
{
    token key;
    key.name = *name;
    set_token_node* node = set_token_find(&global.tokens, key);
    if(node)
        return &node->key;
    return NULL;
}

#define W "12"

void
info_header(void)
{
    write("; %"W"s %"W"s %"W"s %"W"s %"W"s%"W"s", "NAME", "VALUE", "SIZE", "ADDR", "IS FN", "FN ARGS");
}

void
global_info(void)
{
    write("; GLOBAL INFO");
    info_header();
    foreach(set_token, &global.tokens, it,
        write("; %"W"s %"W"s %"W"lu %"W"lu %"W"lu%"W"lu",
                it.ref->name.value,
                it.ref->type.value,
                it.ref->size,
                it.ref->addr,
                it.ref->is_function,
                it.ref->args.size);
    )
}

void
stack_info(void)
{
    if(!vec_str_empty(&global.stack))
    {
        write("; STACK INFO");
        info_header();
        foreach(vec_str, &global.stack, it,
        {
            token* tok = find(it.ref);
            write("; %"W"s %"W"s %"W"lu %"W"lu %"W"lu%"W"lu",
                    tok->name.value,
                    tok->type.value,
                    tok->size,
                    tok->addr,
                    tok->is_function,
                    tok->args.size);
        })
    }
}

void
dump(void)
{
    foreach(lst_str, &global.assem, it,
        fprintf(global.save, "%s\n", it.ref->value);)
}

void
label(str* name)
{
    write("%s:", name->value);
}

void
rts(void)
{
    write("\tRTS");
}

void
load_digit(char* d)
{
    write("\tLDA #%3s", d);
    write("\tSTA  %3d", global.addr);
}

void
load_ident(token* tok)
{
    write("\tLDA  %3d", tok->addr);
    write("\tSTA  %3d", global.addr);
}

void
load_dereference(token* tok)
{
    write("\tLDX  %3d", tok->addr);
    write("\tLDA  $0,X");
    write("\tSTA  %3d", global.addr);
}

void
load_addr(token* tok)
{
    write("\tLDA #%3d", tok->addr);
    write("\tSTA  %3d", global.addr);
}

void
assign(token* tok)
{
    write("\tLDA  %3d", global.addr);
    write("\tSTA  %3d", tok->addr);
}

void
assign_dereference(token* tok)
{
    write("\tLDA  %3d", global.addr);
    write("\tLDX  %3d", tok->addr);
    write("\tSTA  $0,X");
}

void
push_argument(void)
{
    write("\tLDA  %3d", global.addr);
    write("\tPHA");
}

void
adc(void)
{
    write("\tCLC");
    write("\tLDA  %3d", global.addr - 1);
    write("\tADC  %3d", global.addr - 2);
    write("\tSTA  %3d", global.addr - 2);
}

void
sbc(void)
{
    write("\tSEC");
    write("\tLDA  %3d", global.addr - 2);
    write("\tSBC  %3d", global.addr - 1);
    write("\tSTA  %3d", global.addr - 2);
}

void
erase(str* name)
{
    token key;
    key.name = *name;
    set_token_erase(&global.tokens, key);
}

void
insert(token t)
{
    set_token_insert(&global.tokens, t);
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

char
front_feed(void)
{
    return *deq_char_front(&global.feed);
}
void
pop_feed(void)
{
    global.column += 1;
    char c = front_feed();
    if(c !='\n')
        str_push_back(&global.comment, c);
    deq_char_pop_front(&global.feed);
}

void
push_feed(char c)
{
    global.column -= 1;
    deq_char_push_front(&global.feed, c);
    str_pop_back(&global.comment);
}

int
end_feed(void)
{
    return deq_char_empty(&global.feed);
}

void
space(void)
{
    for(char c; !end_feed() && is_space(c = front_feed());)
    {
        if(c == '\n')
        {
            global.line += 1;
            global.column = 0;
            while(!str_empty(&global.comment))
            {
                char back = *str_back(&global.comment);
                if(!is_space(back))
                    break;
                str_pop_back(&global.comment);
            }
            write("; %s", global.comment.value);
            str_clear(&global.comment);
        }
        pop_feed();
    }
}

char
next(void)
{
    space();
    return front_feed();
}

void
match(char c)
{
    if(next() != c)
        quit("expected '%c' but got '%c'\n", c, next());
    pop_feed();
}

str
read(int pred(char))
{
    space();
    str s = str_init("");
    for(char c; pred(c = front_feed());)
    {
        str_push_back(&s, c);
        pop_feed();
    }
    return s;
}

str
type(void)
{
    str ident = read(is_ident);
    if(next() == '&')
    {
        str_push_back(&ident, '&');
        pop_feed();
    }
    return ident;
}

int
is_reference(str* t)
{
    return *str_back(t) == '&';
}

char*
infer_type(str* t)
{
    char* type;
    if(is_digit(t->value[0]))
        type = "u8";
    else
    {
        str copy = str_copy(t);
        int ref = is_reference(&copy);
        if(ref)
            str_pop_back(&copy);
        token* tok = find(&copy);
        str_free(&copy);
        if(tok)
        {
            if(ref)
            {
                str temp = str_copy(&tok->type);
                str_append(&temp, "&");
                token* ref_tok = find(&temp);
                if(ref_tok)
                    type = ref_tok->type.value;
                else
                    quit("type '%s' not defined", temp.value);
                str_free(&temp);
            }
            else
                type = tok->type.value;
        }
        else
            quit("type '%s' not defined", t->value);
    }
    return type;
}

str
expression(void);

void
call_params(token* tok)
{
    match('(');
    size_t size = 0;
    while(1)
    {
        if(next() == ')')
            break;
        size += 1;
        if(size > tok->args.size)
            break;
        str t = expression();
        char* tt = infer_type(&t);
        char* uu = infer_type(&tok->args.value[size - 1]);
        if(strcmp(tt, uu) != 0)
            quit("type mismatch; types are '%s' and '%s'", tt, uu);
        push_argument(); // XXX. NEEDS TO ACCOUNT FOR TYPE SIZE.
        if(next() == ',')
            match(',');
        str_free(&t);
    }
    if(size != tok->args.size)
        quit("function '%s' requires '%lu' argument%s but received '%lu' argument%s",
                tok->name.value,
                tok->args.size,
                tok->args.size == 1 ? "" : "s",
                size,
                size == 1 ? "" : "s");
    match(')');
}

void
call(token* tok)
{
    call_params(tok);
    write("\tJSR  %s\n", tok->name.value);
}

str
reference(void)
{
    match('&');
    if(is_digit(next()))
    {
        str d = read(is_digit);
        quit("digit '%s' may not be referenced; only identifiers may be referenced", d.value);
        str_free(&d);
    }
    else
    if(is_ident(next()))
    {
        str ref = read(is_ident);
        token* tok = find(&ref);
        if(!tok)
            quit("identifier '%s' not defined with attempted reference", ref.value);
        load_addr(tok);
        str_append(&ref, "&");
        return ref;
    }
    else
        quit("character '%c' cannot follow reference operator; an identifier must be supplied", next());
}

str
unary(void)
{
    char u = next();
    if(u == '&')
        return reference();
    else
        quit("unary operator '%c' not supported", u);
}

size_t
infer_offset(str* t)
{
    if(is_reference(t))
        return 1;
    else
    {
        char* tt = infer_type(t);
        str temp = str_init(tt);
        token* tok = find(&temp);
        if(!tok)
            quit("type %s not defined\n", tt);
        str_free(&temp);
        return tok->size;
    }
}

str
term(void)
{
    str out;
    if(next() == '(')
    {
        match('(');
        out = expression();
        match(')');
    }
    else
    if(is_digit(next()))
    {
        out = read(is_digit);
        load_digit(out.value);
    }
    else
    if(is_ident(next()))
    {
        out = read(is_ident);
        token* tok = find(&out);
        if(!tok)
            quit("identifier '%s' not defined", out.value);
        if(tok->is_function)
            call(tok);
        else
        {
            if(is_reference(&tok->type))
                load_dereference(tok);
            else
                load_ident(tok);
        }
    }
    else
        out = unary();
    global.addr += infer_offset(&out);
    return out;
}

int
is_compatible(char* tt, char* uu)
{
    str t = str_init(tt);
    str u = str_init(uu);
    if(is_reference(&t))
        str_pop_back(&t);
    if(is_reference(&u))
        str_pop_back(&u);
    int compatible = str_compare(&t, u.value) == 0;
    str_free(&t);
    str_free(&u);
    return compatible;
}

void
operate(str* o)
{
    if(str_compare(o, "+") == 0)
        adc();
    else
    if(str_compare(o, "-") == 0)
        sbc();
    global.addr -= 1;
}

int
can_operate(char* t)
{
    return strcmp(t, "u8") == 0 || strcmp(t, "u8&") == 0;
}

str
expression(void)
{
    str t = term();
    char* tt = infer_type(&t);
    while(1)
    {
        if(next() == ')')
            break;
        if(next() == ';')
            break;
        if(next() == ',')
            break;
        str o = read(is_operator);
        if(str_empty(&o))
            quit("missing operator; previous term was '%s'", t.value);
        if(!valid_operator(&o))
            quit("invalid operator '%s'", o.value);
        str u = term();
        char* uu = infer_type(&u);
        if(!is_compatible(tt, uu))
            quit("type mismatch; types are '%s' and '%s'", tt, uu);
        // If one operand is a reference, the expression promotes to a reference.
        if(is_reference(&u))
            tt = uu;
        if(!can_operate(tt)) quit("operator '%s' cannot operate on type '%s'\n", o.value, tt);
        if(!can_operate(uu)) quit("operator '%s' cannot operate on type '%s'\n", o.value, uu);
        operate(&o);
        str_free(&u);
        str_free(&o);
    }
    str_free(&t);
    global.addr -= 1;
    return str_init(tt);
}

void
evaluate(str* ident)
{
    if(next() == ';')
        quit("expression may not be empty; see identifier '%s'", ident->value);
    str type = expression();
    token* defined = find(&type);
    if(!defined)
        quit("type '%s' not defined", type.value);
    token* exists = find(ident);
    if(exists)
    {
        if(is_reference(&exists->type))
        {
            if(is_reference(&type))
                quit("existing references may not be assigned a new reference; see '%s'", exists->name.value);
            assign_dereference(exists);
        }
        else
        {
            if(is_reference(&type))
                quit("existing values may not be casted to references; see '%s'", exists->name.value);
            assign(exists);
        }
    }
    else
    {
        vec_str_push_back(&global.stack, str_copy(ident));
        insert(identifier(type.value, ident->value, defined->size, global.addr));
        global.addr += infer_offset(&type);
    }
    str_free(&type);
}

void
reverse(str* ident)
{
    while(!str_empty(ident))
    {
        push_feed(*str_back(ident));
        str_pop_back(ident);
    }
}

void
fallback(str* ident)
{
    reverse(ident);
    if(next() == ';')
        quit("expressions may not be empty; see identifier '%s'", ident->value);
    str type = expression();
    str_free(&type);
}

void
ret(void)
{
    token* tok = find(&global.function);
    if(str_compare(&tok->type, "void") != 0)
    {
        if(next() == ';')
            quit("empty return statement in function '%s' requires type '%s'", global.function.value, tok->type.value);
        str type = expression();
        if(str_compare(&tok->type, type.value) != 0)
            quit("computed return statement in function '%s' is '%s' but expected '%s'", global.function.value, type.value, tok->type.value);
        str_free(&type);
    }
    else
    if(next() != ';')
        quit("void function '%s' may not compute a return expression", global.function.value);
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
                quit("assignments may not be empty; see identifier '%s'", ident.value);
            evaluate(&ident);
        }
        else
        if(str_compare(&ident, "return") == 0)
            ret();
        else
            fallback(&ident);
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

vec_str
function_params(void)
{
    match('(');
    vec_str args = vec_str_init();
    while(1)
    {
        if(next() == ')')
            break;
        str t = type();
        str n = read(is_ident);
        if(find(&n))
            quit("function argument names must be unique; see '%s'", n.value);
        token* tok = find(&t);
        if(!tok)
            quit("unknown type '%s' in function paramater list", t.value);
        vec_str_push_back(&global.stack, str_copy(&n));
        insert(identifier(t.value, n.value, tok->size, global.addr));
        global.addr += tok->size;
        vec_str_push_back(&args, str_copy(&t));
        str_free(&t);
        str_free(&n);
        if(next() == ',')
            match(',');
    }
    match(')');
    return args;
}

token
function_sign(vec_str* args)
{
    token sig;
    if(next() == '-')
    {
        match('-');
        match('>');
        str t = type();
        token* tok = find(&t);
        if(!tok)
            quit("unknown function return type '%s'", t.value);
        sig = identifier(t.value, global.function.value, tok->size, 0);
        str_free(&t);
    }
    else
        sig = identifier("void", global.function.value, 0, 0);
    sig.is_function = 1;
    sig.args = *args;
    return sig;
}

void
unpack_params(size_t size)
{
    write("\tPLA");
    write("\tTAX");
    write("\tPLA");
    write("\tTAY");
    while(size > 0)
    {
        size -= 1;
        write("\tPLA");
        write("\tSTA  %d", size);
    }
    write("\tTYA");
    write("\tPHA");
    write("\tTXA");
    write("\tPHA");
}

void
function(void)
{
    global.addr = 0;
    str name = read(is_ident);
    label(&name);
    str_swap(&name, &global.function);
    str_free(&name);
    if(find(&global.function))
        quit("function '%s' already defined", global.function.value);
    vec_str args = function_params();
    token sig = function_sign(&args);
    insert(sig);
    unpack_params(args.size);
    block();
    stack_info();
    while(!vec_str_empty(&global.stack))
    {
        erase(vec_str_back(&global.stack));
        vec_str_pop_back(&global.stack);
    }
    rts();
}

void
program(void)
{
    write("JMP main");
    while(1)
    {
        function();
        space();
        if(end_feed())
            break;
    }
    global_info();
}

void
table(void)
{
    struct { char* name; size_t size; } pair[] = {
        { "u8",     1 },
        { "u8&",    1 },
        { "return", 0 },
        { "void",   0 },
    };
    for(size_t i = 0; i < len(pair); i++)
        insert(keyword(pair[i].name, pair[i].size));
}

void
queue(char* text)
{
    str code = str_init(text);
    for(size_t i = 0; i < code.size; i++)
        deq_char_push_back(&global.feed, code.value[i]);
    str_free(&code);
}

void
compile(char* text)
{
    global_init("out.asm");
    table();
    queue(text);
    program();
    dump();
    global_free();
}

int
main(void)
{
    compile(
        "ref(u8& c)                \n"
        "{                         \n"
        "   c = 9;                 \n"
        "}                         \n"
        "main()                    \n"
        "{                         \n"
        "    a = 4;                \n"
        "    b = 5;                \n"
        "    c = 6;                \n"
        "    d = 7;                \n"
        "    ref(&a);              \n"
        "}                         \n");
}
