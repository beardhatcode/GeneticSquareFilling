#include <stdio.h>
#include <stdlib.h>
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
        i++;
    }

    return 0;
}

/**
 * Prints a representation of the polygon
 * @param poly
 */
void polygon_print(polygon* poly) {
    printf("Printing polygon %p", poly);
    
}

