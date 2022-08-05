#ifdef __CUDA__
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

typedef struct {
	CUdevice device;
	CUcontext context;
} CudaDevice_s;

qboolean InitCUDA ( void );

#endif //__CUDA__
