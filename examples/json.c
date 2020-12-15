//
// -- A fast JSON parser written in about 500 lines --
//
// Note:
//     - Support for True, False, and Null is not implemented.
//     - Strings are limited to alphanumeric characters and underscores.
//
//     * That being said, the parser can easily be modified to include support
//       for the above, but a C-like subset of JSON is preferred for
//       the purpose of this example.
//
//     - Output is alphabetically ordered.
//     - Once the JSON has been parsed, values can be erased, modified,
//       inserted, and even copied with common vec and set operations.
//
// See:
//     https://www.json.org/json-en.html

#include <stdio.h>
#include <stdlib.h>

#include <str.h>

#define P
#define T char
#include <stk.h>

struct val;

typedef struct val* valp;

static void
valp_free(valp*);

static valp
valp_copy(valp*);

#define T valp
#include <vec.h>

static vec_valp*
vec_valp_heap_init(void)
{
   vec_valp* self = malloc(sizeof(*self));
   *self = vec_valp_init();
   return self;
}

static void
vec_valp_heap_free(vec_valp* self)
{
    vec_valp_free(self);
    free(self);
}

static vec_valp*
vec_valp_heap_copy(vec_valp* self)
{
   vec_valp* copy = vec_valp_heap_init();
   *copy = vec_valp_copy(self);
   return copy;
}

typedef enum
{
    STRING,
    OBJECT,
    ARRAY,
    NUMBER,
}
fam;

struct set_pair;

static void
set_pair_heap_free(struct set_pair*);

static struct set_pair*
set_pair_heap_copy(struct set_pair*);

typedef union
{
    str string;
    struct set_pair* object;
    vec_valp* array;
    double number;
}
type;

typedef struct val
{
    fam family;
    type of;
}
val;

static void
val_free(valp self)
{
    if(self->family == STRING)
        str_free(&self->of.string);
    else
    if(self->family == OBJECT)
        set_pair_heap_free(self->of.object);
    else
    if(self->family == ARRAY)
        vec_valp_heap_free(self->of.array);
}

static val
val_copy(valp self)
{
    val temp;
    temp.family = self->family;
    if(temp.family == STRING)
        temp.of.string = str_copy(&self->of.string);
    else
    if(temp.family == OBJECT)
        temp.of.object = set_pair_heap_copy(self->of.object);
    else
    if(temp.family == ARRAY)
        temp.of.array  = vec_valp_heap_copy(self->of.array);
    else
    if(temp.family == NUMBER)
        temp.of.number = self->of.number;
    return temp;
}

static valp
valp_init(void)
{
   return malloc(sizeof(val));
}

static void
valp_free(valp* self)
{
    val_free(*self);
    free(*self);
}

static valp
valp_copy(valp* self)
{
   valp copy = valp_init();
   *copy = val_copy(*self);
   return copy;
}

typedef struct
{
    str string;
    valp value;
}
pair;

static void
pair_free(pair* self)
{
    str_free(&self->string);
    valp_free(&self->value);
}

static pair
pair_copy(pair* self)
{
    return (pair) {
        str_copy(&self->string),
        valp_copy(&self->value),
    };
}

#define T pair
#include <set.h>

static int
set_pair_key_compare(pair* a, pair* b)
{
    return str_key_compare(&a->string, &b->string);
}

static set_pair*
set_pair_heap_init(void)
{
   set_pair* self = malloc(sizeof(*self));
   *self = set_pair_init(set_pair_key_compare);
   return self;
}

static void
set_pair_heap_free(set_pair* self)
{
    set_pair_free(self);
    free(self);
}

static set_pair*
set_pair_heap_copy(set_pair* self)
{
   set_pair* copy = set_pair_heap_init();
   *copy = set_pair_copy(self);
   return copy;
}

static stk_char
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

static int
is_space(char c)
{
    return c == ' '
        || c == '\n'
        || c == '\r'
        || c == '\t';
}

static int
next(stk_char* feed)
{
    char c;
    while(is_space(c = *stk_char_top(feed)))
        stk_char_pop(feed);
    return c;
}

static void
match(stk_char* feed, char c)
{
    if(next(feed) != c)
    {
        printf("expected '%c' but received '%c'\n", c, next(feed));
        exit(1);
    }
    stk_char_pop(feed);
}

static int
is_number(char c)
{
    return c == '.'
        || c == '+'
        || c == '-'
        || c == 'e'
        || c == 'E'
        || (c >= '0' && c <= '9');
}

