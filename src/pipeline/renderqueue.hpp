#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <mesh.hpp>
#include <material.hpp>
#include <shader.hpp>


struct RenderCommand {
  Mesh *mesh;
  Material *material;
  Shader *shader;
  glm::mat4 modelMatrix;
};

enum class CommandType{
    Opaque,
    Transparent
};

class RenderQueue {
private:
    std::vector<RenderCommand> _opaqueCommands;
    std::vector<RenderCommand> _transparentCommands;

public:

    template<CommandType TType>
    void addCommand(const RenderCommand& command)
    {
        if constexpr (TType == CommandType::Opaque) {
            _opaqueCommands.push_back(command);
        } else {
            _transparentCommands.push_back(command);
        }
    }

    std::vector<RenderCommand>& getOpaqueCommands() { 
        return _opaqueCommands; 
    }
    
    std::vector<RenderCommand>& getTransparentCommands() 
    { 
        return _transparentCommands; 
    }

    void clear() {
        _opaqueCommands.clear();
        _transparentCommands.clear();
    }
};