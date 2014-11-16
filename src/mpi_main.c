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

int transfer_individus_gather(population * popu, int task_id)
{
    int i, target_id, r;
    /* sizes */
    int req_lovers = popu->num_lovers;
    int lover_size = (popu->numpoints * 2);
    int per_target = popu->num_lovers / (NUM_TASKS - 1);

    /* Make some space */
    float send_indi[req_lovers][lover_size];
    int selected;
    /* Gather */
    float recv_indi[NUM_TASKS][per_target][lover_size];
    int procces;

    /* Select a->num_lovers random individu's */;
    for (i = 0; i < req_lovers; i++)
    {
        selected = do_pos_tournament(popu, 2 * popu->size * SELECTION_PRESSURE / 100);
        serialize_individu(send_indi[i], popu->list + selected, popu->numpoints);
    }

    r = MPI_Allgather(send_indi, per_target * lover_size, MPI_INT, recv_indi, per_target * lover_size, MPI_FLOAT, MPI_COMM_WORLD);
    MYMPIERRORHANDLE(r);
    target_id = popu->size;
    for (procces = 0; procces < NUM_TASKS; procces++)
    {
        if (procces == task_id) continue;
        for (i = 0; i < per_target && target_id < popu->size + popu->num_lovers; i++)
        {
            /* Define the start of a "block" */
            deserialize_individu(recv_indi[procces][i], popu->list + target_id, popu->numpoints);
            get_fitness(popu, popu->list + target_id);
            //log_mpidbg("\x1b[3%dm %d Received form %d-%d nr %d: %f %f\x1b[0m\n", task_id, task_id, procces, i,target_id, (popu->list + target_id)->fitness, recv_indi[procces][i][0]);
            target_id++;
        }
    }
    do_deathmatch(popu, target_id - popu->size);

    return 0;

}

int transfer_individus_island(population * popu, int task_id)
{
    int i, target_id, r;

    /* sizes */
    int req_lovers = popu->num_lovers;
    int lover_size = (popu->numpoints * 2);

    float recv_indi[req_lovers][lover_size];
    MPI_Request req[req_lovers];
    MPI_Status status[req_lovers];

    /* Make some space */
    float send_indi[req_lovers][lover_size];
    int selected;

    /* Select a->num_lovers random individu's */;
    for (i = 0; i < req_lovers; i++)
    {
        selected = do_pos_tournament(popu, 2 * popu->size * SELECTION_PRESSURE / 100);
        serialize_individu(send_indi[i], popu->list + selected, popu->numpoints);
    }


    /* Star */
    //log_mpidbg("%d is sending 1 to some (%d)\n", task_id, req_lovers);
    for (i = 0; i < req_lovers; i++)
    {
        /* One for peers */
        target_id = (task_id + 1 + i * (NUM_TASKS / req_lovers)) % NUM_TASKS;
        r = MPI_Isend(send_indi[i], lover_size, MPI_FLOAT, target_id, 5, MPI_COMM_WORLD, &(req[i]));
        MYMPIERRORHANDLE(r);
    }
    //log_mpidbg("%d has sent 1 to %d (%d), waiting...\n", task_id, req_lovers, req_lovers);
    for (i = 0; i < req_lovers; i++)
    {
        r = MPI_Recv(recv_indi[i], lover_size, MPI_FLOAT, MPI_ANY_SOURCE, 5, MPI_COMM_WORLD, &(status[i]));
        MYMPIERRORHANDLE(r);
    }

    //log_mpidbg("%d got %d/%d, waiting for recv...\n", task_id, req_lovers, req_lovers);

    target_id = popu->size;
    for (i = 0; i < req_lovers; i++)
    {
        deserialize_individu(recv_indi[i], popu->list + target_id, popu->numpoints);
        get_fitness(popu, popu->list + target_id);
        target_id++;
    }

    for (i = 0; i < req_lovers; i++)
    {
        r = MPI_Wait(&(req[i]), &(status[i]));
        MYMPIERRORHANDLE(r);
    }
    //log_mpidbg("%d done waiting\n", task_id);
    r = MPI_Barrier(MPI_COMM_WORLD);
    MYMPIERRORHANDLE(r);
    do_deathmatch(popu, target_id - popu->size);

    return 0;
}

int transfer_individus(population * popu, int task_id)
{


    if (popu->num_lovers / (NUM_TASKS - 1) >= 1)
    {
        transfer_individus_gather(popu, task_id);
    }
    else
    {
        transfer_individus_island(popu, task_id);
    }
    /* Convert back from float-array for each process and add to population */

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

    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);


    r = MPI_Comm_size(MPI_COMM_WORLD, &NUM_TASKS);
    MYMPIERRORHANDLE(r);
    r = MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MYMPIERRORHANDLE(r);

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

    return 0;
}

int mpi_parse_input(int argc, char *argv[], polygon* poly, int* num)
{
    /* handle errors */
    int r = polygon_read(argv[2], poly);
    *num = atoi(argv[1]);
    return r;
}

