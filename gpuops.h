#ifndef _GPUOPS_H_
#define _GPUOPS_H_

typedef struct TestResult {
	double time_int;
	double time_float;
	double bandwidth[3];
} TestResult;

extern TestResult gpu_test();

#endif
