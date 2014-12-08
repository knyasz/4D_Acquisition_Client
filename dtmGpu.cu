#include "dtm.h"
#include <stdio.h>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>
#include <math.h>

// This will output the proper CUDA error strings in the event that a CUDA host call returns an error
#define checkCudaErrors(err)  __checkCudaErrors (err, __FILE__, __LINE__)

//definition of constant vector depth to world
__constant__ double  depthToWorld[] = {1.0 / 5.9421434211923247e+02, 1.0 / 5.9104053696870778e+02, 3.3930780975300314e+02, 2.4273913761751615e+02};

__constant__ double  worldToColor[] = {5.2921508098293293e+02, 5.2556393630057437e+02, 3.2894272028759258e+02, 2.6748068171871557e+02};


inline void __checkCudaErrors(cudaError err, const char *file, const int line )
{
    if(cudaSuccess != err)
    {
        fprintf(stderr, "%s(%i) : CUDA Runtime API error %d: %s.\n",file, line, (int)err, cudaGetErrorString( err ) );
        exit(-1);
    }
}

/*__device__ void
transposeMat(uchar matIn[][], uchar matOut[][])
{
	uint16 tx(threadIdx.x);
	uint16 ty(threadIdx.y);
	matOut[tx][ty] = matIn[ty][tx];
}

__device__ void
transposeMat(float matIn[][], float matOut[][])
{
	uint16 tx(threadIdx.x);
	uint16 ty(threadIdx.y);
	matOut[tx][ty] = matIn[ty][tx];
}*/

__device__ void
transformByPoint(SWorldPoint*** inPoints, const float* trMat, SWorldPoint*** transformedPoints)
{
	uint16 tx(threadIdx.x);
	uint16 ty(threadIdx.y);
	float tmpPoint[] = {inPoints[ty][tx]->x,inPoints[ty][tx]->y,inPoints[ty][tx]->z};

	float w(tmpPoint[0] * trMat[3] + tmpPoint[1] * trMat[7] + tmpPoint[2] * trMat[11] + trMat[15]);

	if (w)
	{
		float invW = 1.0f / w;
		transformedPoints[ty][tx]->x = (tmpPoint[0] * trMat[0] + tmpPoint[1] * trMat[4] + tmpPoint[2] * trMat[8] + trMat[12]) * invW;
		transformedPoints[ty][tx]->y = (tmpPoint[0] * trMat[1] + tmpPoint[1] * trMat[5] + tmpPoint[2] * trMat[9] + trMat[13]) * invW;
		transformedPoints[ty][tx]->z = (tmpPoint[0] * trMat[2] + tmpPoint[1] * trMat[6] + tmpPoint[2] * trMat[10] + trMat[14]) * invW;
	}
	else
	{
		transformedPoints[ty][tx]->x = tmpPoint[0];
		transformedPoints[ty][tx]->y = tmpPoint[1];
		transformedPoints[ty][tx]->z = tmpPoint[2];
	}
}

__global__ void
dtmKernel(const uint16 *in, uint16 *out,const int minDist, const int maxDist,const int cols, const int rows)
{
	__shared__  float sharedIn[N][N];
	__shared__  float sharedTmp[N][N];
	__shared__  float sharedOut[N][N];

	unsigned int tx(threadIdx.x);
	unsigned int ty(threadIdx.y);
	unsigned int ind(blockIdx.x*blockDim.x + blockIdx.y*blockDim.y*cols + ty*cols + tx);
	//unsigned int tid(ty*blockDim.x + tx);
	

	sharedIn[ty][tx] = in[ind];
	sharedOut[ty][tx] = 0;
	sharedTmp[ty][tx] = 0.f;
	__syncthreads();

	if (sharedIn[ty][tx] < 2047)
	{
		sharedTmp[ty][tx] = static_cast<float>(1.0f / static_cast<float>(sharedIn[ty][tx]*-0.0030711016f + 3.330949516f));


		if ((sharedTmp[ty][tx] >= minDist) && (sharedTmp[ty][tx] <= maxDist))
		{
			sharedOut[ty][tx] = sharedIn[ty][tx];
		}
	}

	__syncthreads();

	//out[ind] = sharedIn[ty][tx];
	out[ind] = static_cast<uint16>(sharedOut[ty][tx]);
}


