#include <mesh.hpp>

Mesh::Mesh()
{
    _vertexArrayObject = 0;
    _vertexBufferObject = 0;
    _indexBufferObject = 0;

    _vertexCount = 0;
    _indexCount = 0;

    _renderMode = RenderMode::Triangles;
}

void Mesh::setRenderMode(RenderMode renderMode)
{
    _renderMode = renderMode;
}

void Mesh::setVertices(const void *vertices, const size_t vertexCount, const size_t vertexSize)
{
    if(vertexCount == 0)
    {
        throw std::invalid_argument("Vertex count cannot be zero.");
    }

    if(_vertexArrayObject == 0)
    {
        glGenVertexArrays(1, &_vertexArrayObject);
    }

    glBindVertexArray(_vertexArrayObject);

    if(_vertexBufferObject == 0)
    {
        glGenBuffers(1, &_vertexBufferObject);
    }

    _vertexCount = vertexCount;

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::setVertexProperties(const std::vector<VertexProperty> &properties)
{
    if(properties.empty())
    {
        throw std::invalid_argument("Vertex properties cannot be empty.");
    }

    if(_vertexArrayObject == 0)
    {
        glGenVertexArrays(1, &_vertexArrayObject);
    }

    glBindVertexArray(_vertexArrayObject);

    for(const auto &property : properties)
    {
        glEnableVertexAttribArray(property.index);
        glVertexAttribPointer(property.index, property.size, static_cast<GLenum>(property.type), property.normalized, property.stride, (void *)property.offset);
    }

    glBindVertexArray(0);
}

void Mesh::setIndices(const std::vector<GLuint> &indices)
{
    if(indices.empty())
    {
        throw std::invalid_argument("Indices cannot be empty.");
    }

    if(_indexBufferObject == 0)
    {
        glGenBuffers(1, &_indexBufferObject);
    }

    _indexCount = indices.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::render() const
{
    glBindVertexArray(_vertexArrayObject);

    if(_indexBufferObject != 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject);
        glDrawElements(static_cast<GLenum>(_renderMode), _indexCount, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        glDrawArrays(static_cast<GLenum>(_renderMode), 0, _vertexCount);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    if(_vertexBufferObject != 0)
    {
        glDeleteBuffers(1, &_vertexBufferObject);
    }

    if(_indexBufferObject != 0)
    {
        glDeleteBuffers(1, &_indexBufferObject);
    }

    if(_vertexArrayObject != 0)
    {
        glDeleteVertexArrays(1, &_vertexArrayObject);
    }
}