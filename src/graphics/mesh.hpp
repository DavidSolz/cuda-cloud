#pragma once

#include <glad/glad.h>
#include <unordered_map>
#include <vector>

#include <stdexcept>

enum VertexPropertyType {
  Float = GL_FLOAT,
  Double = GL_DOUBLE,
  Int = GL_INT,
  UnsignedInt = GL_UNSIGNED_INT,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Byte = GL_BYTE,
  UnsignedByte = GL_UNSIGNED_BYTE
};

struct VertexProperty {
  size_t index;
  size_t size;
  VertexPropertyType type;
  bool normalized;
  size_t stride;
  size_t offset;
};

enum class RenderMode {
  Triangles = GL_TRIANGLES,
  TriangleStrip = GL_TRIANGLE_STRIP,
  TriangleFan = GL_TRIANGLE_FAN,
  Lines = GL_LINES,
  LineStrip = GL_LINE_STRIP,
  LineLoop = GL_LINE_LOOP,
  Points = GL_POINTS
};

enum class VertexAttribute {
  Position = 0,
  Normal = 1,
  TexCoord = 2,
  Tangent = 3,
  Color = 4,
  BoneID = 5,
  BoneWeight = 6
};

class Mesh {
private:
  GLuint _vertexArrayObject;
  GLuint _indexBufferObject;

  std::unordered_map<VertexAttribute, GLuint> _attributeBuffers;

  size_t _vertexCount;
  size_t _indexCount;

  RenderMode _renderMode;

public:
  Mesh();

  void setRenderMode(RenderMode renderMode);

  template <typename T>
  void setAttribute(VertexAttribute attribute, const std::vector<T> &data,
                    bool normalized = false) {
    if (data.empty()) {
      throw std::invalid_argument("Attribute data cannot be empty.");
    }

    if (_vertexArrayObject == 0) {
      glGenVertexArrays(1, &_vertexArrayObject);
    }

    auto it = _attributeBuffers.find(attribute);

    if (it == _attributeBuffers.end()) {
      GLuint buffer;
      glGenBuffers(1, &buffer);
      _attributeBuffers[attribute] = buffer;
    }

    GLuint attributeBuffer = _attributeBuffers[attribute];

    glBindVertexArray(_vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, attributeBuffer);

    size_t attributeIndex = static_cast<size_t>(attribute);
    size_t attributeSize = sizeof(T) / sizeof(typename T::value_type);

    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(attributeIndex);

    if constexpr (std::is_integral_v<typename T::value_type>) {
      glVertexAttribIPointer(attributeIndex, attributeSize, GL_INT, 0,
                             (void *)0);
    } else {
      glVertexAttribPointer(attributeIndex, attributeSize, GL_FLOAT,
                            normalized ? GL_TRUE : GL_FALSE, 0, (void *)0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (attribute == VertexAttribute::Position) {
      _vertexCount = data.size();
    }
  }

  void setIndices(const std::vector<GLuint> &indices);

  size_t getVertexCount() const;
  size_t getIndexCount() const;

  size_t getVertexArrayObject() const;

  void render() const;

  ~Mesh();
};