__global__ void
dtmKernelColor(const uint16 *in, const uchar *inCol ,uchar *out,const int minDist, const int maxDist,const int cols, const int rows)
{
	__shared__  uint16 sharedIn[N][N];
	__shared__  uchar sharedColor[N][N];
	__shared__  float sharedTmp[N][N];
	__shared__  uchar sharedOut[N][N];

	unsigned int tx(threadIdx.x);
	unsigned int ty(threadIdx.y);
	unsigned int ind(blockIdx.x*blockDim.x + blockIdx.y*blockDim.y*cols + ty*cols + tx);
	//unsigned int tid(ty*blockDim.x + tx);
	

	sharedIn[ty][tx] = in[ind];
	sharedOut[ty][tx] = 0;
	sharedTmp[ty][tx] = 0.f;
	sharedColor[ty][tx] = inCol[ind];
	__syncthreads();

	if (sharedIn[ty][tx] < 2047)
	{
		sharedTmp[ty][tx] = static_cast<float>(1.0 / static_cast<double>(sharedIn[ty][tx]*-0.0030711016 + 3.330949516));


		if ((sharedTmp[ty][tx] >= minDist) && (sharedTmp[ty][tx] <= maxDist))
		{
			sharedOut[ty][tx] = sharedColor[ty][tx];
		}
	}

	__syncthreads();

	out[ind] = sharedOut[ty][tx];
}

__global__ void
dtmKernelColorAlternative(const uint16 *in, uchar *inCol,const int minDist, const int maxDist,const int cols, const int rows)
{
	__shared__  float sharedIn[N][N];
	__shared__  float sharedColor[N][N];
	__shared__  float sharedTmp[N][N];

	unsigned int tx(threadIdx.x);
	unsigned int ty(threadIdx.y);
	unsigned int ind(blockIdx.x*blockDim.x + blockIdx.y*blockDim.y*cols + ty*cols + tx);
	//unsigned int tid(ty*blockDim.x + tx);


	sharedIn[ty][tx] = in[ind];
	sharedTmp[ty][tx] = 0.f;
	sharedColor[ty][tx] = inCol[ind];
	__syncthreads();

	if (sharedIn[ty][tx] < 2047)
	{
		sharedTmp[ty][tx] = static_cast<float>(1.0 / static_cast<float>(sharedIn[ty][tx]*-0.0030711016f + 3.330949516f));


		if ((sharedTmp[ty][tx] < minDist) || (sharedTmp[ty][tx] > maxDist))
		{
			//sharedColor[ty][tx] = 0;
			inCol[ind] = 0;
		}
	}

	__syncthreads();

	//inCol[ind] = static_cast<uchar>(sharedColor[ty][tx]);
}

