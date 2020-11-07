#include <str.h>
#include <stdio.h>

#define NONE (-1)

typedef struct
{
    int x;
    int y;
    int priorty;
}
point;

static point
point_init(int x, int y)
{
    return (point) { x, y, 0 };
}

static int
point_compare(point* a, point* b)
{
    return a->priorty < b->priorty;
}

static int
point_equal(point* a, point* b)
{
    return a->x == b->x && a->y == b->y;
}

static point
point_from(str* maze, const char* c, int width)
{
    int index = str_find(maze, c);
    return point_init(index % width, index / width);
}

static int
point_index(point* p, int width)
{
    return p->x + width * p->y;
}

#define P
#define T point
#include <pqu.h>

#define P
#define T point
#include <vec.h>

#define P
#define T point
#include <deq.h>

#define P
#define T int
#include <vec.h>

deq_point
astar(str* maze, int width)
{
    point start = point_from(maze, "@", width);
    point goal = point_from(maze, "!", width);
    pqu_point front = pqu_point_init();
    pqu_point_push(&front, start, point_compare);
    vec_point from = vec_point_init();
    vec_int costs = vec_int_init();
    vec_point_resize(&from, maze->size);
    vec_int_resize(&costs, maze->size);
    foreach(vec_int, &costs, it, *it.ref = NONE;);
    costs.value[point_index(&start, width)] = 0;
    while(!pqu_point_empty(&front))
    {
        point current = front.copy(pqu_point_top(&front));
        pqu_point_pop(&front, point_compare);
        if(point_equal(&current, &goal))
            break;
        point deltas[] = {
            { -1, +1, 0 }, { 0, +1, 0 }, { 1, +1, 0 },
            { -1,  0, 0 }, /* ~ ~ ~ ~ */ { 1,  0, 0 },
            { -1, -1, 0 }, { 0, -1, 0 }, { 1, -1, 0 },
        };
        for(size_t i = 0; i < len(deltas); i++)
        {
            point delta = deltas[i];
            point next = point_init(current.x + delta.x, current.y + delta.y);
            int new_cost = costs.value[point_index(&current, width)];
            if(maze->value[point_index(&next, width)] != '#')
            {
                int cost = costs.value[point_index(&next, width)];
                if(cost == NONE || new_cost < cost)
                {
                    costs.value[point_index(&next, width)] = new_cost;
                    next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                    pqu_point_push(&front, next, point_compare);
                    from.value[point_index(&next, width)] = current;
                }
            }
        }
    }
    point current = goal;
    deq_point path = deq_point_init();
    while(!point_equal(&current, &start))
    {
        deq_point_push_front(&path, current);
        current = from.value[point_index(&current, width)];
    }
    deq_point_push_front(&path, start);
    pqu_point_free(&front);
    vec_point_free(&from);
    vec_int_free(&costs);
    return path;
}

int
main(void)
{
    char* layout =
        "#########################################################################\n"
        "#   #               #               #           #                   #   #\n"
        "#   #   #########   #   #####   #########   #####   #####   #####   # ! #\n"
        "#               #       #   #           #           #   #   #       #   #\n"
        "#########   #   #########   #########   #####   #   #   #   #########   #\n"
        "#       #   #               #           #   #   #   #   #           #   #\n"
        "#   #   #############   #   #   #########   #####   #   #########   #   #\n"
        "#   #               #   #   #       #           #           #       #   #\n"
        "#   #############   #####   #####   #   #####   #########   #   #####   #\n"
        "#           #       #   #       #   #       #           #   #           #\n"
        "#   #####   #####   #   #####   #   #########   #   #   #   #############\n"
        "#       #       #   #   #       #       #       #   #   #       #       #\n"
        "#############   #   #   #   #########   #   #####   #   #####   #####   #\n"
        "#           #   #           #       #   #       #   #       #           #\n"
        "#   #####   #   #########   #####   #   #####   #####   #############   #\n"
        "#   #       #           #           #       #   #   #               #   #\n"
        "#   #   #########   #   #####   #########   #   #   #############   #   #\n"
        "#   #           #   #   #   #   #           #               #   #       #\n"
        "#   #########   #   #   #   #####   #########   #########   #   #########\n"
        "#   #       #   #   #           #           #   #       #               #\n"
        "# @ #   #####   #####   #####   #########   #####   #   #########   #   #\n"
        "#   #                   #           #               #               #   #\n"
        "#########################################################################\n";
    str maze = str_create(layout);
    int width = str_find(&maze, "\n") + 1;
    deq_point path = astar(&maze, width);
    foreach(deq_point, &path, it, maze.value[point_index(it.ref, width)] = 'x';);
    printf(maze.value);
    str_free(&maze);
    deq_point_free(&path);
}
