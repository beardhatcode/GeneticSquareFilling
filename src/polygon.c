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
 *         less than 0 on failed malloc or read, 
 */
int polygon_read(char* filename, polygon* poly) {
    FILE *fptr = fopen(filename, "r"); //Open file
    float x, y;
    int num, r, i = 0;
    double dx, dy, a, b;

    //Get number of lines and initiate polygon
    r = fscanf(fptr, "%d", &num);
    if (r < 0)
        return r;

    polygon_init(num, poly);
    
    
    while (EOF != fscanf(fptr, "%f %f\n", &x, &y)) {
        poly->verts[i].id = i;
        poly->verts[i].x = x;
        poly->verts[i].y = y;



        // Keep boundaries up to date
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


    //Generate a function for each of the edges (y=ax+b)
    for (i = 0; i < poly->num_vert; i++) {
        dx = (double) poly->verts[i].x - (double) poly->verts[i].next->x;
        dy = (double) poly->verts[i].y - (double) poly->verts[i].next->y;

        a = dy / dx;
        b = (double) poly->verts[i].y - a * (double) poly->verts[i].x;
        poly->verts[i].line_a = a;
        poly->verts[i].line_b = b;
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
    double a, b, yintersect;
    
    //Loop over the vertices
    for (i = 0; i < poly->num_vert; i++) {
        a = poly->verts[i].line_a;
        b = poly->verts[i].line_b;
        yintersect = a * x + b;

        if (!isinf(a)) {
            //"if not line that is parallel to y axis"
            
            //Use product of differences to determine if within points on line
            double deltaproduct = (poly->verts[i].x - x) * (poly->verts[i].next->x - x);
            if (deltaproduct <= 0 && yintersect >= y) {
                if (!(deltaproduct == 0 && poly->verts[i].x == x && poly->verts[i].next->x != x)) {
                    //If deltaproduct == 0 it is exactly beneath a point (don't double count it)
                    count++;
                }
            }
        }
    }

    return count == 1;
}

