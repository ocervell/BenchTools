#include <time.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <iostream>

#define N 1000

const int N_TESTS = 3;
const float CONST_FLOAT = 0.7;

//Stores the results for each of the tests
double time_int[] = {0.0, 0.0, 0.0};
double time_float[] = {0.0, 0.0, 0.0};

//Performs N^3 operations
void *integer_op(void *threadid){
	int A[N][N];
	for(int i=0;i<N;i++)
        {
        	for(int j=0;j<N;j++)
                {
			A[i][j] = 0;
			for(int k = 0; k < N; k++)
			{
                        	A[i][j]++;
				A[j][i]++;
                        	A[i][j]++;
				A[j][i]++;
				A[i][j]++;

                                A[j][i]++;
                                A[i][j]++;
                                A[j][i]++;
                                A[i][j]++;
                                A[j][i]++;
			}
                }
        }
}

//Performs N^3 operations
void *float_op(void *threadid){
	float A[N][N];

	for(int i=0;i<N;i++)
        {
        	for(int j=0;j<N;j++)
                {
			A[i][j] = 0.0;
			for(int k = 0; k < N; k++)
			{
                        	A[i][j]+=CONST_FLOAT;
                        	A[j][i]+=CONST_FLOAT;
                        	A[i][j]+=CONST_FLOAT;
                        	A[j][i]+=CONST_FLOAT;
                        	A[i][j]+=CONST_FLOAT;
				
				A[i][j]+=CONST_FLOAT;
                                A[j][i]+=CONST_FLOAT;
                                A[i][j]+=CONST_FLOAT;
                                A[j][i]+=CONST_FLOAT;
                                A[i][j]+=CONST_FLOAT;
			}
                }
        }
}

void cpu_test(int n_threads, int test_number){
	pthread_t threads[n_threads];
	
	int rc;
	int i;
	long int total_time = 0;

	//INTEGER OPERATIONS
	printf("\nRunning operations on %d threads . . .", n_threads);
	clock_t start = clock(), diff;
    	for (i=0; i<n_threads; i++){
		rc = pthread_create(&threads[i], NULL, integer_op, (void *)i);
    	}
	for (int i=0; i<n_threads; i++){
       		(void) pthread_join(threads[i], NULL);
    	}
	diff = clock() - start;
	time_int[test_number] = (double) diff / (double) CLOCKS_PER_SEC;
	
	//FLOAT OPERATIONS
	start = clock();
    	for (i=0; i<n_threads; i++){
		rc = pthread_create(&threads[i], NULL, float_op, (void *)i);
    	}
	for (int i=0; i<n_threads; i++){
       		(void) pthread_join(threads[i], NULL);
    	}
	diff = clock() - start;
	time_float[test_number] = (double) diff / (double) CLOCKS_PER_SEC;
}

float average(double values[3]){
	double sum = 0;
	for(int i = 0; i < N_TESTS; i++){
                sum += values[i];
	}
        return sum/N_TESTS;
}

float stand_deviation(double values[3], float average ){
	float variance = 0.00;
	for(int i =0; i < N_TESTS; i++){
                variance += (pow((average - values[i]), 2));
	}
	variance = variance / N_TESTS;
	return sqrt( variance );
}

double get_gops(int n_threads, double time) {	
	unsigned long int n = N;
	return (pow(n,3)/pow(10,9))*(n_threads/time)*10;
}

//Print results for n operations
void print_results(int n_threads, int test_number){
	printf("\n  Integer operations");
	printf("\n    Time spent: %f seconds", time_int[test_number]);
	printf("\n    GIOPS: %f\n", get_gops(n_threads, time_int[test_number]));
	
	printf("\n  Float operations");
	printf("\n    Time spent: %f seconds", time_float[test_number]);
	printf("\n    FLOPS: %f\n", get_gops(n_threads, time_float[test_number]));
}

int main(int argc, char *argv[]) {
	if(argc != 2)
	{
		printf("\nUsage: <#thread>\n");
		return 1;
	}

	int n_threads = atoi(argv[1]);
	for(int i=0; i<N_TESTS; i++) {
		printf("\n---- Running test #%d ----\n", i+1);
		cpu_test(n_threads, i);
		print_results(n_threads, i);
	}
		
	float avg_int = average(time_int);
	float avg_float = average(time_float);
	float sd_int = stand_deviation(time_int, avg_int);
	float sd_float = stand_deviation(time_float, avg_float);
	
        printf("\n========== SUMMARY OF CPU PERFORMANCE ============");
        printf("\nTests                                    : %d", N_TESTS);
	printf("\nNumber of threads                        : %d", n_threads);
        printf("\nAverage time for integer operations      : %fs", avg_int);
        printf("\nStandard deviation for integer operations: %fs", sd_int);
	printf("\nAverage integer operations per second    : %f GIOPS", get_gops(n_threads, avg_int));
        printf("\nAverage time for float operations        : %fs", avg_float);
        printf("\nStandard deviation for float operations  : %fs", sd_float);
	printf("\nAverage float operations per second      : %f GFLOPS", get_gops(n_threads, avg_float));
        printf("\n====================================================\n\n");
}
