//
// -- A fast JSON parser written in less than 500 lines --
//
// Note:
//     Function arguments containing /* M */ signal a move like std::move
//     Support for True, False, and Null not implemented.
//
// See:
//     https://www.json.org/json-en.html

#include <stdio.h>
#include <stdlib.h>

#include <str.h>

#define P
#define T char
#include <stk.h>

#define quit(msg, ...) { printf("parse error: line %d: %s(): "msg, __LINE__, __func__, __VA_ARGS__); exit(1); }

typedef struct val val;

typedef val* valp;

void
valp_free(val**);

val*
valp_copy(val**);

typedef struct set_pair set_pair;

void
set_pair_heap_free(set_pair**);

set_pair*
set_pair_heap_copy(set_pair**);

#define T valp
#include <vec.h>

typedef enum
{
    STRING,
    OBJECT,
    ARRAY,
    NUMBER,
}
fam;

typedef union
{
    str string;
    set_pair* object;
    vec_valp array;
    double number;
}
type;

typedef struct val
{
    fam family;
    type of;
}
val;

val
val_init(fam family, type of)
{
    val value;
    value.family = family;
    value.of = of;
    return value;
}

void
val_free(val* self)
{
    if(self->family == STRING)
        str_free(&self->of.string);
    else
    if(self->family == OBJECT)
        set_pair_heap_free(&self->of.object);
    else
    if(self->family == ARRAY)
        vec_valp_free(&self->of.array);
}

val
val_copy(val* self)
{
    val temp;
    temp.family = self->family;
    if(temp.family == STRING)
        temp.of.string = str_copy(&self->of.string);
    else
    if(temp.family == OBJECT)
        temp.of.object = set_pair_heap_copy(&self->of.object);
    else
    if(temp.family == ARRAY)
        temp.of.array  = vec_valp_copy(&self->of.array);
    else
    if(temp.family == NUMBER)
        temp.of.number = self->of.number;
    return temp;
}

val*
valp_init(val* value)
{
   val* self = malloc(sizeof(*self));
   *self = *value;
   return self;
}

void
valp_free(val** self)
{
    val_free(*self);
    free(*self);
}

val*
valp_copy(val** self)
{
   val* copy = valp_init(*self);
   *copy = val_copy(*self);
   return copy;
}

typedef struct
{
    str string;
    val* value;
}
pair;

void
pair_free(pair* self)
{
    str_free(&self->string);
    valp_free(&self->value);
}

pair
pair_copy(pair* self)
{
    return (pair) {
        str_copy(&self->string),
        valp_copy(&self->value),
    };
}

#define T pair
#include <set.h>

int
set_pair_key_compare(pair* a, pair* b)
{
    return str_key_compare(&a->string, &b->string);
}

set_pair*
set_pair_heap_init(set_pair* set)
{
   set_pair* self = malloc(sizeof(*self));
   *self = *set;
   return self;
}

void
set_pair_heap_free(set_pair** self)
{
    set_pair_free(*self);
    free(*self);
}

set_pair*
set_pair_heap_copy(set_pair** self)
{
   set_pair* copy = set_pair_heap_init(*self);
   *copy = set_pair_copy(*self);
   return copy;
}

stk_char
prime(str* text)
{
    stk_char feed = stk_char_init();
    while(text->size)
    {
        char c = *str_back(text);
        stk_char_push(&feed, c);
        str_pop_back(text);
    }
    return feed;
}

int
is_space(char c)
{
    return c == ' '
        || c == '\n'
        || c == '\r'
        || c == '\t';
}

int
next(stk_char* feed)
{
    char c;
    while(is_space(c = *stk_char_top(feed)))
        stk_char_pop(feed);
    return c;
}

void
match(stk_char* feed, char c)
{
    if(next(feed) != c)
        quit("expected '%c' but received '%c'", c, next(feed));
    stk_char_pop(feed);
}

int
is_number(char c)
{
    return c == '.'
        || c == '+'
        || c == '-'
        || c == 'e'
        || c == 'E'
        || (c >= '0' && c <= '9');
}

