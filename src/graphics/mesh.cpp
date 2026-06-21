#include <mesh.hpp>

Mesh::Mesh()
{
    _vertexArrayObject = 0;
    _indexBufferObject = 0;

    _vertexCount = 0;
    _indexCount = 0;

    _renderMode = RenderMode::Triangles;
}

void Mesh::setRenderMode(RenderMode renderMode)
{
    _renderMode = renderMode;
}

void Mesh::setIndices(const std::vector<GLuint> &indices)
{
    if(indices.empty())
    {
        throw std::invalid_argument("Indices cannot be empty.");
    }

    if(_vertexArrayObject == 0)
    {
        glGenVertexArrays(1, &_vertexArrayObject);
    }

    if(_indexBufferObject == 0)
    {
        glGenBuffers(1, &_indexBufferObject);
    }

    _indexCount = indices.size();

    glBindVertexArray(_vertexArrayObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

size_t Mesh::getVertexCount() const
{
    return _vertexCount;
}

size_t Mesh::getIndexCount() const
{
    return _indexCount;
}

size_t Mesh::getVertexArrayObject() const
{
    return _vertexArrayObject;
}

void Mesh::render() const
{
    if (_vertexArrayObject == 0) return;

    glBindVertexArray(_vertexArrayObject);

    if(_indexBufferObject != 0)
    {
        glDrawElements(static_cast<GLenum>(_renderMode), _indexCount, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(static_cast<GLenum>(_renderMode), 0, _vertexCount);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    for (const auto& pair : _attributeBuffers) {
        glDeleteBuffers(1, &pair.second);
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