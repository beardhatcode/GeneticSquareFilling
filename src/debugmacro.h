/* 
 * File:   debugmacro.h
 * Author: garonn
 *
 * Created on 10 October 2014, 14:23
 */

#ifndef DEBUGMACRO_H
#define	DEBUGMACRO_H


#include <stdio.h>
#include <errno.h>
#include <string.h>

#define printfe(M, ...) fprintf(stderr, M,##__VA_ARGS__)
#ifdef DEBUG
#define log_dbg(M, ...) fprintf(stderr, M,##__VA_ARGS__)
#else
#define log_dbg(M, ...) 
#endif

#ifdef DEBUGMPI
#define log_mpidbg(M, ...) fprintf(stderr, M,##__VA_ARGS__)
#else
#define log_mpidbg(M, ...) 
#endif


#define MYMPIERRORHANDLE(M) {if(M!=MPI_SUCCESS){char errorStr[500]={0};int len; MPI_Error_string(M,errorStr,&len);fprintf(stderr,"--\nMPI error on line %d of %s\n%s\n--\n", __LINE__,__FILE__,errorStr);exit(M);}}

#endif	/* DEBUGMACRO_H */

