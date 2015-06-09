#include <time.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "gpuops.h"

const int N = 20 * (1 << 20);
const int N_TESTS = 3;
const float CONST_FLOAT = 0.7;

//Stores the results for each of the tests
double time_int[] = {0.0, 0.0, 0.0};
double time_float[] = {0.0, 0.0, 0.0};
double bandw_b[] = {0.0, 0.0, 0.0};
double bandw_kb[] = {0.0, 0.0, 0.0};
double bandw_mb[] = {0.0, 0.0, 0.0};

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

double get_gops(double time) {
        return (2*N)/(time*1e5); //(20*N)/(time*1e6);
}


int main(int argc, char *argv[]) {
        TestResult result;
	for(int i=0; i<N_TESTS; i++) {
                printf("\n---- Running test #%d ----\n", i+1);
                result = gpu_test();
		time_int[i] = result.time_int;
		time_float[i] = result.time_float;
		bandw_b[i] = result.bandwidth[0];
		bandw_kb[i] = result.bandwidth[1];
		bandw_mb[i] = result.bandwidth[2];
        }

        float avg_int = average(time_int);
        float avg_float = average(time_float);
        float sd_int = stand_deviation(time_int, avg_int);
        float sd_float = stand_deviation(time_float, avg_float);
	float avg_bandw_b = average(bandw_b);
	float avg_bandw_kb = average(bandw_kb);
	float avg_bandw_mb = average(bandw_mb);
	float sd_bandw_b = stand_deviation(bandw_b, avg_bandw_b);
	float sd_bandw_kb = stand_deviation(bandw_kb, avg_bandw_kb);
	float sd_bandw_mb = stand_deviation(bandw_mb, avg_bandw_mb);

        printf("\n========== SUMMARY OF GPU PERFORMANCE ============");
        printf("\nTests                                    : %d", N_TESTS);
        printf("\nAverage time for integer operations      : %fms", avg_int);
        printf("\nStandard deviation for integer operations: %fms", sd_int);
        printf("\nAverage integer operations per second    : %f GIOPS", get_gops(avg_int));
        printf("\n\nAverage time for float operations        : %fms", avg_float);
        printf("\nStandard deviation for float operations  : %fms", sd_float);
        printf("\nAverage float operations per second      : %f GFLOPS", get_gops(avg_float));
        printf("\n\nAverage bandwidth (1B test)	 	   : %f ", avg_bandw_b);
        printf("\nStandard Deviation (1B test)	 	   : %f ", sd_bandw_b);
        printf("\nAverage bandwidth (1KB test)	 	   : %f ", avg_bandw_kb);
        printf("\nStandard Deviation (1KB test)	 	   : %f ", sd_bandw_kb);
        printf("\nAverage bandwidth (1MB test)	 	   : %f ", avg_bandw_mb);
        printf("\nStandard Deviation (1MB test)	 	   : %f ", sd_bandw_mb);
        printf("\n====================================================\n\n");
}

