#pragma once

#include <glad/glad.h>
#include <vector>

#include <stdexcept>

enum VertexPropertyType
{
    Float = GL_FLOAT,
    Double = GL_DOUBLE,
    Int = GL_INT,
    UnsignedInt = GL_UNSIGNED_INT,
    Short = GL_SHORT,
    UnsignedShort = GL_UNSIGNED_SHORT,
    Byte = GL_BYTE,
    UnsignedByte = GL_UNSIGNED_BYTE
};

struct VertexProperty
{
    size_t index;
    size_t size;
    VertexPropertyType type;
    bool normalized;
    size_t stride;
    size_t offset;  
};

enum class RenderMode
{
    Triangles = GL_TRIANGLES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Lines = GL_LINES,
    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    Points = GL_POINTS
};

class Mesh
{
private:
    GLuint _vertexArrayObject;
    GLuint _vertexBufferObject;
    GLuint _indexBufferObject;

    size_t _vertexCount;
    size_t _indexCount;

    RenderMode _renderMode;

public:
    Mesh();

    void setRenderMode(RenderMode renderMode);

    void setVertices(const void *vertices, const size_t vertexCount, const size_t vertexSize);
    void setVertexProperties(const std::vector<VertexProperty> &properties);
    void setIndices(const std::vector<GLuint> &indices);

    GLuint getVertexArrayObject() const;
    GLuint getVertexBufferObject() const;
    GLuint getIndexBufferObject() const;

    size_t getVertexCount() const;
    size_t getIndexCount() const;

    void render() const;

    ~Mesh();
};