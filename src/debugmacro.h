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

#ifdef DEBUG

#define log_dbg(M, ...) fprintf(stderr, M,##__VA_ARGS__)

#else



#define log_dbg(M, ...) 

#endif
#endif	/* DEBUGMACRO_H */

