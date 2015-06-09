#include "gpuops.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define INT_M 2

const int N = 20 * (1 << 20);

double get_time()
{
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec*1e-6;
}

//float operations
__global__ void add_float(int n, float a, float *x, float *y, float *z)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n) {
	y[i] = a*x[i] + y[i];//y=4
	z[i] = a*x[i] + z[i];//z=5
	z[i] = a*y[i] + z[i];//z=13
	y[i] = a*x[i] + z[i];//y=15
        y[i] = a*x[i] + y[i];//y=17
	}
}

//int operations
__global__ void add_int(int n, int a, int *x, int *y, int *z)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n) {
	y[i] = a*x[i] + y[i];//y=4
	z[i] = a*x[i] + z[i];//z=5
	z[i] = a*y[i] + z[i];//z=13
        y[i] = a*x[i] + z[i];//y=15
        y[i] = a*x[i] + y[i];//y=17
	}
}

float speed_test_int(int n_blocks, int n_cores){
	int *x, *y, *z, *d_x, *d_y, *d_z;
	x = (int*)malloc(N*sizeof(int));
	y = (int*)malloc(N*sizeof(int));
	z = (int*)malloc(N*sizeof(int));

	cudaMalloc(&d_x, N*sizeof(int)); 
	cudaMalloc(&d_y, N*sizeof(int));
	cudaMalloc(&d_z, N*sizeof(int));

	for (int i = 0; i < N; i++) {
		x[i] = 1;
		y[i] = 2;
		z[i] = 3;
	}

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaMemcpy(d_x, x, N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_y, y, N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_z, z, N*sizeof(int), cudaMemcpyHostToDevice);

	cudaEventRecord(start);

	printf("\nPerforming integer operation on %d elements . . .\n", N);
	//blocks, cores
	add_int<<<n_blocks, n_cores>>>(N, 2, d_x, d_y, d_z);

	cudaEventRecord(stop);

	cudaMemcpy(y, d_y, N*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(z, d_z, N*sizeof(int), cudaMemcpyDeviceToHost);

	cudaEventSynchronize(stop);
	float mseconds = 0;
	cudaEventElapsedTime(&mseconds, start, stop);

	int maxError_y = 0;
	int maxError_z = 0;
	for (int i = 0; i < N; i++) {
	maxError_y = max(maxError_y, abs(y[i]-17));
	maxError_z = max(maxError_z, abs(z[i]-13));
	}

	cudaFree( x );
        cudaFree( y );
        cudaFree( z );

//	printf("\nMax error y: %fn", maxError_y);
//	printf("\nMax error z: %fn", maxError_z);
	printf("\nTime elapsed: %f", mseconds);

	return mseconds;
}

float speed_test_float(int n_blocks, int n_cores){
	float *x, *y, *z, *d_x, *d_y, *d_z;
	x = (float*)malloc(N*sizeof(float));
	y = (float*)malloc(N*sizeof(float));
	z = (float*)malloc(N*sizeof(float));

	cudaMalloc(&d_x, N*sizeof(float)); 
	cudaMalloc(&d_y, N*sizeof(float));
	cudaMalloc(&d_z, N*sizeof(float));

	for (int i = 0; i < N; i++) {
		x[i] = 1.0f;
		y[i] = 2.0f;
		z[i] = 3.0f;
	}

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_y, y, N*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_z, z, N*sizeof(float), cudaMemcpyHostToDevice);

	cudaEventRecord(start);

	printf("\nPerforming float operation on %d elements . . .\n", N);
	//blocks, cores
	add_float<<<n_blocks, n_cores>>>(N, 2.0f, d_x, d_y, d_z);

	cudaEventRecord(stop);

	cudaMemcpy(y, d_y, N*sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(z, d_z, N*sizeof(float), cudaMemcpyDeviceToHost);

	cudaEventSynchronize(stop);
	float mseconds = 0;
	cudaEventElapsedTime(&mseconds, start, stop);

	cudaFree( x );
	cudaFree( y );
	cudaFree( z );

	float maxError_y = 0.0f;
	float maxError_z = 0.0f;
	for (int i = 0; i < N; i++) {
	maxError_y = max(maxError_y, abs(y[i]-17.0f));
	maxError_z = max(maxError_z, abs(z[i]-13.0f));
	}

//	printf("\nMax error y: %fn", maxError_y);
//	printf("\nMax error z: %fn", maxError_z);
	printf("\nTime elapsed: %f", mseconds);

	return mseconds;
}


double bandwidth_test(int size){
	int n = size;
	
        char *x, *d_x, *d_y;
        x = (char*)malloc(n*sizeof(char));

        cudaMalloc(&d_x, n*sizeof(char));
        cudaMalloc(&d_y, n*sizeof(char));

        for (int i = 0; i < n; i++) {
                x[i] = 'a';
        }

        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaMemcpy(d_x, x, n*sizeof(char), cudaMemcpyHostToDevice);

        cudaEventRecord(start);

        printf("\nRuning memory bandwidth test with %d bytes . . .\n", n);

        cudaEventRecord(stop);

        cudaMemcpy(x, d_x, n*sizeof(char), cudaMemcpyDeviceToHost);

        cudaEventSynchronize(stop);
        float mseconds = 0;
        cudaEventElapsedTime(&mseconds, start, stop);

        cudaFree( x );

        printf("\nTime elapsed: %fms", mseconds);
	
	double bandwidth = (n / (mseconds/1000))*1e-9;
        printf("\nBandwidth (GB/s): %f\n", bandwidth);

        return bandwidth;
}

int ConvertSMVer2Cores(int major, int minor)
{
        // Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
        typedef struct {
                int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
                int Cores;
        } sSMtoCores;

        sSMtoCores nGpuArchCoresPerSM[] =
        { { 0x10,  8 }, // Tesla Generation (SM 1.0) G80 class
          { 0x11,  8 }, // Tesla Generation (SM 1.1) G8x class
          { 0x12,  8 }, // Tesla Generation (SM 1.2) G9x class
          { 0x13,  8 }, // Tesla Generation (SM 1.3) GT200 class
          { 0x20, 32 }, // Fermi Generation (SM 2.0) GF100 class
          { 0x21, 48 }, // Fermi Generation (SM 2.1) GF10x class
          { 0x30, 192}, // Fermi Generation (SM 3.0) GK10x class
          {   -1, -1 }
        };

        int index = 0;
        while (nGpuArchCoresPerSM[index].SM != -1) {
                if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) ) {
                        return nGpuArchCoresPerSM[index].Cores;
                }
                index++;
        }
        printf("MapSMtoCores SM %d.%d is undefined (please update to the latest SDK)!\n", major, minor);
        return -1;
}

TestResult gpu_test() {
	TestResult result;
	int dev =0, n_blocks = 2, n_cores = 384; //blocks per grid and threads per block
	
	//Getting number of GPU cores	
	cudaSetDevice(0);
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);
	n_blocks = deviceProp.multiProcessorCount;
	n_cores = ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount;
	printf("\n  (%2d) Multiprocessors, (%3d) CUDA Cores/MP:     %d CUDA Cores\n",
	        n_blocks, ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),n_cores);	
	
	//Running tests
	n_blocks = ceil(N+n_cores)/n_cores;	
	
	printf("\n----- Runing GPU speed test - integer operations -----\n");
	result.time_int = speed_test_int(n_blocks, n_cores);	
	printf("\n----- Runing GPU speed test - float operations -----\n");
	result.time_float = speed_test_float((N+511)/n_cores, n_cores);

	printf("\n----- Runing memory tests -----\n");
	result.bandwidth[0] = bandwidth_test(1);
	result.bandwidth[1] = bandwidth_test(1000);
	result.bandwidth[2] = bandwidth_test(1000000);

	//bandwidth test
	return result;
}


