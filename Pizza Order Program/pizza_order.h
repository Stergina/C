/* Stergiani Koukoumtzi 3170258 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h> 
#include <time.h> 
#include <math.h> 

/* Declared in p3170258-pizza2.c */
extern int Ncook;
extern int Noven;
extern int Ndeliverer;
extern int Torderlow;
extern int Torderhigh;
extern int Norderlow;
extern int Norderhigh;
extern int Tlow;
extern int Thigh;
extern int Tprep;
extern int Tbake;

void *order(void *x);