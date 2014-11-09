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
#include <math.h>
#include <unistd.h> 
#include "mpi.h"
#include "debugmacro.h"
#include "polygon.h"
#include "genetic_base.h"
#include "settings.h"
#include "mpi_main.h"

/* Keep number of procceses globaly */
static int NUM_TASKS = -1;

void serialize_individu(float* target, individu* i, int numpoints)
{
    int j;
    for (j = 0; j < numpoints; j++)
    {
        target[j * 2 + 0] = i->points[j].x;
        target[j * 2 + 1] = i->points[j].y;
    }
}

void deserialize_individu(float* source, individu* i, int numpoints)
{
    int j;
    for (j = 0; j < numpoints; j++)
    {
        i->points[j].x = source[j * 2 + 0];
        i->points[j].y = source[j * 2 + 1];
    }
}

int transfer_individus(population * popu, int task_id)
{
    int i, j, start_pos, start_pos_pro, procces, target_id;

    /* sizes */
    int req_lovers = popu->num_lovers / (NUM_TASKS - 1);
    int lover_size = (popu->numpoints * 2);

    /* Make some space */
    int selected[req_lovers];
    float send_indi[req_lovers * lover_size];
    float recv_indi[NUM_TASKS * req_lovers * lover_size];

    /* Select a->num_lovers/num_pro UNIQUE random individu's */
    for (i = 0; i < req_lovers; i++)
    {
        selected[i] = do_pos_tournament(popu, popu->size * SELECTION_PRESSURE / 100);
        for (j = 0; j < i; j++)
            if (selected[i] == selected[j])
            {
                i--;
                break;
            }

    }


    /* Convert to float-array                       */
    /* fill array as | pt1.x | pt1.y | pt2.x | ... |*/
    for (i = 0; i < req_lovers; i++)
    {
        start_pos = i * lover_size;
        serialize_individu(send_indi + start_pos, popu->list + selected[i], popu->numpoints);
    }

    /* Send data to everyone */
    MPI_Allgather(send_indi, req_lovers * lover_size, MPI_FLOAT, recv_indi, req_lovers * lover_size, MPI_FLOAT, MPI_COMM_WORLD);


    /* Convert back from float-array for each process and add to population */
    target_id = popu->size; /* first individu to be placed at size:*/
    for (procces = 0; procces < NUM_TASKS; procces++)
    {
        if (procces == task_id) continue;
        start_pos_pro = procces * (req_lovers * lover_size);
        for (i = 0; i < req_lovers && target_id < popu->size + popu->num_lovers; i++)
        {
            /* fill array as | fitness | pt1.x | pt1.y | pt2.x | ... |*/
            //Define the start of a "block"
            start_pos = start_pos_pro + i * lover_size;

            deserialize_individu(recv_indi + start_pos, popu->list + target_id, popu->numpoints);

            get_fitness(popu, popu->list + target_id);
            target_id++;
        }
    }

    do_deathmatch(popu, target_id - popu->size);
    return 0;
}

