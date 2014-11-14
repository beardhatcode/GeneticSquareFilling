#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "polygon.h"
#include "genetic_base.h"

int polygon_init(int size, polygon* poly)
{

    int i;
    poly->verts = (vertex*) calloc(size, sizeof (vertex));
    if (NULL == poly->verts)
    {
        return -1;
    }

    /* Set up links to next en prev */
    for (i = 0; i < size; i++)
    {
        poly->verts[i].next = &(poly->verts[(i + 1) % size]);
        poly->verts[i].prev = &(poly->verts[(size + i - 1) % size]);
    }


    poly->num_vert = size;
    return 0;
}

void polygon_free(polygon* poly)
{
    free(poly->verts);
}

/**
 * Insert the data of a file into a pollygon
 * @param filename woth polygon points
 * @param poly an initiated empty (0) pollygon
 * @return 0 on succes, 
 *         less than 0 on failed malloc or read, 
 * @todo crap
 */
int polygon_read(char* filename, polygon* poly)
{
    FILE *fptr = fopen(filename, "r"); /* Open file */
    float x, y;
    int num, r, i = 0;
    double dx, dy, a, b;

    if (NULL == fptr)
        return -1;

    /* Get number of lines and initiate polygon */
    r = fscanf(fptr, "%d", &num);
    if (r < 0)
        return r;

    polygon_init(num, poly);


    while (EOF != fscanf(fptr, "%f %f\n", &x, &y))
    {
        poly->verts[i].id = i;
        poly->verts[i].x = x;
        poly->verts[i].y = y;



        /* Keep boundaries up to date */
        if (x < poly->bound_min_x)
            poly->bound_min_x = x;
        if (x > poly->bound_max_x)
            poly->bound_max_x = x;
        if (y < poly->bound_min_y)
            poly->bound_min_y = y;
        if (y > poly->bound_max_y)
            poly->bound_max_y = y;

        i++;
    }

    fclose(fptr);
    /*Generate a function for each of the edges (y=ax+b) */
    for (i = 0; i < poly->num_vert; i++)
    {
        dx = (double) poly->verts[i].x - (double) poly->verts[i].next->x;
        dy = (double) poly->verts[i].y - (double) poly->verts[i].next->y;

        a = dy / dx;
        b = (double) poly->verts[i].y - a * (double) poly->verts[i].x;
        poly->verts[i].line_a = a;
        poly->verts[i].line_b = b;
    }

    /*Rough approximation of diameter */
    poly->diagonal = sqrt(
                          (poly->bound_min_x - poly->bound_max_x)*(poly->bound_min_x - poly->bound_max_x) +
                          (poly->bound_min_y - poly->bound_max_y)*(poly->bound_min_y - poly->bound_max_y)
                          );

    return 0;
}

/**
 * Prints a representation of the polygon
 * @param poly
 */
void polygon_print(polygon* poly)
{
    printf("Printing constellation x%px\n", poly);
    printf("The constellation be containd by these boundries\n");
    printf("\tthe brig o' captn Vandewalle at %f North and %f West\n", poly->bound_max_x, poly->bound_max_y);
    printf("\tan' the scruvyy seadogs over at %f South and %f East\n", poly->bound_min_y, poly->bound_min_y);

}

int polygon_contains(float x, float y, polygon* poly)
{
    int i, count = 0;
    double a, b, yintersect;

    /*Loop over the vertices */
    for (i = 0; i < poly->num_vert; i++)
    {
        a = poly->verts[i].line_a;
        b = poly->verts[i].line_b;
        yintersect = a * x + b;

        if (!isinf(a))
        {
            /*"if not line that is parallel to y axis" */

            /*Use product of differences to determine if between points on line */
            double deltaproduct = (poly->verts[i].x - x) * (poly->verts[i].next->x - x);
            if (deltaproduct <= 0 && yintersect >= y)
            {

                /* special case: point on line -> accept */
                if (yintersect == y)
                    return 1;

                if (!(deltaproduct == 0 && poly->verts[i].x == x && poly->verts[i].next->x != x))
                {
                    /*If deltaproduct == 0 it is exactly beneath a point (don't double count it) */
                    count++;
                    if (count > 1)
                        break;
                }
            }
        }
        else
        {
            /* line parrallel with y axis */
            if (poly->verts[i].x == x)
            {
                if ((poly->verts[i].y - y)*(poly->verts[i].next->y - y) <= 0)
                {
                    /* point on line */
                    return 1;
                }
                else
                {
                    /* point above or below line*/
                    if(poly->verts[i].y > y)
                        count++;
                }
                if (count > 1)
                    break;
            }
        }
    }

    return count == 1;
}

/**
 * Fill's the given array with number random points within 
 * the polygon
 * 
 * @param number of places to fill
 * @param container ALLOCATED pointer to array;
 * @return 0 on succes
 */
int polygon_random_points(int number, point* container, polygon* poly)
{
    float dx = poly->bound_max_x - poly->bound_min_x;
    float dy = poly->bound_max_y - poly->bound_min_y;
    float x, y;
    int i;
    for (i = 0; i < number; i++)
    {
        do
        {
            x = poly->bound_min_x + ((double) rand() / (double) RAND_MAX) * dx;
            y = poly->bound_min_y + ((double) rand() / (double) RAND_MAX) * dy;

        }
        while (!polygon_contains(x, y, poly));

        container[i].x = x;
        container[i].y = y;
    }

    return 0;
}
