/* 
 * File:   polygon.h
 * Author: garonn
 *
 * Created on 01 October 2014, 18:54
 */

#ifndef POLYGON_H
#define	POLYGON_H

typedef struct vertex {
    int id;
    float x;
    float y;
    struct vertex* next;
    struct vertex* prev;
    double line_a,line_b;
} vertex;

/**
 * Struct of a pollygon
 */
typedef struct polygon {
    // Number of vertices
    int num_vert;
    // the vertices
    vertex* verts;
    float bound_min_x,bound_min_y,bound_max_x,bound_max_y;
} polygon;


/**
 * Initiate an empty polygon
 * @param size number of vertices
 * @param pointer to store polygon
 * @return 0 on succes, 
 *         -1 on failed malloc, 
 */
int polygon_init(int size ,polygon* poly);

/**
 * Insert the data of a file into a pollygon
 * @param filename woth polygon points
 * @param poly an initiated empty (0) pollygon
 * @return 0 on succes, 
 *         -1 on failed malloc, 
 *         -2 on inpropper data or unreadable file, 
 */
int polygon_read(char* filename,polygon* poly);

/**
 * Prints a representation of the polygon
 * @param poly
 */
void polygon_print(polygon* poly);


/**
 * Checks if a point is within a
 * @param x x pos
 * @param y y pos
 * @param poly
 * @return 1 if point in contained within figure, 0 else
 */
int polygon_contains(float x, float y,polygon* poly);

#endif	/* POLYGON_H */