static int
is_string(char c)
{
    return c == '_'
        || (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || is_number(c);
}

static str
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

static str
read_string(stk_char* feed)
{
    match(feed, '"');
    str string = read(feed, is_string);
    match(feed, '"');
    return string;
}

static double
read_number(stk_char* feed)
{
    str number = read(feed, is_number);
    double converted = atof(number.value);
    str_free(&number);
    return converted;
}

static set_pair*
read_object(stk_char*);

static valp
read_value(stk_char* feed)
{
    valp value = valp_init();
    if(is_number(next(feed)))
    {
        value->family = NUMBER;
        value->of.number = read_number(feed);
    }
    else
    if(next(feed) == '"')
    {
        value->family = STRING;
        value->of.string = read_string(feed);
    }
    else
    if(next(feed) == '{')
    {
        value->family = OBJECT;
        value->of.object = read_object(feed);
    }
    else
    if(next(feed) == '[')
    {
        match(feed, '[');
        value->family = ARRAY;
        value->of.array = vec_valp_heap_init();
        while(next(feed) != ']')
        {
            valp sub = read_value(feed);
            vec_valp_push_back(value->of.array, sub);
            if(next(feed) == ']')
                break;
            match(feed, ',');
        }
        match(feed, ']');
    }
    return value;
}

static set_pair*
read_object(stk_char* feed)
{
    set_pair* child = set_pair_heap_init();
    match(feed, '{');
    while(next(feed) != '}')
    {
        str string = read_string(feed);
        match(feed, ':');
        valp value = read_value(feed);
        set_pair_insert(child, (pair) { string, value });
        if(next(feed) == '}')
            break;
        match(feed, ',');
    }
    match(feed, '}');
    return child;;
}

static valp
jsonify(char* serial)
{
    str text = str_init(serial);
    stk_char feed = prime(&text);
    valp json = read_value(&feed);
    stk_char_free(&feed);
    str_free(&text);
    return json;
}

static void
tab(int tabs)
{
    int spaces = 4;
    while(tabs--)
        for(int i = 0; i < spaces; i++)
            printf(" ");
}

static void
pprint(valp, int);

static void
traverse(set_pair* json, int tabs)
{
    printf("{\n");
    foreach(set_pair, json, it,
        tab(tabs);
        printf("\"%s\" : ", it.ref->string.value);
        pprint(it.ref->value, tabs);
        putchar('\n');
    )
    tab(tabs - 1);
    printf("}");
}

static void
pprint(valp value, int tabs)
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
            vec_valp* array = value->of.array;
            int index = 0;
            foreach(vec_valp, array, it,
                valp v = *it.ref;
                printf("[%d] = ", index);
                pprint(v, tabs);
                if(it.ref < vec_valp_end(array) - 1)
                    printf(", ");
                index += 1;
            )
            printf("]");
        }
    }
}

static void
print(valp value)
{
    pprint(value, 0);
    putchar('\n');
}

static int
is_array(valp value)
{
    return value && value->family == ARRAY;
}

static int
is_object(valp value)
{
    return value && value->family == OBJECT;
}

static valp
get(valp value, char* s)
{
    if(is_object(value))
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

static int
is_array_in_bounds(valp value, size_t index)
{
    return index < value->of.array->size;
}

static valp
ind(valp value, size_t index)
{
    if(is_array(value))
        if(is_array_in_bounds(value, index))
            return value->of.array->value[index];
    return NULL;
}

static void
erase_obj(valp value, char* string)
{
    if(is_object(value))
    {
        pair p;
        p.string = str_init(string);
        set_pair_erase(value->of.object, p);
        str_free(&p.string);
    }
}

static void
erase_ind(valp value, size_t index)
{
    if(is_array(value))
        if(is_array_in_bounds(value, index))
            vec_valp_erase(value->of.array, index);
}

int
main(void)
{
    valp json = jsonify(
        "{\n"
            "\"CCC\" : [1.2, 2.2, 3.3],\n"
            "\"BBB\" : 2.2,"
            "\"AAA\" : 1.1,"
            "\"ZZZ\" : \"this_is_a_test\","
            "\"EEE\" : {"
                "\"c\" : 9.3,"
                "\"Z\" : 9.11321,"
                "\"d\" : 3.4,"
                "\"D\" : { \"nested_arrays\": [[[[[[[{}, {}, {}]]]]]]] } ,"
                "\"b\" : 7.2,"
                "\"TEST\" : {"
                    "\"a\" : 9.1,"
                    "\"c\" : 9.3,"
                    "\"e\" : \"name\","
                    "\"b\" : 9.2,"
                    "\"something\" : [\"gustav\", \"susan\", 1, 0],"
                    "\"f\" : \"adam\","
                    "\"g\" : \"gustav\","
                    "\"d\" : 9.4,"
                "}"
            "}"
        "}"
    );
    valp copy = valp_copy(&json);
    print(get(json, "CCC"));
    erase_ind(get(json, "CCC"), 0);
    erase_obj(get(json, "EEE"), "TEST");
    erase_obj(get(json, "EEE"), "D");
    print(get(json, "CCC"));
    print(ind(get(json, "CCC"), 0));
    puts("--- MODIFIED JSON ---");
    print(json);
    puts("--- ORIGINAL COPY ---");
    print(copy);
    valp_free(&json);
    valp_free(&copy);
}