int parrallel_loops(polygon* poly, int numpoints, int id, int pop_size, int itters, loop_checker end_loop)
{ /* <editor-fold defaultstate="collapsed"> */
    int r, i;
    int loopsatatus;
    individu* best_ndi;

    population* popu = NULL;
    log_mpidbg("Procces %d will place %d points in %p %d atempts\n", id, numpoints, poly, itters);
    r = init_population(pop_size, numpoints, poly, &popu);

    if (r != 0)
    {
        printfe("Could not allocate enough space!\n");
        return -2;
    }

    if (numpoints > 1)
    {
        do
        {
            r = do_iterations(popu, itters);
            if (r < 0)
            {
                printfe("Failed iteration (code %d)!\n", r);
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

            float recv_indi[popu->numpoints * 2];
            MPI_Status status;

            r = MPI_Recv(recv_indi, popu->numpoints * 2, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MYMPIERRORHANDLE(r);
            deserialize_individu(recv_indi, popu->list, popu->numpoints);

            get_fitness(popu, popu->list);

        }
        printf("%.9f\n", best_ndi->fitness);
#ifndef PERFORMANCE_PRINT
        for (i = 0; i < popu->numpoints; i++)
        {
            printf("%f %f\n", best_ndi->points[i].x, best_ndi->points[i].y);
        }
#else
        printf("%d proc\n",NUM_TASKS);
        printf("%d points\n",numpoints);
#endif
    }
    else
    {

        if (loopsatatus == GENETIC_BEST)
        {

            {
                int r;
                float send_indi[popu->numpoints * 2];
                serialize_individu(send_indi, popu->list + get_best(popu), popu->numpoints);
                r = MPI_Send(send_indi, popu->numpoints * 2, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
                MYMPIERRORHANDLE(r);
            }
        }

    }
    free_population(&popu);
    return 0;
}/* </editor-fold> */

int master_loop(population* a, int done_itters, int itters, int id)
{
    static double prev_best;
    double send_stat[2];
    double gathered_best[NUM_TASKS * 2];
    static int total_itters = 0;
    int i, r;
    int loops_saturated = 0;
    static int times_staturated = 0;
    static int times_stagnation = 0;
    double cur_best;
    int best_pro = 0;
    total_itters += done_itters;
    log_mpidbg("Results after %d iterations, \x1b[1mbefore\x1b[0m  migration\n", total_itters);

    send_stat[0] = a->best;
    send_stat[1] = (double) done_itters / (double) itters;

    r = MPI_Gather(send_stat, 2, MPI_DOUBLE, gathered_best, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MYMPIERRORHANDLE(r);
    for (i = 0; i < NUM_TASKS; i++)
        log_mpidbg("\t\x1b[1mP%d: %4.10f (%1.2f)\x1b[0m \n", i, gathered_best[2 * i], gathered_best[2 * i + 1]);
    log_mpidbg("\n");

    for (i = 0; i < NUM_TASKS; i++)
    {
        if (gathered_best[2 * i + 1] < 0.95)
        {
            //log_mpidbg("No convergence yet %d did %f of loops\n", i, gathered_best[2 * i + 1]);
            loops_saturated++; /*One of the things did not converge*/
        }
        if (gathered_best[2 * i] > cur_best)
        {
            cur_best = gathered_best[2 * i];
            best_pro = i;
        }
    }


    if (fabs(prev_best - cur_best) > 0.0000000001 * cur_best)
    {
        log_mpidbg("Can't ignore change %f\n", fabs(prev_best - cur_best));
        loops_saturated = 0;
        times_stagnation = 0;
    }
    else
    {
        times_stagnation++;
    }
    prev_best = cur_best;


    if ((float) loops_saturated / (float) NUM_TASKS > 0.3 || times_stagnation > 5)
    {
        log_mpidbg("\x1b[31mIt is time (%d)\x1b[0m\n", times_staturated);
        times_staturated++;
    }
    else
    {
        times_staturated = 0;
        log_mpidbg("\x1b[32mTime reset (%d)\x1b[0m\n", times_stagnation);
    }


    if (times_staturated > 3)
    {
        /* Send */
        {
            MPI_Request endrequests[NUM_TASKS - 1];
            /* Send end signal */
            for (i = 1; i < NUM_TASKS; i++)
            {
                int r = MPI_Isend(&best_pro, 1, MPI_INT, i, 55, MPI_COMM_WORLD, &(endrequests[i - 1]));
                MYMPIERRORHANDLE(r);
            }

            MPI_Status status;
            for (i = 1; i < NUM_TASKS; i++)
            {
                int r = MPI_Wait(&(endrequests[i - 1]), &status);
                MYMPIERRORHANDLE(r);
            }

            /* transfer individus */
            if (NUM_TASKS > 1)
                transfer_individus(a, id);


            return best_pro == id ? GENETIC_BEST : GENETIC_NOT_BEST;
        }

    }
    else
    {

        /* transfer individus */
        if (NUM_TASKS > 1)
            transfer_individus(a, id);
    }





    return GENETIC_CONTINUE;
}

int slave_loop(population* a, int done_itters, int itters, int id)
{
    static int total_itters = 0;
    double send_stat[2];
    int flag, r;
    MPI_Status status;

    total_itters += done_itters;

    /* send the best data to MASTER */
    send_stat[0] = a->best;
    send_stat[1] = (double) done_itters / (double) itters;
    r = MPI_Gather(send_stat, 2, MPI_DOUBLE, NULL, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MYMPIERRORHANDLE(r);
    /*A end broadcast might occur here (in master_loop) */

    /* Transfer individus */
    transfer_individus(a, id);
    r = MPI_Iprobe(MPI_ANY_SOURCE, 55, MPI_COMM_WORLD, &flag, &status);
    MYMPIERRORHANDLE(r);

    if (flag)
    {
        r = MPI_Recv(&flag, 1, MPI_INT, MPI_ANY_SOURCE, 55, MPI_COMM_WORLD, &status);
        MYMPIERRORHANDLE(r);
        return flag == id ? GENETIC_BEST : GENETIC_NOT_BEST;
    }

    return GENETIC_CONTINUE;
}

int master_main(polygon* poly, int numpoints, int id)
{
    parrallel_loops(poly, numpoints, id, NUM_INDIVIDUS / 2, 50, &master_loop);
    return 0;
}

int slave_main(polygon* poly, int numpoints, int id)
{
    parrallel_loops(poly, numpoints, id, NUM_INDIVIDUS / 2, 50, &slave_loop);

    return 0;
}