__global__ void
depthToWorldColorKernel(const uint16 *in, const float *trMat ,SWorldPoint *out,const int minDist, const int maxDist,const int cols, const int rows)
{
	__shared__  uint16 sharedIn[N][N];
	__shared__  SWorldPoint* sharedOut[N][N];
	__shared__  float sharedTmp[N][N];

	unsigned int tx(threadIdx.x);
	unsigned int ty(threadIdx.y);
	unsigned int ind(blockIdx.x*blockDim.x + blockIdx.y*blockDim.y*cols + ty*cols + tx);
	//unsigned int tid(ty*blockDim.x + tx);


	sharedIn[ty][tx] = in[ind];
	sharedOut[ty][tx] = &out[ind];
	__syncthreads();

	//calculate the depth to meters
	if (sharedIn[ty][tx] < 2047)
	{
		sharedTmp[ty][tx] = static_cast<float>(1.0 / static_cast<double>(sharedIn[ty][tx]*-0.0030711016 + 3.330949516));


		if ((sharedTmp[ty][tx] < minDist) || (sharedTmp[ty][tx] > maxDist))
		{
			sharedTmp[ty][tx] = 0.f;
		}
	}

	__syncthreads();

	//calculate the real world data
	sharedOut[ty][tx]->x = (ty - depthToWorld[CX]) * sharedTmp[ty][tx] * depthToWorld[FX];
	sharedOut[ty][tx]->y = (tx - depthToWorld[CY]) * sharedTmp[ty][tx] * depthToWorld[FY];
	sharedOut[ty][tx]->z = sharedTmp[ty][tx];

	__syncthreads();

	//calc the transformed matrix
	transformByPoint(reinterpret_cast<SWorldPoint***>(sharedOut), trMat, reinterpret_cast<SWorldPoint***>(sharedOut));

	__syncthreads();

	//calc the colored pixels

	out[ind].x = round((sharedOut[ty][tx]->x * worldToColor[FX] * (1.f / sharedOut[ty][tx]->z)) + worldToColor[CX]);
	out[ind].y = round((sharedOut[ty][tx]->y * worldToColor[FY] * (1.f / sharedOut[ty][tx]->z)) + worldToColor[CY]);
}


float dtmGpu( uchar* h_in, uchar* h_out, const int rows, const int cols, const int minDist, const int maxDist)
{
	const int SIZE = rows * cols * sizeof(uint16);
	cudaEvent_t start, stop;
	
	checkCudaErrors( cudaEventCreate(&start) );
	checkCudaErrors( cudaEventCreate(&stop) );

	// Allocate the device input image
	uchar *d_in = NULL;
	checkCudaErrors( cudaMalloc((void **)&d_in, SIZE) );

	// Allocate the device output image
	uchar *d_out = NULL;
	checkCudaErrors( cudaMalloc((void **)&d_out, SIZE) );

	// Copy the host input image  to the device memory
	checkCudaErrors( cudaMemcpy(d_in, h_in, SIZE, cudaMemcpyHostToDevice) );

	checkCudaErrors( cudaEventRecord(start, NULL) );

	// Launch the CUDA Kernel
	dim3 block(N, N);
	dim3 grid(cols / N, rows / N);
	dtmKernel<<<grid, block>>>(reinterpret_cast<uint16*>(d_in), reinterpret_cast<uint16*>(d_out), minDist, maxDist,cols,rows);
	checkCudaErrors( cudaGetLastError() );

	checkCudaErrors( cudaEventRecord(stop, NULL) );
	checkCudaErrors( cudaEventSynchronize(stop) );

	// Copy the device result to the host
	checkCudaErrors( cudaMemcpy(h_out, d_out, SIZE, cudaMemcpyDeviceToHost) );

	checkCudaErrors( cudaFree(d_in) );
	checkCudaErrors( cudaFree(d_out) );

	float msec = 0.f;
	checkCudaErrors( cudaEventElapsedTime(&msec, start, stop) );

//#ifdef __CPU_VERSION__
	//printf("the gpu version takes %fl\n", msec);
//#endif
	return msec;
}

