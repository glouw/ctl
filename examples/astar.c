// This example uses CTL to build an astar pathfinder.
// The solver starts from position '@' and ends at position '!'.
//
// For implementation details:
//     https://www.redblobgames.com/pathfinding/a-star/introduction.html

#include <str.h>
#include <stdio.h>

typedef struct
{
    int x;
    int y;
    int priorty;
    int width;
}
point;

static point
point_init(int x, int y, int width)
{
    return (point) { x, y, 0, width };
}

static int
point_compare_priority(point* a, point* b)
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
    return point_init(index % width, index / width, width);
}

static int
point_index(point* p)
{
    return p->x + p->width * p->y;
}

static int
point_key_compare(point* a, point* b)
{
    int i = point_index(a);
    int j = point_index(b);
    return (i == j) ? 0 : (i < j) ? -1 : 1;
}

#define P
#define T point
#include <pqu.h>

#define P
#define T point
#include <deq.h>

#define P
#define T point
#define U int
#include <map.h>

#define P
#define T point
#define U point
#include <map.h>

deq_point
astar(str* maze, int width)
{
    point start = point_from(maze, "@", width);
    point goal = point_from(maze, "!", width);
    pqu_point front = pqu_point_create(point_compare_priority);
    pqu_point_push(&front, start);
    map_point_point from = map_point_point_create(point_key_compare);
    map_point_int costs = map_point_int_create(point_key_compare);
    map_point_int_insert(&costs, start, 0);
    while(!pqu_point_empty(&front))
    {
        point current = *pqu_point_top(&front);
        pqu_point_pop(&front);
        if(point_equal(&current, &goal))
            break;
        point deltas[] = {
            { -1, +1, 0, width }, { 0, +1, 0, width }, { 1, +1, 0, width },
            { -1,  0, 0, width }, /* ~ ~ ~ ~ ~ ~ ~ */  { 1,  0, 0, width },
            { -1, -1, 0, width }, { 0, -1, 0, width }, { 1, -1, 0, width },
        };
        for(size_t i = 0; i < len(deltas); i++)
        {
            point delta = deltas[i];
            point next = point_init(current.x + delta.x, current.y + delta.y, width);
            int new_cost = *map_point_int_at(&costs, current);
            if(maze->value[point_index(&next)] != '#')
            {
                map_point_int_node* cost = map_point_int_find(&costs, next);
                if(cost == map_point_int_end(&costs)
                || new_cost < cost->second)
                {
                    map_point_int_insert(&costs, next, new_cost);
                    next.priorty = new_cost + abs(goal.x - next.x) + abs(goal.y - next.y);
                    pqu_point_push(&front, next);
                    map_point_point_insert(&from, next, current);
                }
            }
        }
    }
    point current = goal;
    deq_point path = deq_point_init();
    while(!point_equal(&current, &start))
    {
        deq_point_push_front(&path, current);
        current = *map_point_point_at(&from, current);
    }
    deq_point_push_front(&path, start);
    pqu_point_free(&front);
    map_point_point_free(&from);
    map_point_int_free(&costs);
    return path;
}

int
main(void)
{
    str maze = str_create(
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
        "#########################################################################\n");
    int width = str_find(&maze, "\n") + 1;
    deq_point path = astar(&maze, width);
    foreach(deq_point, &path, it, maze.value[point_index(it.ref)] = 'x';);
    printf("%s", maze.value);
    str_free(&maze);
    deq_point_free(&path);
}
