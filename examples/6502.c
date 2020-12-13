//
// -- A 6502 compiler specalizing in brute force zero page 16-bit integer math and loop unrolling --
//

#include <str.h>
#define str_equal(a, b) (str_compare(a, b) == 0)

#include <stdio.h>
#include <stdarg.h>

#define quit(msg, ...) { printf("error: internal %d: "msg, __LINE__, __VA_ARGS__); exit(1); }

#define DEFAULT_WORD_SIZE (2)

#define P
#define T char
#include <stk.h>

#define T str
#include <lst.h>

#define LIST X(NONE) X(TYPE) X(FUN) X(ARRAY) X(VAR)

typedef enum
{
#define X(name) name,
    LIST
#undef X
}
family;

const char* lookup[] = {
#define X(name) #name,
    LIST
#undef X
};

typedef struct
{
    str type;
    str name;
    size_t size;
    size_t addr;
    family fam;
}
token;

token
token_init(char* type, char* name, size_t size, size_t addr, family fam)
{
    return (token) {
        str_init(type),
        str_init(name),
        size,
        addr,
        fam,
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
        self->fam
    );
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
    size_t label;
    struct
    {
        int l;
        int r;
    }
    brace;
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
    global.addr = global.label = global.brace.l = global.brace.r = 0;
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

void
erase(str* name)
{
    token key;
    key.name = *name;
    set_token_erase(&global.tokens, key);
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
    lst_str_push_back(&global.variables, str_copy(&tok.name));
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
        puts(s);
        fprintf(out, "%s\n", s);
    )
    fclose(out);
}

int
is_unary(char c)
{
    return c == '!'
        || c == '-';
}

int
is_digit(char c)
{
    return (c >= '0' && c <= '9') || c == '-' || c == '+';
}

