#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "polygon.h"

int polygon_init(int size, polygon* poly) {

    int i;
    poly->verts = (vertex*) calloc(size, sizeof (vertex));
    if (NULL == poly->verts) {
        return -1;
    }

    //Set up links to next en prev
    for (i = 0; i < size; i++) {
        poly->verts[i].next = &(poly->verts[(i + 1) % size]);
        poly->verts[i].prev = &(poly->verts[(size + i - 1) % size]);
    }


    poly->num_vert = size;
    return 0;
}

/**
 * Insert the data of a file into a pollygon
 * @param filename woth polygon points
 * @param poly an initiated empty (0) pollygon
 * @return 0 on succes, 
 *         -1 on failed malloc, 
 *         -2 on inpropper data or unreadable file, 
 */
int polygon_read(char* filename, polygon* poly) {
    FILE *fptr = fopen(filename, "r");
    float x, y;
    int num, i = 0;
    int r = fscanf(fptr, "%d", &num);
    if (r < 0) return r;
    polygon_init(num, poly);
    while (EOF != fscanf(fptr, "%f %f\n", &x, &y)) {
        poly->verts[i].id = i;
        poly->verts[i].x = x;
        poly->verts[i].y = y;



        // Keep bounderies up to date
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

    return 0;
}

/**
 * Prints a representation of the polygon
 * @param poly
 */
void polygon_print(polygon* poly) {
    printf("Printing constellation x%px\n", poly);
    printf("The constellation be containd by these boundries\n");
    printf("\tthe brig o' captn Vandewalle at %f North and %f West\n", poly->bound_max_x, poly->bound_max_y);
    printf("\tan' the scruvyy seadogs over at %f South and %f East\n", poly->bound_min_y, poly->bound_min_y);

}

int polygon_contains(float x, float y, polygon* poly) {
    int i, count = 0;
    for (i = 0; i < poly->num_vert; i++) {
        double dx = (double) poly->verts[i].x - (double) poly->verts[i].next->x;
        double dy = (double) poly->verts[i].y - (double) poly->verts[i].next->y;
        double a = dy / dx;
        double b = (double) poly->verts[i].y - a * (double) poly->verts[i].x;
        double yintersect = a * x + b;
        if (!isinf(a)) {

            /*
                        printf("(%.2f,%.2f) ", poly->verts[i].x, poly->verts[i].y);
                        printf("(%.2f,%.2f) ", poly->verts[i].next->x, poly->verts[i].next->y);
                        printf("%gx+%g %f %f\n", a, b, a * poly->verts[i].x + b, poly->verts[i].y);
                        printf("   (%1.2f,%1.2f)   ---> %1.2f,%1.2f [%1.2f,%1.2f]         (%c,%c)", x, y, x, yintersect,
                                poly->verts[i].x, poly->verts[i].next->x
                                , yintersect >= y ? 'Y' : 'N',
                                ((poly->verts[i].x - x) * (poly->verts[i].next->x - x) <= 0) ? 'Y' : 'N');
             */

            double deltaproduct = (poly->verts[i].x - x) * (poly->verts[i].next->x - x);

            if (deltaproduct <= 0 && yintersect >= y) {


                if (deltaproduct == 0 && poly->verts[i].x == x && poly->verts[i].next->x != x) {
                    //Exact collision in point, only count first segment.
                    //printf(" HIT DUP");
                } else {
                    count++;
                    //printf(" \x1b[32mHIT\x1b[0m ");
                }
                //printf("\n");

            }

        }


    }
    printf("\x1b[1mFound %d intersections above point (%g,%g), \x1b[0m\x1b[0;%s\x1b[0m\x1b[1m figure\x1b[0m\n", count, x, y, count == 1 ? "32mWITHIN" : "31mOUT OF");

}

