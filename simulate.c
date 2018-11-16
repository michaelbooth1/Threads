/******************************************************************************
*
******************************************************************************/

#include "simulate.h"

int memory, max_memory;
FILE *fp;
linked_stack_t *jobs;

void *thread_helper(void *passed_job){
    job_t *new_job = (job_t *) passed_job;                  //Cast passed in job to type job_t 

    if(max_memory < new_job->required_memory){              //Check if enough max memory to handle the job
        print_exceed_memory(fp, new_job->number);
        return NULL;                                        //Return, discarding the job, if not enough memory
    }else if(memory < new_job->required_memory){            //Check if current memory is enough to handle job
        print_insufficient_memory(fp, new_job->number);     
        push(jobs, (void *) new_job);                       //Push job back onto stack and return
        return NULL;
    }else{
        print_allocate_memory(fp, memory, new_job->required_memory);
        memory -= new_job->required_memory;                 //Allocate memory for the job
    }

    print_starting(fp, new_job->number);

    sleep(new_job->required_time);                          //Wait for required job time

    print_deallocate_memory(fp, memory, new_job->required_memory);
    memory += new_job->required_memory;                     //Deallocate the memory 

    print_completed(fp, new_job->number);
    return NULL;                                                   
}

void simulate(int memory_value, linked_stack_t *stack)
{
    fp = fopen(SYSTEM_OUTPUT, "w");                     //Initialize the global variables 
    memory = max_memory = memory_value;
    jobs = stack;
    pthread_t thread_array[NUMBER_OF_THREADS];          //Create array for the threads
    int less_threads;                                   //Used to check if there are less threads needed than max avaible

    if(jobs->size < NUMBER_OF_THREADS){                 //If number of jobs is less than max number of threads, set variable to job size
        less_threads = jobs->size;
    }else{
        less_threads = NUMBER_OF_THREADS;               //Otherwise set it to max number of threads
    }

    while(jobs->size != 0){                             //While there are jobs to do
        for(int i = 0; i < NUMBER_OF_THREADS; i++){     //Loop through array to run multiple threads at once
            if(jobs->size > 0){                         //Check if there are still jobs to do
                void *temp_job = pop(jobs);             //Pop job and assign to temp variable
                if(pthread_create(&thread_array[i], NULL, thread_helper, (void *) (temp_job))){        //Create thread
                    printf("Error creating thread\n");
                }
            }
        }

        for(int j = 0; j < less_threads; j++){          //Only runs the number of times needed if there are less than the max number of threads
            pthread_join(thread_array[j], NULL);        //Join all threads once they are finished running
        } 
    };
}





