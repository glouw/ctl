//
// -- An infix to postfix converter --
//

#include <stdio.h>

#include <str.h>

// Template signatures can be renamed, such that long form
// lst_str_push_back(lst_str*, ...) can be replaced with
// the a convenient ls_push_back(ls*, ...).

#define lst_str ls
#define T str
#include <lst.h>

#define stk_str ss
#define T str
#include <stk.h>

int
get_prec(char c)
{
    switch(c)
    {
    case '*':
    case '/':
        return 0;
    case '+':
    case '-':
        return 1;
    }
    return 0;
}

int
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int
is_of_operator(char c)
{
    switch(c)
    {
    case '+':
    case '-':
    case '/':
    case '*':
        return 1;
    }
    return 0;
}

int
is_operator(str* s)
{
    return str_compare(s, "+") == 0
        || str_compare(s, "-") == 0
        || str_compare(s, "/") == 0
        || str_compare(s, "*") == 0;
}

int
is_paren(char c)
{
    return c == '('
        || c == ')';
}

int
is_space(char c)
{
    return c == ' ';
}

str
get_digit(str* s, size_t i)
{
    size_t j = i;
    while(is_digit(s->value[j]))
        j += 1;
    return str_substr(s, i, j - i);
}

str
get_operator(str* s, size_t i)
{
    size_t j = i;
    while(is_of_operator(s->value[j]))
        j += 1;
    return str_substr(s, i, j - i);
}

ls
tokenize(str* s)
{
    ls tokens = ls_init();
    for(size_t i = 0; i < s->size; i++)
    {
        char c = s->value[i];
        if(is_space(c))
            continue;
        str token;
        if(is_paren(c))
            token = str_substr(s, i, 1);
        else
        if(is_digit(c))
            token = get_digit(s, i);
        else
        if(is_of_operator(c))
            token = get_operator(s, i);
        else
        {
            printf("error: tokenize(): unknown token '%c'\n", c);
            exit(1);
        }
        ls_push_back(&tokens, token);
        i += token.size - 1;
    }
    return tokens;
}

ls
to_postfix(ls* tokens)
{
    ls postfix = ls_init();
    ss operators = ss_init();
    foreach(ls, tokens, it)
    {
        str* token = it.ref;
        char c = token->value[0];
        if(is_digit(c))
        {
            ls_push_back(&postfix, str_copy(token));
            if(postfix.size > 1 && operators.size)
            {
                if(it.next)
                {
                    str* next = &it.next->value;
                    char cc = next->value[0];
                    if(get_prec(c) < get_prec(cc))
                    {
                        str* operator = ss_top(&operators);
                        char ccc = operator->value[0];
                        if(ccc != '(')
                        {
                            ls_push_back(&postfix, str_copy(operator));
                            ss_pop(&operators);
                        }
                    }
                }
            }
        }
        else
        if(is_operator(token))
            ss_push(&operators, str_copy(token));
        else
        if(is_paren(c))
        {
            if(c == '(')
                ss_push(&operators, str_copy(token));
            else
            if(c == ')')
            {
                int done = 0;
                while(!done)
                {
                    str* top = ss_top(&operators);
                    char cc = top->value[0];
                    if(cc == '(')
                        done = 1;
                    else
                        ls_push_back(&postfix, str_copy(top));
                    ss_pop(&operators);
                }
            }
        }
        else
        {
            printf("error: to_postfix(): unknown token '%c'\n", c);
            exit(1);
        }
    }
    while(!ss_empty(&operators))
    {
        str* operator = ss_top(&operators);
        ls_push_back(&postfix, str_copy(operator));
        ss_pop(&operators);
    }
    ss_free(&operators);
    return postfix;
}

int
main(void)
{
    str s = str_init("64 * (128 / 2 - 128 / (2 - 1))");
    ls tokens = tokenize(&s);
    ls postfix = to_postfix(&tokens);
    puts(s.value);
    foreach(ls, &postfix, it)
        puts(it.ref->value);
    ls_free(&tokens);
    ls_free(&postfix);
    str_free(&s);
}