int main(int argc, char *argv[])
{
    int taskid, r;
    polygon poly = {0};
    int numpoints;


    r = MPI_Init(&argc, &argv);
    if (r != MPI_SUCCESS)
    {
        printfe("An MPI error occured at initialisation: ERROR %d", r);
        MPI_Finalize();
        return -1;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &NUM_TASKS);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    r = mpi_parse_input(argc, argv, &poly, &numpoints);

    if (r == 0)
    {
        if (taskid == MASTER)
        {
            srand(time(NULL));
            master_main(&poly, numpoints, taskid);
        }
        else
        {
            srand(time(NULL) * (taskid + 1));
            slave_main(&poly, numpoints, taskid);
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
    *num = 50;
    return r;
}

int parrallel_loops(polygon* poly, int numpoints, int id, int pop_size, int itters, loop_checker end_loop)
{ // <editor-fold defaultstate="collapsed">
    int r, best, i;
    int loopsatatus;
    individu* best_ndi;

    population* popu = NULL;
    printf("Procces %d will place %d points in %p %d atempts\n", id, numpoints, poly, itters);
    r = init_population(pop_size, numpoints, poly, &popu);

    if (r != 0)
    {
        fprintf(stderr, "Could not allocate enough space!\n");
        return -2;
    }

    if (numpoints > 1)
    {
        do
        {
            r = do_iterations(popu, itters);
            if (r < 0)
            {
                fprintf(stderr, "Failed iteration (code %d)!\n", r);
                break;
            }
            loopsatatus = end_loop(popu, r, itters, id);
        }
        while (loopsatatus == GENETIC_CONTINUE);



    }
    else
    {
        /* one point, everything is optimal*/
        r = 1;
        loopsatatus = id == 0 ? GENETIC_BEST : GENETIC_NOT_BEST;
    }
    /*get best*/

    if (id == MASTER)
    {
        if (loopsatatus == GENETIC_BEST)
        {
            best_ndi = popu->list + get_best(popu);
        }
        else
        {
            /* Put best in front of our list...*/
            best_ndi = popu->list;
            {
                float recv_indi[popu->numpoints * 2];
                MPI_Status status;
                MPI_Recv(recv_indi, popu->numpoints * 2, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                deserialize_individu(recv_indi, popu->list, popu->numpoints);
                get_fitness(popu,popu->list);
            }
        }
        printf("%.9f\n", best_ndi->fitness);
        for (i = 0; i < popu->numpoints; i++)
        {
            printf("%f %f\n", best_ndi->points[i].x, best_ndi->points[i].y);
        }
    }
    else
    {

        if (loopsatatus == GENETIC_BEST)
        {

            {
                float send_indi[popu->numpoints * 2];
                serialize_individu(send_indi, popu->list + get_best(popu), popu->numpoints);
                MPI_Send(send_indi, popu->numpoints * 2, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            }
        }

    }
    free_population(&popu);
    return 0;
}// </editor-fold>	

int master_loop(population* a, int done_itters, int itters, int id)
{
    static double prev_best;
    double send_stat[2];
    double gathered_best[NUM_TASKS * 2];
    static int total_itters = 0;
    int i;
    int time_to_end = 1;
    static int times_to_end = 0;
    double cur_best;
    int best_pro = 0;
    total_itters += done_itters;
    printf("Results after %d iterations, \x1b[1mbefore\x1b[0m and \x1b[3mafter\x1b[0m migration\n", total_itters);

    send_stat[0] = a->best;
    send_stat[1] = (double) done_itters / (double) itters;

    MPI_Gather(send_stat, 2, MPI_DOUBLE, gathered_best, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    for (i = 0; i < NUM_TASKS; i++)
        printf("\t\x1b[1mP%d: %4.10f (%1.2f)\x1b[0m \n", i, gathered_best[2 * i], gathered_best[2 * i + 1]);
    printf("\n");

    for (i = 0; i < NUM_TASKS; i++)
    {
        if (gathered_best[2 * i + 1] > 0.95)
        {
            printf("No convergence yet %d did %f of loops\n", i, gathered_best[2 * i + 1]);
            time_to_end = 0; //One of the things did not converge
        }
        if (gathered_best[2 * i] > cur_best)
        {
            cur_best = gathered_best[2 * i];
            best_pro = i;
        }
    }

    if (fabs(prev_best - cur_best) > 0.01)
    {
        printf("Not neglectable change %f\n", fabs(prev_best - cur_best));
        prev_best = cur_best;
        time_to_end = 0;
    }

    if (time_to_end)
    {
        printf("\x1b[31mIt is time (%d)\x1b[0m\n", times_to_end);
        times_to_end++;
    }
    else
    {
        times_to_end = 0;
        printf("\x1b[32mTime reset\x1b[0m\n");
    }


    if (times_to_end > 3)
    {
        /* Send end signal */
        for (i = 1; i < NUM_TASKS; i++)
            MPI_Send(&best_pro, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }


    /* transfer individus */
    if (NUM_TASKS > 1)
        transfer_individus(a, id);


    if (times_to_end > 3)
    {
        return best_pro == id ? GENETIC_BEST : GENETIC_NOT_BEST;
    }

    return GENETIC_CONTINUE; //1;//done_itters<itters ? 0 : 1;
}

int slave_loop(population* a, int done_itters, int itters, int id)
{
    static int total_itters = 0;
    double send_stat[2];
    int flag;
    MPI_Status status;

    total_itters += done_itters;

    /* send the best data to MASTER */
    send_stat[0] = a->best;
    send_stat[1] = (double) done_itters / (double) itters;

    MPI_Gather(send_stat, 2, MPI_DOUBLE, NULL, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //A end broadcast might occur here (in master_loop)

    /* Transfer individus */
    transfer_individus(a, id);

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

    if (flag)
    {
        MPI_Recv(&flag, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        //printf("%d got end msg   best %d!!!!!!!!!\n", id, flag);
        return flag == id ? GENETIC_BEST : GENETIC_NOT_BEST;
    }

    return GENETIC_CONTINUE; //done_itters<itters ? 0 : 1;
}

int master_main(polygon* poly, int numpoints, int id)
{
    parrallel_loops(poly, numpoints, id, NUM_INDIVIDUS, 5000, &master_loop);
    return 0;
}

int slave_main(polygon* poly, int numpoints, int id)
{
    parrallel_loops(poly, numpoints, id, NUM_INDIVIDUS, 5000, &slave_loop);

    return 0;
}