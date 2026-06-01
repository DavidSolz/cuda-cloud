#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>

#include <string>
#include <fstream>

#include <iostream>

class Shader {
private:

    GLuint _id;

    std::map<std::string, GLint> _uniformLocations;

    GLint getUniformLocation(const std::string& name);

    void checkCompileErrors(GLuint shader, const std::string& type) const;

public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec2(const std::string& name, const glm::vec2& value);
    
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);

    GLuint getId() const;

    ~Shader();
};