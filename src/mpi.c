/* 
 * File:   mpi.c
 * Author: garonn
 *
 * Created on 04 November 2014, 19:51
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h> 
#include "mpi.h"
#include "debugmacro.h"
#include "polygon.h"
#include "genetic_base.h"
#include "settings.h"
#define  MASTER		0

int mpi_parse_input(int argc, char *argv[], polygon* poly, int* num);
int master_main(polygon* poly, int num);
int slave_main(int id, polygon* poly, int num);

int main(int argc, char *argv[])
{
    int numtasks, taskid, r;
    polygon poly = {0};
    int numpoints;

    r = MPI_Init(&argc, &argv);
    if (r != MPI_SUCCESS)
    {
        printfe("An MPI error occured at initialisation: ERROR %d", r);
        MPI_Finalize();
        return -1;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    r = mpi_parse_input(argc, argv, &poly, &numpoints);
    if (r == 0)
    {
        if (taskid == MASTER)
        {
            srand(time(NULL));
            master_main(&poly, numpoints);
        }
        else
        {
            
            srand(time(NULL)*taskid);
            slave_main(taskid, &poly, numpoints);
        }

        MPI_Finalize();
    }
    else
    {
        printfe("An error occured...");
        MPI_Finalize();
        return r;
    }

}

int mpi_parse_input(int argc, char *argv[], polygon* poly, int* num)
{
    int r = polygon_read("../tests/vierkant.poly", poly);
    *num = 5;
    return r;
}

int master_main(polygon* poly, int numpoints)
{
    int r, best, i;
    population* popu = NULL;
    printf("Master will place %d points in %p 200 atempts\n", numpoints, poly);
    r = init_population(NUM_INDIVIDUS, numpoints, poly, &popu);
    printf("popu, %d\n", r);
    if (r != 0)
    {
        fprintf(stderr, "Could not allocate enough space!\n");
        return -2;
    }

    if (numpoints > 1)
    {
        printf("go MASTER\n");
        r = do_iterations(popu, 200);
        if (r < 0)
        {
            fprintf(stderr, "Failed iteration (code %d)!\n", r);
        }
    }
    else
    {
        /* one point, everything is optimal*/
        r = 1;
    }
    /*get best*/
    best = get_best(popu);
    printf("%f\n", popu->list[best].fitness);
    for (i = 0; i < popu->numpoints; i++)
    {
        printf("%f %f\n", popu->list[best].points[i].x, popu->list[best].points[i].y);
    }

    return 0;
}

int slave_main(int id, polygon* poly, int numpoints)
{
    int r, best, i;
    population* popu = NULL;
    printf("Slave %d will place %d points in %p %d000 atempts\n", id, numpoints, poly, id);
    r = init_population(NUM_INDIVIDUS, numpoints, poly, &popu);
    printf("population ok %d\n", id);
    if (r != 0)
    {
        fprintf(stderr, "Could not allocate enough space!\n");
        return -2;
    }

    if (numpoints > 1)
    {
        printf("%d>> in %p\n", id, popu);
        r = do_iterations(popu, id*1000);
        if (r < 0)
        {
            fprintf(stderr, "Failed iteration (code %d)!\n", r);
        }
    }
    else
    {
        /* one point, everything is optimal*/
        r = 1;
    }
    /*get best*/
    best = get_best(popu);
    printf("%d>> %f -> %d\n", id, popu->list[best].fitness, r);
    for (i = 0; i < popu->numpoints; i++)
    {
        printf("%d>> %f %f\n", id, popu->list[best].points[i].x, popu->list[best].points[i].y);
    }

    return 0;
}