int
is_operator(char c)
{
    return c == '+'
        || c == '-'
        || c == '='
        || c == '!'
        || c == '<'
        || c == '>'
        || c == '&'
        || c == '|'
        || c == '^';
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
is_macro_token(char c)
{
    return c == '$'
        || is_ident(c);
}

int
is_space(char c)
{
    return c == ' '
        || c == '\t'
        || c == '\n'
        || c == '\r';
}

int
is_scoped(char c)
{
    int diff = global.brace.l - global.brace.r;
    if(c == '}' && diff == 0)
    {
        global.brace.l = 0;
        global.brace.r = 0;
        return 0;
    }
    if(c == '{') global.brace.l += 1;
    if(c == '}') global.brace.r += 1;
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

token
resolve(token* tok)
{
    token copy = token_copy(tok);
    if(next() == '[')
    {
        token* type = find(&tok->type);
        copy.size = type->size;
        copy.fam = VAR;
        match('[');
        str d = digit();
        copy.addr += atoi(d.value) * type->size;
        str_free(&d);
        match(']');
    }
    return copy;
}

void
declare(char* type, char* name, size_t size, size_t addr, family fam)
{
    str temp = str_init(name);
    token* exists = find(&temp);
    if(exists)
        quit("token '%s' already defined", exists->name.value);
    str_free(&temp);
    insert(token_init(type, name, size, addr, fam));
}

void
def_fun(char* name)
{
    insert(token_init("void", name, 0, 0, FUN));
}

void
keyword(char* name)
{
    insert(token_init(name, name, 0, 0, NONE));
}

void
keytype(char* name, size_t size)
{
    insert(token_init(name, name, size, 0, TYPE));
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
set(size_t addr, int n)
{
    write("\tLDA #%d", (n >> 0) & 0xFF), write("\tSTA %d", addr + 0);
    write("\tLDA #%d", (n >> 8) & 0xFF), write("\tSTA %d", addr + 1);
}

void
load_value(token* tok)
{
    if(tok->fam == ARRAY)
    {
        copy(tok->addr, global.addr, tok->size);
        global.addr += tok->size;
    }
    else
    {
        set(global.addr, 0); // Such that u8 does not get garbage MSB.
        copy(tok->addr, global.addr, tok->size);
        global.addr += DEFAULT_WORD_SIZE;
    }
}

void
load_digit_internal(int value)
{
    set(global.addr, value);
    global.addr += DEFAULT_WORD_SIZE;
}

void
load_digit(int negate)
{
    str integer = str_init("i16");
    get(&integer);
    str_free(&integer);
    str d = digit();
    int value = atoi(d.value);
    if(negate)
        value *= -1;
    load_digit_internal(value);
    str_free(&d);
}

void
apply(char* method)
{
    for(size_t i = 0; i < DEFAULT_WORD_SIZE; i++)
    {
        write("\tLDA %d", global.addr + i - DEFAULT_WORD_SIZE);
        write("\t%s %d", method, global.addr + i);
        write("\tSTA %d", global.addr + i - DEFAULT_WORD_SIZE);
    }
}

void
or(void)
{
    apply("ORA");
}

void
and(void)
{
    apply("AND");
}

void
xor(void)
{
    apply("EOR");
}

void
add(void)
{
    write("\tCLC");
    apply("ADC");
}

void
sub(void)
{
    write("\tSEC");
    apply("SBC");
}

void
not(void)
{
    write("\tLDA #1");
    write("\tEOR %d", global.addr - DEFAULT_WORD_SIZE);
    write("\tSTA %d", global.addr - DEFAULT_WORD_SIZE);
}

family
expression(void);

family
term(void)
{
    family fam = NONE;
    if(is_unary(next()))
    {
        // eg. !a or !1
        if(next() == '!')
        {
            match('!');
            fam = term();
            not();
        }
        else
        if(next() == '-')
        {
            match('-');
            if(is_digit(next()))
            {
                load_digit(1);
                fam = VAR;
            }
            else
            {
                load_digit_internal(0);
                fam = term();
                global.addr -= DEFAULT_WORD_SIZE;
                sub();
            }
        }
    }
    // eg. (a + 1)
    else
    if(next() == '(')
    {
        match('(');
        fam = expression();
        match(')');
    }
    // eg. 1
    else
    if(is_digit(next()))
    {
        load_digit(0);
        fam = VAR;
    }
    // eg. a
    else
    if(is_ident(next()))
    {
        str n = ident();
        token* tok = get(&n);
        token res = resolve(tok);
        fam = res.fam;
        if(fam == TYPE)
            quit("type '%s' cannot be loaded", res.name.value);
        if(fam == FUN)
            quit("function '%s' cannot be loaded", res.name.value);
        load_value(&res);
        str_free(&n);
        token_free(&res);
    }
    else
        quit("unknown character in term '%c'\n", next());
    return fam;
}

int
end_of_expression(char c)
{
    return c == ';' || c == ')';
}

void
compare(char* method, int x, int y)
{
    size_t a = global.label + 0;
    size_t b = global.label + 1;
    global.label += 2;
    size_t i = DEFAULT_WORD_SIZE;
    while(i)
    {
        i -= 1;
        write("\tLDA %d", global.addr + i - DEFAULT_WORD_SIZE);
        write("\tCMP %d", global.addr + i);
        write("\t%s L%d\n", method, a);
    }
    write("\tBNE L%d\n", a);
    set(global.addr - DEFAULT_WORD_SIZE, x);
    write("\tJMP L%d\n", b);
    write("L%d:\n", a);
    set(global.addr - DEFAULT_WORD_SIZE, y);
    write("L%d:\n", b);
}

void
equal(void)
{
    compare("BNE", 1, 0);
}

void
less_than(void)
{
    compare("BCC", 0, 1);
}

void
greater_than(void)
{
    compare("BNE", 0, 1);
}

void
greater_than_or_equal(void)
{
    less_than();
    not();
}

void
less_than_or_equal(void)
{
    greater_than();
    not();
}

family
expression(void)
{
    family fam = term();
    while(!end_of_expression(next()))
    {
        family next = NONE;
        str o = operator();
        if(str_equal(&o, "=="))
        {
            // eg. a == 1
            next = expression();
            global.addr -= DEFAULT_WORD_SIZE;
            equal();
        }
        else
        {
            next = term();
            global.addr -= DEFAULT_WORD_SIZE;
            // eg. a | 1
            if(str_equal(&o, "|"))
                or();
            // eg. a & 1
            else
            if(str_equal(&o, "&"))
                and();
            // eg. a ^ 1
            else
            if(str_equal(&o, "^"))
                xor();
            // eg. a + 1
            else
            if(str_equal(&o, "+"))
                add();
            // eg. a - 1
            else
            if(str_equal(&o, "-"))
                sub();
            // eg. a < 1
            else
            if(str_equal(&o, "<"))
                less_than();
            // eg. a > 1
            else
            if(str_equal(&o, ">"))
                greater_than();
            // eg. a <= 1
            else
            if(str_equal(&o, "<="))
                less_than_or_equal();
            // eg. a >= 1
            else
            if(str_equal(&o, ">="))
                greater_than_or_equal();
            else
                quit("unknown operator '%s'", o.value);
        }
        if(fam != VAR || next != VAR)
            quit("operators only support variables (received '%s' and '%s')\n", lookup[fam], lookup[next]);
        fam = next;
        str_free(&o);
    }
    return fam;
}

void
declare_array(char* type, char* name, size_t size)
{
    match('[');
    // Array sizes are constants to facilitate fast zero page loads with loop unrolling.
    str d = digit();
    size_t width = atoi(d.value);
    str_free(&d);
    match(']');
    declare(type, name, width * size, global.addr, ARRAY);
    global.addr += width * size;
}

void
assign(token* tok)
{
    match('=');
    printf("A %d\n", global.addr);
    family fam = expression();
    printf("B %d\n", global.addr);
    if(fam == ARRAY)
        global.addr -= tok->size;
    else
        global.addr -= DEFAULT_WORD_SIZE;
    copy(global.addr, tok->addr, tok->size);
}

void
declarations(token* tok)
{
    str name = ident();
    // eg. int a[5]
    if(next() == '[')
        declare_array(tok->type.value, name.value, tok->size);
    // eg. int a
    // eg. int a = 3
    else
    {
        size_t addr = global.addr;
        match('=');
        expression();
        declare(tok->type.value, name.value, tok->size, addr, VAR);
        global.addr = addr + tok->size;
    }
    str_free(&name);
}

void
deref(token* tok)
{
    token res = resolve(tok);
    // eg. a[1] = 1;
    if(next() == '=')
        assign(&res);
    // eg. a[1] + 1;
    else
        quit("empty statement (deref) has no effect; see '%s'", res.name.value);
    token_free(&res);
}

void
misc(str* lead)
{
    token* tok = get(lead);
    // eg. int a
    // eg. int a = 3
    // eg. int a[5]
    if(tok->fam == TYPE)
        declarations(tok);
    else
    {
        // eg. a[1] = 1
        // eg. a[1] + 1
        if(next() == '[')
            deref(tok);
        else
        // eg. a = 1
        if(next() == '=')
            assign(tok);
        // eg. a + 1
        else
            quit("empty statement (misc) has no effect; see '%s'", lead->value);
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
block(void);

void
if_statement(void)
{
    match('(');
    expression();
    global.addr -= DEFAULT_WORD_SIZE;
    match(')');
    size_t a = global.label + 0;
    size_t b = global.label + 1;
    global.label += 2;
    for(size_t i = 0; i < DEFAULT_WORD_SIZE; i++)
    {
        write("\tLDA %d", global.addr + i);
        write("\tBNE L%d", a);
    }
    write("\tJMP L%d", b);
    write("L%d:", a);
    block();
    write("L%d:", b);
}

int
start_of_expression(char c)
{
    return is_digit(c) || c == '(' || is_unary(c);
}

void
statement(void)
{
    if(next() == ';')
        quit("empty statement '%c' found", next());
    if(start_of_expression(next()))
        quit("empty statement has no effect; see '%c'", next());
    // eg. for() { }
    str lead = ident();
    if(str_equal(&lead, "for"))
        unroll_for();
    // eg. asm() { }
    else
    if(str_equal(&lead, "asm"))
        inline_asm();
    // eg. if() { }
    else
    if(str_equal(&lead, "if"))
        if_statement();
    // eg. int a
    // eg. int a[5]
    // eg. a[1] = 1
    // eg. a = 1
    // eg. a + 1
    else
    {
        misc(&lead);
        match(';');
    }
    str_free(&lead);
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
    write("; %8s %8s %6s %6s %6s", "TYPE", "NAME", "SIZE", "ADDR", "F");
    foreach(lst_str, &reversed, it,
        token* tok = get(it.ref);
        write("; %8s %8s %6d %6d %6d",
            tok->type.value, tok->name.value, tok->size, tok->addr, tok->fam);
        global.addr -= tok->size;
        erase(&tok->name);
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
    def_fun(name.value);
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
    keytype("i16", DEFAULT_WORD_SIZE);
    keytype("u8", 1);
    keyword("for");
    keyword("if");
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
        "    i16 a[4];               \n"
        "    i16 b[4];               \n"
        "    for($a : a) {$a = 255;} \n"
        "    b = a;                 \n"
        "    u8 c = 1;              \n"
        "    u8 d = 2;              \n"
        "}                          \n"
    );
}
