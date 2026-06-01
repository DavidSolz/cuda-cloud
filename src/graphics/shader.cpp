#include <shader.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);

    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {

        throw std::runtime_error("Failed to open shader files" + vertexPath + " and/or " + fragmentPath);
    }

    vertexCode = std::string((std::istreambuf_iterator<char>(vShaderFile)), std::istreambuf_iterator<char>());
    fragmentCode = std::string((std::istreambuf_iterator<char>(fShaderFile)), std::istreambuf_iterator<char>());

    vShaderFile.close();
    fShaderFile.close();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

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
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

GLuint Shader::getId() const {
    return _id;
}

Shader::~Shader() {
    glDeleteProgram(_id);
}

GLint Shader::getUniformLocation(const std::string& name) {
    if (_uniformLocations.find(name) != _uniformLocations.end()) {
        return _uniformLocations[name];
    }
    GLint location = glGetUniformLocation(_id, name.c_str());
    if (location == -1) {
        std::cout << "Warning: uniform '" << name << "' not found in shader!" << std::endl;
    }
    _uniformLocations[name] = location;
    return location;
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type) const {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader compilation error of type: " << type << "\n" << infoLog << std::endl;
            throw std::runtime_error("Shader compilation error of type: " + type + "\n");
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Program linking error of type: " << type << "\n" << infoLog << std::endl;
            throw std::runtime_error("Program linking error of type: " + type + "\n");
        }
    }
}