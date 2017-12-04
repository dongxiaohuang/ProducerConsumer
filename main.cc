

/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <unistd.h> //sleep

using namespace std;


void *producer(void *id);

void *consumer(void *id);

// Global variables
struct job {
    int id;
    int duration;
};

key_t key = SEM_KEY;
int sem_id;
vector<job> queue;
const job NULL_JOB = {-1, -1};
int produced_job_num = 0;
int consumed_job_num = 0;
int queue_size;
int main(int argc, char **argv) {

// define the required variables
  // int queue_size;
    int job_num;
    int pro_num;
    int con_num;
    vector<int> input_vector;

    sem_id = sem_create(key, Num_Sem);

// check the input numbers
    if (argc != 5) {
        cerr << "please input the correct numbers(4) of inputs!" << endl;
        exit(-1);
    } else {
        for (int i = 1; i <= 4; i++) {
            int check_feedback = check_arg(argv[i]);
            if (check_feedback == -1) {
                cerr << "please input valid number" << endl;
                exit(check_feedback);
            }
            input_vector.push_back(check_feedback);
        }
    }

//initialise the parameters
    queue_size = input_vector[0];
    job_num = input_vector[1];
    pro_num = input_vector[2];
    con_num = input_vector[3];
    for (int i = 0; i < queue_size; i++) {
        queue.push_back(NULL_JOB);
    }

// set-up the producers id and consumers id
    pthread_t producerid[pro_num];
    pthread_t consumerid[con_num];
// set-up and initialise the semaphores
    sem_init(sem_id, MUTEX, 1);/* Semaphore mutex to ensure mutual exclusion */
    sem_init(sem_id, EMPTY, 0); /* Semaphore empty to ensure queue is not empty */
    sem_init(sem_id, FULL, queue_size);  /* Semaphore full to ensure queue is not full */
    sem_init(sem_id, OUTPUT, 1);  /* Semaphore to make sure that output will not be conflicted*/

    para_pro producer_para[pro_num];
    int  consumer_para[con_num];


    for (int j = 0; j < pro_num; ++j) {
        producer_para[j].identity_i = j+1; // id start from 1
        producer_para[j].job_n = job_num;
        pthread_create(&producerid[j], NULL, producer, (void *) &producer_para[j]);
    }
    for (int i = 0; i < con_num; ++i) {
        consumer_para[i] = i+1; // id start from 1
        pthread_create(&consumerid[i], NULL, consumer, (void *) &consumer_para[i]); // create and execute
    }

    // join the thread
    for (int k = 0; k < con_num ; ++k) {
        pthread_join(consumerid[k], NULL);
    }
    for (int l = 0; l < pro_num ; ++l) {
        pthread_join(producerid[l], NULL);
    }

    sem_close(sem_id);
    return 0;
}

void *producer(void *parameter) {
    // initialise the parameters
    para_pro *prop_para = (para_pro *) parameter;
    int job_num = prop_para->job_n;
    int prop_id = prop_para->identity_i;

    for (int j = 0; j < job_num; ++j)
    {
        //produce a job
        job new_job;
        new_job.duration = rand() % 10 + 1;

        if(sem_wait_for_20s(sem_id,FULL) == -1)
        {
            cout <<"no more consumer consumes jobs" <<endl;
            pthread_exit(0);
        }else
        {
            sem_wait(sem_id, MUTEX);// down(mutex)
            int i = produced_job_num % queue_size;
            new_job.id = i + 1;
            queue[i] = new_job;
            sleep(rand() % 5 + 1);
            produced_job_num++;
            sem_wait(sem_id, OUTPUT);
            cout << "Producer(" << prop_id << "): Job id " << new_job.id << " duration " << new_job.duration << endl;
            sem_signal(sem_id, OUTPUT);
            sem_signal(sem_id, MUTEX); // up(mutex).
	    sem_signal(sem_id, EMPTY); //up(empty)
        }
    }
    // quite when there are no job to produce.
    sem_wait(sem_id, OUTPUT);
    cout << "Producer(" << prop_id << "): No more jobs to generate."<< endl;
    sem_signal(sem_id, OUTPUT);
    pthread_exit(0);
}

void *consumer(void *parameter) {

        //initialise the parameters
        int cons_id = *(int *) parameter;
        int job_id = -1;
        int sleep_duration;
        int i;
    while (1) {
        if(sem_wait_for_20s(sem_id, EMPTY) == -1)
        {
	  
          cout << "Consumer("<< cons_id <<"): No more jobs left." << endl;
            pthread_exit(0);
        }else {
            // get a job
	  sem_wait(sem_id, MUTEX);
          i = consumed_job_num % queue_size;
	  // sem_wait(sem_id, MUTEX);//down(mutex)
          if (queue[i].id != NULL_JOB.id)
          {
            sleep_duration = queue[i].duration;
            job_id = queue[i].id;
            consumed_job_num ++;
            sem_wait(sem_id, OUTPUT); // to avoid output conflict
            cout << "Consumer(" << cons_id << "): Job id " << job_id << " executing sleep duration "<< sleep_duration<< endl;
            sem_signal(sem_id, OUTPUT);
            queue[i].id = NULL_JOB.id;
          }
          sem_signal(sem_id, MUTEX); // up(mutex)
          sleep(sleep_duration);
          sem_signal(sem_id, FULL); // up(full)
            // start doing the job
          sem_wait(sem_id, OUTPUT);
          if(job_id != -1)
          cout << "Consumer(" << cons_id << "): Job id " << job_id << " completed" <<endl;
          sem_signal(sem_id, OUTPUT);
            }
      }


        pthread_exit(0);

}
