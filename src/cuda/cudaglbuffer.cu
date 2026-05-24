#include<cudaglbuffer.cuh>

CudaGLBuffer::CudaGLBuffer(GLuint buffer, CudaBufferAccess access)
{
    _buffer = buffer;
    _access = access;
    _cudaResource = nullptr;

    cudaGraphicsGLRegisterBuffer(&_cudaResource, _buffer, static_cast<unsigned int>(_access));
}

void CudaGLBuffer::cudaCheck(cudaError_t code)
{
    if (code != cudaSuccess)
    {
        throw std::runtime_error(std::string("CUDA error: ") + cudaGetErrorString(code));
    }
}

void CudaGLBuffer::map()
{
    cudaCheck(cudaGraphicsMapResources(1, &_cudaResource, 0));
}

void CudaGLBuffer::unmap()
{
    cudaCheck(cudaGraphicsUnmapResources(1, &_cudaResource, 0));
}

GLuint CudaGLBuffer::getVBO() const
{
    return _buffer;
}

CudaGLBuffer::~CudaGLBuffer()
{
    if (_cudaResource)
    {
        cudaGraphicsUnregisterResource(_cudaResource);
        _cudaResource = nullptr;
    }
}