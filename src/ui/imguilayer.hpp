#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>

struct GLFWwindow;

class ImGuiLayer
{
private:

    GLFWwindow* _window;

public:
    ImGuiLayer(GLFWwindow* window, const std::string& glslVersion = "#version 410");

    void begin() const;

    void end() const;

    ~ImGuiLayer();
};