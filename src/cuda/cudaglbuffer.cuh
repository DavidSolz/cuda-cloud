#pragma once

#include <stdexcept>

#include <glad/glad.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

enum class CudaBufferAccess
{
    ReadWrite = cudaGraphicsMapFlagsNone,
    ReadOnly = cudaGraphicsMapFlagsReadOnly,
    WriteOnly = cudaGraphicsMapFlagsWriteDiscard
};

class CudaGLBuffer
{
private:

    GLuint _buffer;
    cudaGraphicsResource* _cudaResource;
    CudaBufferAccess _access;

    static void cudaCheck(cudaError_t code);

public:
    CudaGLBuffer(GLuint buffer, CudaBufferAccess access = CudaBufferAccess::ReadWrite);
    
    void map();
    
    template<typename T>
    T* getMappedPointer(size_t* sizeBytes = nullptr) const
    {
        void* ptr = nullptr;
        size_t size = 0;

        cudaCheck(cudaGraphicsResourceGetMappedPointer(&ptr, &size, _cudaResource));

        if (sizeBytes)
            *sizeBytes = size;

        return static_cast<T*>(ptr);
    }
    
    void unmap();
    
    GLuint getVBO() const;
    
    ~CudaGLBuffer();
};