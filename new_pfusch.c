#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <iso646.h>
#include <time.h>


typedef struct point
{
    float x;
    float y;
} point_t;

point_t random_point()
{
    return (point_t) {
        .x = 1 - 2 * drand48(),
        .y = 1 - 2 * drand48()
    };
}

bool is_point_in_unit_circle(point_t const p)
{
    return (p.x * p.x + p.y * p.y <= 1);
}

point_t random_point_in_unit_circle()
{
    point_t p;
    do
    {
        p = random_point();
    } while (!is_point_in_unit_circle(p));

    return p;
    
}

float approximate_pi(int const num_samples)
{
    int inside = 0;

    for (int i = 0; i < num_samples; i++)
    {
        point_t const p = random_point();

        if (is_point_in_unit_circle(p))
        {
            inside++;
        }
    }
    

    return 4.0f * (float)inside / (float)num_samples;
}

int main()
{
    srand48(time(0));

    point_t const piuc = random_point_in_unit_circle();
    printf("point{x: %.2f, y: %.2f} is in unit curcle\n", piuc.x, piuc.y);

    printf("PI is approximately %.6f!\n", approximate_pi(10000));

    return 0;
}
