#pragma once

#include <Windows.h>

#include <cuda.h>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#include <structmember.h>
#define _DEBUG
#else
#include <Python.h>
#include <structmember.h>
#endif

#define BUILDING_HOST
#include "DSP.h"

#define DEBUG printf

#define PY_ASSERT(EVAL, RETVAL, ...)										\
if( !(EVAL) )																\
{																			\
	static char buffer[4096];												\
	sprintf_s(buffer, "assertion "#EVAL	": "__VA_ARGS__);					\
	PyErr_SetString(cuda_exception, buffer);								\
	return RETVAL;															\
}


/***************************************************************************************************
*
***************************************************************************************************/

#define CU_BEGIN(FNC_NAME)													\
	const char __cuda_function[] = #FNC_NAME;								\
	const char* __cuda_error_call = NULL;									\
	const char* __cuda_error_file = NULL;									\
	unsigned __cuda_error_line = 0;											\
	CUresult __cuda_result = CUDA_SUCCESS;

#define CU_END(RETVAL, ...)													\
__cudaError:																\
	static char __cuda_error_buffer[4096];									\
	sprintf_s(__cuda_error_buffer, "%s %s %d\n%s: %s",						\
		__cuda_error_file,													\
		__cuda_function,													\
		__cuda_error_line,													\
		__cuda_error_call,													\
		cuGetErrorString(__cuda_result));									\
	PyErr_SetString(cuda_exception, __cuda_error_buffer);					\
	__VA_ARGS__;															\
	return RETVAL;


/***************************************************************************************************
*
***************************************************************************************************/

#define CU_ASSERT(EVAL, RETVAL, ...)										\
if( !(EVAL) )																\
{																			\
	static char buffer[4096];												\
	sprintf_s(buffer, "%s: assertion "#EVAL,								\
		__cuda_function,													\
		__VA_ARGS__);														\
	PyErr_SetString(cuda_exception, buffer);								\
	return RETVAL;															\
}

/***************************************************************************************************
*
***************************************************************************************************/

#define CU_CALL(CALL)														\
if( __cuda_result = (CALL) )												\
{																			\
	__cuda_error_call = #CALL;												\
	__cuda_error_file = __FILE__;											\
	__cuda_error_line = __LINE__;											\
	goto __cudaError;														\
}

/***************************************************************************************************
*
***************************************************************************************************/

struct CudaDevice
{
	char name[56];
	CUdevice handle;
	CUdevprop_st prop;
};

/***************************************************************************************************
*
***************************************************************************************************/

extern PyObject* cuda_exception;
extern unsigned long cuda_version;
extern unsigned long cuda_device_count;
extern CudaDevice* cuda_device;

/***************************************************************************************************
*
***************************************************************************************************/

inline const char* cuGetErrorString(CUresult result)
{
	switch(result)
	{
#define CU_ERROR_LIST(CASE, MSG) case CASE: return #CASE ": " MSG; 
		CU_ERROR_LIST(CUDA_SUCCESS, "The API call returned with no errors. In the case of query calls, this can also mean that the operation being queried is complete (see ::cuEventQuery() and ::cuStreamQuery()).")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_VALUE, "This indicates that one or more of the parameters passed to the API call is not within an acceptable range of values.")
		CU_ERROR_LIST(CUDA_ERROR_OUT_OF_MEMORY, "The API call failed because it was unable to allocate enough memory to perform the requested operation.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_INITIALIZED, "This indicates that the CUDA driver has not been initialized with ::cuInit() or that initialization has failed.")
		CU_ERROR_LIST(CUDA_ERROR_DEINITIALIZED, "This indicates that the CUDA driver is in the process of shutting down.")
		CU_ERROR_LIST(CUDA_ERROR_PROFILER_DISABLED, "This indicates profiling APIs are called while application is running in visual profiler mode.")
		CU_ERROR_LIST(CUDA_ERROR_PROFILER_NOT_INITIALIZED, "This indicates profiling has not been initialized for this context. Call cuProfilerInitialize() to resolve this.")
		CU_ERROR_LIST(CUDA_ERROR_PROFILER_ALREADY_STARTED, "This indicates profiler has already been started and probably cuProfilerStart() is incorrectly called.")
		CU_ERROR_LIST(CUDA_ERROR_PROFILER_ALREADY_STOPPED, "This indicates profiler has already been stopped and probably cuProfilerStop() is incorrectly called.")
		CU_ERROR_LIST(CUDA_ERROR_NO_DEVICE, "This indicates that no CUDA-capable devices were detected by the installed CUDA driver.")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_DEVICE, "This indicates that the device ordinal supplied by the user does not correspond to a valid CUDA device.")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_IMAGE, "This indicates that the device kernel image is invalid. This can also indicate an invalid CUDA module.")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_CONTEXT, "This most frequently indicates that there is no context bound to the current thread. This can also be returned if the context passed to an API call is not a valid handle (such as a context that has had ::cuCtxDestroy() invoked on it). This can also be returned if a user mixes different API versions (i.e. 3010 context with 3020 API calls). See ::cuCtxGetApiVersion() for more details.")
		CU_ERROR_LIST(CUDA_ERROR_CONTEXT_ALREADY_CURRENT, "This indicated that the context being supplied as a parameter to the API call was already the active context. This error return is deprecated as of CUDA 3.2. It is no longer an error to attempt to push the active context via ::cuCtxPushCurrent().")
		CU_ERROR_LIST(CUDA_ERROR_MAP_FAILED, "This indicates that a map or register operation has failed.")
		CU_ERROR_LIST(CUDA_ERROR_UNMAP_FAILED, "This indicates that an unmap or unregister operation has failed.")
		CU_ERROR_LIST(CUDA_ERROR_ARRAY_IS_MAPPED, "This indicates that the specified array is currently mapped and thus cannot be destroyed.")
		CU_ERROR_LIST(CUDA_ERROR_ALREADY_MAPPED, "This indicates that the resource is already mapped.")
		CU_ERROR_LIST(CUDA_ERROR_NO_BINARY_FOR_GPU, "This indicates that there is no kernel image available that is suitable for the device. This can occur when a user specifies code generation options for a particular CUDA source file that do not include the corresponding device configuration.")
		CU_ERROR_LIST(CUDA_ERROR_ALREADY_ACQUIRED, "This indicates that a resource has already been acquired.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_MAPPED, "This indicates that a resource is not mapped.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_MAPPED_AS_ARRAY, "This indicates that a mapped resource is not available for access as an array.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_MAPPED_AS_POINTER, "This indicates that a mapped resource is not available for access as a pointer.")
		CU_ERROR_LIST(CUDA_ERROR_ECC_UNCORRECTABLE, "This indicates that an uncorrectable ECC error was detected during execution.")
		CU_ERROR_LIST(CUDA_ERROR_UNSUPPORTED_LIMIT, "This indicates that the ::CUlimit passed to the API call is not supported by the active device.")
		CU_ERROR_LIST(CUDA_ERROR_CONTEXT_ALREADY_IN_USE, "This indicates that the ::CUcontext passed to the API call can only be bound to a single CPU thread at a time but is already bound to a CPU thread.")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_SOURCE, "This indicates that the device kernel source is invalid.")
		CU_ERROR_LIST(CUDA_ERROR_FILE_NOT_FOUND, "This indicates that the file specified was not found.")
		CU_ERROR_LIST(CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND, "This indicates that a link to a shared object failed to resolve.")
		CU_ERROR_LIST(CUDA_ERROR_SHARED_OBJECT_INIT_FAILED, "This indicates that initialization of a shared object failed.")
		CU_ERROR_LIST(CUDA_ERROR_OPERATING_SYSTEM, "This indicates that an OS call failed.")
		CU_ERROR_LIST(CUDA_ERROR_INVALID_HANDLE, "This indicates that a resource handle passed to the API call was not valid. Resource handles are opaque types like ::CUstream and ::CUevent.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_FOUND, "This indicates that a named symbol was not found. Examples of symbols are global/constant variable names, texture names, and surface names.")
		CU_ERROR_LIST(CUDA_ERROR_NOT_READY, "This indicates that asynchronous operations issued previously have not completed yet. This result is not actually an error, but must be indicated differently than ::CUDA_SUCCESS (which indicates completion). Calls that may return this value include ::cuEventQuery() and ::cuStreamQuery().")
		CU_ERROR_LIST(CUDA_ERROR_LAUNCH_FAILED, "An exception occurred on the device while executing a kernel. Common causes include dereferencing an invalid device pointer and accessing out of bounds shared memory. The context cannot be used, so it must be destroyed (and a new one should be created). All existing device memory allocations from this context are invalid and must be reconstructed if the program is to continue using CUDA.")
		CU_ERROR_LIST(CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES, "This indicates that a launch did not occur because it did not have appropriate resources. This error usually indicates that the user has attempted to pass too many arguments to the device kernel, or the kernel launch specifies too many threads for the kernel's register count. Passing arguments of the wrong size (i.e. a 64-bit pointer when a 32-bit int is expected) is equivalent to passing too many arguments and can also result in this error.")
		CU_ERROR_LIST(CUDA_ERROR_LAUNCH_TIMEOUT, "This indicates that the device kernel took too long to execute. This can only occur if timeouts are enabled - see the device attribute ::CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT for more information. The context cannot be used (and must be destroyed similar to ::CUDA_ERROR_LAUNCH_FAILED). All existing device memory allocations from this context are invalid and must be reconstructed if the program is to continue using CUDA.")
		CU_ERROR_LIST(CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING, "This error indicates a kernel launch that uses an incompatible texturing mode.")
		CU_ERROR_LIST(CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED, "This error indicates that a call to ::cuCtxEnablePeerAccess() is trying to re-enable peer access to a context which has already had peer access to it enabled.")
		CU_ERROR_LIST(CUDA_ERROR_PEER_ACCESS_NOT_ENABLED, "This error indicates that ::cuCtxDisablePeerAccess() is trying to disable peer access which has not been enabled yet via ::cuCtxEnablePeerAccess(). ")
		CU_ERROR_LIST(CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE, "This error indicates that the primary context for the specified device has already been initialized.")
		CU_ERROR_LIST(CUDA_ERROR_CONTEXT_IS_DESTROYED, "This error indicates that the context current to the calling thread has been destroyed using ::cuCtxDestroy, or is a primary context which has not yet been initialized.")
		default:
		CU_ERROR_LIST(CUDA_ERROR_UNKNOWN, "This indicates that an unknown internal error has occurred.")
#undef	CU_ERROR_LIST
	}

	// this must be unreachable, never return null string
	return static_cast<const char*>(0);
}

