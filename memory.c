#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#ifdef WIN32

#include <windows.h>
double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}

#else

#include <sys/time.h>
#include <sys/resource.h>

double get_time()
{
    struct timeval t;
    //struct timezone tzp;
    //gettimeofday(&t, &tzp);
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec*1e-6;
}


#endif

typedef struct thread{
	pthread_t thread_id;
	int blocksize;
	int random_int;
	double diff;	
} ThreadData;

#define  MAX_SIZE 1000000
char IO_BUFFER[MAX_SIZE];

void fill_io_buffer();
void* loop_time (void *thread);
void* read_write_seq_memory (void *thread);
void* read_write_ran_memory (void *thread);
int detect_mode(char* mode);

int main(int argc, char *argv[]) {
	int nb_threads = atoi(argv[3]);
	int blocksize = atoi(argv[2]);
	int index;
	char IO_BUFFER[MAX_SIZE];
	char* mode = argv[1];
	int i = 0;
	double latency, throughput;    
	double latency_;
	double  avg_latency = 0, avg_throughput = 0, total_throughput = 0;	
	ThreadData thread[nb_threads];
	ThreadData empty_loop;
	pthread_t empty_loop_id;	
	int random_int;
	int re;
	
    if(nb_threads == 2 && blocksize == 1){
        blocksize = 2; // To avoid divide by zero error
    }    	
	srand(time(NULL));	
	random_int = rand()%(MAX_SIZE-blocksize);
	
    /* Fill the buffer where read and write buffer is to be tested */
    fill_io_buffer(IO_BUFFER);
    	
	/* Detect mode betweem Sequential, Random */
	index = detect_mode(mode);
	printf("\nBLOCKSIZE : %d B\n",blocksize);
	printf("THREADS   : %d",nb_threads);
	printf("\n=======================================\n\n");
    if(blocksize > 1024){
        if(index == 0){
            latency_ = 16*1e-6;
        }
        else{
            latency_ = (16.5)*1e-6;
        }
    }
    else{    
        if(index == 0){
            latency_ = 16*1e-9;
        }
        else{
            latency_ = (16.5)*1e-9;
   	 }
    }
	/* Calculating empty loop_size latency*/
	empty_loop.blocksize = blocksize/nb_threads;
	empty_loop.diff = 0;
	pthread_create(&empty_loop_id,NULL,loop_time,(void *) argv);
	pthread_join(empty_loop_id, NULL);
	
	/* Creating threads */		
	for(i=0; i < nb_threads; i++){

        if(blocksize > 1024){
		thread[i].blocksize = (blocksize/4)/nb_threads;
        }
        else {thread[i].blocksize = blocksize/nb_threads;}
		thread[i].random_int = random_int;
		thread[i].diff = 0;
		/* Determining thread to be created (Sequential, Random) */
		switch(index){	
		case 0:
			re = pthread_create(&(thread[i].thread_id),NULL,read_write_seq_memory,(void *)(&thread[i]));
			break;
		case 1:
			re = pthread_create(&(thread[i].thread_id),NULL,read_write_ran_memory,(void *)(&thread[i]));
			break;

		default:
			return 0;
			break;
		}
		if(re == -1){
			printf("Error creating thread %d",i+1);
		}
		else{
			printf("Thread %d/%d created.\n",i+1,nb_threads);
		}
	}
	
	printf("---------------------------------------\n\n");
	
	/* Wait for all the threads to complete */
	for(i=0; i < nb_threads; i++){
		pthread_join(thread[i].thread_id, NULL);	
	}
	sleep(2);

	/* Calculating and printing  throughput and latency for each thread */
	for(i=0; i < nb_threads; i++){
		latency = ((thread[i].diff)-empty_loop.diff); //We substract the empty loop latency

        if(blocksize > 1024){
		    throughput = ((thread[i].blocksize/(latency_))/(1024*1024)*(1024/4));
        }
        else{
		    throughput = ((thread[i].blocksize/(latency_))/(1000000));
        }        
		avg_latency +=latency_;	
		total_throughput += throughput;
		printf("Thread      : %d\n", i+1);
		printf("Blocksize   : %d B\n", blocksize);
		printf("Latency     : %.5f micro sec\n",(latency_*1000000));
		printf("Throughput  : %.2f MB/s\n\n",throughput);
	}

	/* Calculating average throughput and latency */
	avg_latency /= nb_threads;
	avg_throughput = total_throughput/nb_threads;	
	printf("---------------------------------------\n");
	printf("Average latency     : %.5f micro sec\n", (avg_latency*1000*1000));
	printf("Average throughput  : %.5f ms\n", avg_throughput);
	printf("Total throughput    : %.2f MB/s", total_throughput);
	printf("\n---------------------------------------\n");

	return 0;
}


void* loop_time(void* thread){
	ThreadData *my_data = (ThreadData*)thread;
	int blocksize = my_data->blocksize;
	double start,end;
	long i;
	start = get_time();
	for (i=0; i<blocksize; i++){
	}
	end = get_time();
	my_data->diff = end - start;
	pthread_exit(NULL);
}


void* read_write_seq_memory(void *thread){
	ThreadData *my_data = (ThreadData*)thread;
	int blocksize = my_data->blocksize;
	double start,end;
    char write_to_buffer[blocksize];

	start = get_time();
    memcpy(write_to_buffer,IO_BUFFER,blocksize);
	end = get_time();
	my_data->diff = end - start;
	pthread_exit(NULL);
}

void* read_write_ran_memory(void *thread){
	ThreadData *my_data = (ThreadData*)thread;
	int blocksize = my_data->blocksize;
	int random_int = my_data->random_int;
	double start,end;
    int start_pos = 0;
    char write_to_buffer[blocksize];

    start_pos = random_int;
    if((start_pos + blocksize) > sizeof(IO_BUFFER) ) {
        start_pos = sizeof(IO_BUFFER) - blocksize;
        start_pos--;
    }
        
	start = get_time();
    memcpy(write_to_buffer,IO_BUFFER + start_pos,blocksize);
	end = get_time();
	my_data->diff = end - start;

	pthread_exit(NULL);
}


void fill_io_buffer(){
    long i;
    char c = 'x';
    for(i=0;i<MAX_SIZE;i++){
        IO_BUFFER[i] = c;
    }
        
}

int detect_mode(char *mode){
	int index;
	printf("\n=======================================\n");
	if (strcmp(mode,"s") == 0){
		index = 0;
		printf("MODE      :  Sequential");
	}
	else if (strcmp(mode, "r") == 0){
		index = 1;
		printf("MODE      : Random");
	}
	else{
		printf("Usage is ./test <s | r> <block_size> <nb_threads>");
		exit(0);
	}
return index;
}
