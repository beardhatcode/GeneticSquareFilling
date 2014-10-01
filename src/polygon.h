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
} vertex;

/**
 * Struct of a pollygon
 */
typedef struct polygon {
    // Number of vertices
    int num_vert;
    // the vertices
    vertex* verts;
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

#endif	/* POLYGON_H */

