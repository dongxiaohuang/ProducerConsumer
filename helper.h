

/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/
#ifndef HELPER_H
#define HELPER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
using namespace std;

# define SEM_KEY 9987 // Change this number as needed
# define Num_Sem 4
# define MUTEX 0
# define EMPTY 1
# define FULL 2
# define OUTPUT 3
union semun {
     int val;               /* used for SETVAL only */
     struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
     ushort *array;         /* used for GETALL and SETALL */
 };

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);
int sem_wait_for_20s (int id, short unsigned int num);
struct para_pro // used to pass the parameters to producer
{
    int identity_i;
    int job_n;
 };
struct para_con // used to pass the parameters to consumer
{
    int identity_i;
    int q_size;
};

#endif
