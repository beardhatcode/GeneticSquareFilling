/* 
 * File:   main.c
 * Author: Robbert Gurdeep Singh
 *
 * Created on 30 September 2014, 13:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "polygon.h"

int main(int argc, char** argv) {

    polygon poly = {0};

    printf("I've got %d tickets to paradise\n", argc);
    if (argc != 3) {
        printf("USAGE: %s num file\n", argv[0]);
        printf("num \t Number of points to add \n"
                "file \t A file containing data\n\n");
        polygon_read("../tests/vierkant.poly", &poly);
    } else {
        polygon_read(argv[2], &poly);
    }


    polygon_print(&poly);

}