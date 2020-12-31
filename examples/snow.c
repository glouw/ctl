//
// -- Showcases container-container templating to create a 2D array using vectors,
//    and then proceeds to create a neat snow animation --
//

#include <stdio.h>
#include <sys/select.h>

#define P
#define T char
#include <vec.h>

// P is omitted since the previous inclusion of vec
// defines vec_char_init, vec_char_free, and vec_char_copy

#define T vec_char // A 2D array, like std::vector<std::vector<char>>.
#include <vec.h>

enum
{
    SNOWFLAKE = '*', AIR  = ' '
};

static char
get(vec_vec_char* snow, size_t x, size_t y)
{
    if(x < snow->size)
    if(y < snow->value[x].size)
        return snow->value[x].value[y];
    return SNOWFLAKE;
}

static void
set(vec_vec_char* snow, size_t x, size_t y, char c)
{
    snow->value[x].value[y] =  c;
}

static vec_vec_char
setup(size_t xres, size_t yres)
{
    vec_vec_char snow = vec_vec_char_init();
    vec_vec_char_resize(&snow, xres, vec_char_init());
    foreach(vec_vec_char, &snow, it)
        vec_char_resize(it.ref, yres, AIR);
    return snow;
}

static void
add(vec_vec_char* snow, size_t xres)
{
    int chance = 16;
    for(size_t x = 0; x < xres; x++)
        if(rand() % chance == 0)
            set(snow, x, 0, SNOWFLAKE);
}

static void
fall(vec_vec_char* snow, size_t xres, size_t yres)
{
    vec_vec_char copy = vec_vec_char_copy(snow);
    for(size_t y = 0; y < yres; y++)
    for(size_t x = 0; x < xres; x++)
        if(get(&copy, x, y) == SNOWFLAKE)
        {
            size_t y0 = y + 0;
            size_t y1 = y + 1;
            if(get(&copy, x, y1) != SNOWFLAKE)
            {
                set(snow, x, y1, SNOWFLAKE);
                set(snow, x, y0, AIR);
            }
        }
    vec_vec_char_free(&copy);
}

static void
delay(size_t usec)
{
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = usec;
    select(1, NULL, NULL, NULL, &time);
}

static void
draw(vec_vec_char* snow, size_t xres, size_t yres)
{
    for(size_t y = 0; y < yres; y++)
    {
        for(size_t x = 0; x < xres; x++)
            printf("%c", get(snow, x, y));
        putchar('\n');
    }
    printf("\x1B[%zuA", yres); // Rewinds stdout by 'yres' rows.
    printf("\r"); // Rewinds stdout to the zero column.
    delay(25000);
}

int
main(void)
{
    size_t xres = 64;
    size_t yres = 32;
    vec_vec_char snow = setup(xres, yres);
    for(int loops = 512; loops > 0; loops--)
    {
        add(&snow, xres);
        fall(&snow, xres, yres);
        draw(&snow, xres, yres);
    }
    vec_vec_char_free(&snow);
}
