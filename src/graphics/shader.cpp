#include <shader.hpp>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;

  std::ifstream vShaderFile(vertexPath);
  std::ifstream fShaderFile(fragmentPath);

  if (!vShaderFile.is_open() || !fShaderFile.is_open()) {

    throw std::runtime_error("Failed to open shader files" + vertexPath +
                             " and/or " + fragmentPath);
  }

  vertexCode = std::string((std::istreambuf_iterator<char>(vShaderFile)),
                           std::istreambuf_iterator<char>());
  fragmentCode = std::string((std::istreambuf_iterator<char>(fShaderFile)),
                             std::istreambuf_iterator<char>());

  vShaderFile.close();
  fShaderFile.close();

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  GLuint vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  _id = glCreateProgram();
  glAttachShader(_id, vertex);
  glAttachShader(_id, fragment);
  glLinkProgram(_id);
  checkCompileErrors(_id, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  reflectUniforms();
}

void Shader::reflectUniforms() {

  GLint count = 0;

  glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &count);

  GLint maxLength = 0;

  glGetProgramiv(_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

  std::vector<char> name(maxLength);

  for (GLuint i = 0; i < count; i++) {
    GLsizei length;
    GLint size;
    GLenum type;

    glGetActiveUniform(_id, i, maxLength, &length, &size, &type, name.data());

    std::string uniformName(name.data(), length);

    GLint location = glGetUniformLocation(_id, uniformName.c_str());

    _uniforms.emplace(uniformName, Uniform{location, type});
  }
}

void Shader::use() const { glUseProgram(_id); }

GLuint Shader::getId() const { return _id; }

Shader::~Shader() { glDeleteProgram(_id); }

GLint Shader::getUniformLocation(const std::string &name) {
  auto it = _uniforms.find(std::string(name));

  if (it == _uniforms.end()) {
    return -1;
  }

  return it->second.location;
}

void Shader::checkCompileErrors(GLuint shader, const std::string &type) const {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "Shader compilation error of type: " << type << "\n"
                << infoLog << std::endl;
      throw std::runtime_error("Shader compilation error of type: " + type +
                               "\n");
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "Program linking error of type: " << type << "\n"
                << infoLog << std::endl;
      throw std::runtime_error("Program linking error of type: " + type + "\n");
    }
  }
}