void dtmGpuColor( uint16* h_in, uchar* h_inCol,uchar* h_outCol, const int rows, const int cols, const int minDist, const int maxDist)
{
	const int SIZEU16 = rows * cols * sizeof(uint16);
	const int SIZEU8 = rows * cols * sizeof(uchar);
	cudaEvent_t start, stop;
	
	checkCudaErrors( cudaEventCreate(&start) );
	checkCudaErrors( cudaEventCreate(&stop) );

	// Allocate the device input images
	uint16 *d_in = NULL;
	checkCudaErrors( cudaMalloc((void **)&d_in, SIZEU16) );
	uchar *d_inCol = NULL;
	checkCudaErrors( cudaMalloc((void **)&d_inCol, SIZEU8) );

	// Allocate the device output image
	uchar *d_out = NULL;
	checkCudaErrors( cudaMalloc((void **)&d_out, SIZEU8) );

	// Copy the host input image  to the device memory
	checkCudaErrors( cudaMemcpy(d_in, h_in, SIZEU16, cudaMemcpyHostToDevice) );
	checkCudaErrors( cudaMemcpy(d_inCol, h_inCol, SIZEU8, cudaMemcpyHostToDevice) );

	checkCudaErrors( cudaEventRecord(start, NULL) );

	// Launch the CUDA Kernel
	dim3 block(N, N);
	dim3 grid(cols / N, rows / N);
	//dtmKernelColor<<<grid, block>>>(d_in, d_inCol, d_out, minDist, maxDist,cols,rows);
	dtmKernelColorAlternative<<<grid, block>>>(d_in,d_inCol,minDist, maxDist,cols,rows);
	checkCudaErrors( cudaGetLastError() );

	checkCudaErrors( cudaEventRecord(stop, NULL) );
	checkCudaErrors( cudaEventSynchronize(stop) );

	// Copy the device result to the host
	//checkCudaErrors( cudaMemcpy(h_outCol, d_out, SIZEU8, cudaMemcpyDeviceToHost) );
	checkCudaErrors( cudaMemcpy(h_outCol, d_in, SIZEU8, cudaMemcpyDeviceToHost) );

	checkCudaErrors( cudaFree(d_in) );
	checkCudaErrors( cudaFree(d_inCol) );
	checkCudaErrors( cudaFree(d_out) );

	float msec = 0.f;
	checkCudaErrors( cudaEventElapsedTime(&msec, start, stop) );

	//printf("This took us %fl", msec);
}

void depthToRgbWorldPoint(uchar* h_in, float* h_transMat ,uchar* h_out, const int rows, const int cols, const int minDist, const int maxDist)
{
		const int SIZEU16 = rows * cols * sizeof(uint16);
		const int SIZEWP = rows * cols * sizeof(SWorldPoint);
		const int TRMATRC = 4;
		const int SIZETRMAT = TRMATRC*TRMATRC*sizeof(float);
		cudaEvent_t start, stop;


		checkCudaErrors( cudaEventCreate(&start) );
		checkCudaErrors( cudaEventCreate(&stop) );

		// Allocate the device input images
		uchar *d_in = NULL;
		checkCudaErrors( cudaMalloc((void **)&d_in, SIZEU16) );
		uchar *d_trMat = NULL;
		checkCudaErrors( cudaMalloc((void **)&d_trMat, SIZETRMAT) );

		// Allocate the device output image
		uchar *d_out = NULL;
		checkCudaErrors( cudaMalloc((void **)&d_out, SIZEWP) );

		// Copy the host input image  to the device memory
		checkCudaErrors( cudaMemcpy(d_in, h_in, SIZEU16, cudaMemcpyHostToDevice) );
		checkCudaErrors( cudaMemcpy(d_trMat, h_transMat, SIZETRMAT, cudaMemcpyHostToDevice) );

		checkCudaErrors( cudaEventRecord(start, NULL) );

		// Launch the CUDA Kernel
		dim3 block(N, N);
		dim3 grid(cols / N, rows / N);
		depthToWorldColorKernel<<<grid, block>>>(reinterpret_cast<uint16*>(d_in), reinterpret_cast<float*>(d_trMat), reinterpret_cast<SWorldPoint*>(d_out), minDist, maxDist,cols,rows);
		checkCudaErrors( cudaGetLastError() );

		checkCudaErrors( cudaEventRecord(stop, NULL) );
		checkCudaErrors( cudaEventSynchronize(stop) );

		// Copy the device result to the host
		checkCudaErrors( cudaMemcpy(h_out, d_out, SIZEWP, cudaMemcpyDeviceToHost) );

		checkCudaErrors( cudaFree(d_in) );
		checkCudaErrors( cudaFree(d_trMat) );
		checkCudaErrors( cudaFree(d_out) );

		float msec = 0.f;
		checkCudaErrors( cudaEventElapsedTime(&msec, start, stop) );

		//printf("This took us %fl", msec);
	}


