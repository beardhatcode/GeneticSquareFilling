#include <stdio.h>
#include <stdlib.h>
#include "polygon.h"

int polygon_init(int size ,polygon* poly){
    poly->verts = (vertex*) calloc(size,sizeof(vertex));
    if(NULL == poly->verts){
        return -1;
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
int polygon_read(char* filename,polygon* poly){
    FILE *fptr = fopen(filename,"r");
    float x,y;
    while (0 < fscanf(fptr,"%f %f\n",&x,&y)) {
        printf("found %f %f\n",x,y);
    }

    return 0;
}

/**
 * Prints a representation of the polygon
 * @param poly
 */
void polygon_print(polygon* poly){
    printf("heaz");
}