int
is_string(char c)
{
    return c == ' '
        || c == '_'
        || (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || is_number(c);
}


str
read(stk_char* feed, int clause(char c))
{
    next(feed);
    str s = str_init("");
    char c;
    while(clause(c = *stk_char_top(feed)))
    {
        str_push_back(&s, c);
        stk_char_pop(feed);
    }
    return s;
}

str
read_string(stk_char* feed)
{
    match(feed, '"');
    str string = read(feed, is_string);
    match(feed, '"');
    return string;
}

double
read_number(stk_char* feed)
{
    str number = read(feed, is_number);
    double converted = atof(number.value);
    str_free(&number);
    return converted;
}

set_pair*
read_object(stk_char*);

val*
read_value(stk_char* feed)
{
    val value;
    if(is_number(next(feed)))
    {
        value.family = NUMBER;
        value.of.number = read_number(feed);
    }
    else
    if(next(feed) == '"')
    {
        value.family = STRING;
        value.of.string = read_string(feed);
    }
    else
    if(next(feed) == '{')
    {
        value.family = OBJECT;
        value.of.object = read_object(feed);
    }
    else
    if(next(feed) == '[')
    {
        match(feed, '[');
        value.family = ARRAY;
        value.of.array = vec_valp_init();
        while(next(feed) != ']')
        {
            val* v = read_value(feed);
            vec_valp_push_back(&value.of.array, v);
            if(next(feed) == ']')
                break;
            match(feed, ',');
        }
        match(feed, ']');
    }
    return valp_init(&value);
}

set_pair*
read_object(stk_char* feed)
{
    set_pair child = set_pair_init(set_pair_key_compare);
    match(feed, '{');
    while(next(feed) != '}')
    {
        str string = read_string(feed);
        match(feed, ':');
        val* value = read_value(feed);
        set_pair_insert(&child, (pair) { string, value });
        if(next(feed) == '}')
            break;
        match(feed, ',');
    }
    match(feed, '}');
    return set_pair_heap_init(&child);
}

val*
jsonify(char* serial)
{
    str text = str_init(serial);
    stk_char feed = prime(&text);
    val* json = read_value(&feed);
    stk_char_free(&feed);
    str_free(&text);
    return json;
}

void
tab(int tabs)
{
    int spaces = 4;
    while(tabs--)
        for(int i = 0; i < spaces; i++)
            printf(" ");
}

void
traverse(set_pair*, int tabs);

void
pprint(val* value, int tabs)
{
    if(value)
    {
        if(value->family == NUMBER)
            printf("%.2f", value->of.number);
        else
        if(value->family == STRING)
            printf("\"%s\"", value->of.string.value);
        else
        if(value->family == OBJECT)
            traverse(value->of.object, tabs + 1);
        else
        if(value->family == ARRAY)
        {
            printf("[");
            vec_valp* array = &value->of.array;
            foreach(vec_valp, array, it,
                val* value = *it.ref;
                pprint(value, tabs);
                if(it.ref < vec_valp_end(array) - 1)
                    printf(", ");
            )
            printf("]");
        }
    }
}

void
print(val* value)
{
    pprint(value, 0);
}

void
traverse(set_pair* json, int tabs)
{
    printf("{\n");
    foreach(set_pair, json, it,
        tab(tabs + 1);
        printf("\"%s\" : ", it.ref->string.value);
        pprint(it.ref->value, tabs);
        putchar('\n');
    )
    tab(tabs);
    printf("}");
}

val*
get(val* value, char* s)
{
    if(value && value->family == OBJECT)
    {
        pair p;
        p.string = str_init(s);
        set_pair_node* node = set_pair_find(value->of.object, p);
        str_free(&p.string);
        if(node)
            return node->key.value;
    }
    return NULL;
}

val*
ind(val* value, int index)
{
    if(value && value->family == ARRAY && index < value->of.array.size)
        return value->of.array.value[index];
    return NULL;
}

int
main(void)
{
    val* json = jsonify(
        "{"
            "\"AAA\" : 1.1,"
            "\"BBB\" : 2.2,"
            "\"CCC\" : [1.2, 2.2, 3.3],"
            "\"EEE\" : {"
                "\"Z\" : 9.1,"
                "\"b\" : 7.2,"
                "\"c\" : 9.3,"
                "\"d\" : 3.4,"
                "\"TEST\" : {"
                    "\"a\" : 9.1,"
                    "\"b\" : 9.2,"
                    "\"c\" : 9.3,"
                    "\"d\" : 9.4,"
                    "\"e\" : \"name\","
                    "\"f\" : \"adam\","
                    "\"g\" : \"gustav\","
                    "\"something\" : [\"gustav\", \"susan\"],"
                "}"
            "}"
        "}"
    );
    print(ind(get(get(get(json, "EEE"), "TEST"), "something"), 1));
    valp_free(&json);
}
