#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>

#include <fstream>
#include <string>

#include <iostream>

struct Uniform {
  GLint location;
  GLenum type;
};

template<typename T>
concept UniformType =
    std::same_as<T,float> ||
    std::same_as<T,int> ||
    std::same_as<T,glm::vec2> ||
    std::same_as<T,glm::vec3> ||
    std::same_as<T,glm::vec4> ||
    std::same_as<T,glm::mat4>;

class Shader {
private:
  GLuint _id;

  std::unordered_map<std::string, Uniform> _uniforms;

  GLint getUniformLocation(const std::string &name);

  void checkCompileErrors(GLuint shader, const std::string &type) const;

  void reflectUniforms();

public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);

  void use() const;

  template <typename T>
  void setUniform(const std::string &name, const T &value) {
    auto it = _uniforms.find(name);

    if (it == _uniforms.end())
      return;

    const auto &uniform = it->second;

    if constexpr (std::is_same_v<T, float>) {
      glUniform1f(uniform.location, value);
    } else if constexpr (std::is_same_v<T, int>) {
      glUniform1i(uniform.location, value);
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
      glUniform2fv(uniform.location, 1, &value[0]);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
      glUniform3fv(uniform.location, 1, &value[0]);
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
      glUniform4fv(uniform.location, 1, &value[0]);
    } else if constexpr (std::is_same_v<T, glm::mat2>) {
      glUniformMatrix2fv(uniform.location, 1, GL_FALSE, &value[0][0]);
    } else if constexpr (std::is_same_v<T, glm::mat3>) {
      glUniformMatrix3fv(uniform.location, 1, GL_FALSE, &value[0][0]);
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
      glUniformMatrix4fv(uniform.location, 1, GL_FALSE, &value[0][0]);
    }
  }

  GLuint getId() const;

  ~Shader();
};