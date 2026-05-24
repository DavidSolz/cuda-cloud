#pragma once


#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <string>
#include <stdexcept>

struct ApplicationProperties {
    int width;
    int height;
    int frameInterval;
    std::string title;
};

class Application {
private:

    static bool _initialized;

    GLFWwindow* _window;

    size_t _width;
    size_t _height;

    static void onResize(GLFWwindow* window, int width, int height);

public:
    Application(const ApplicationProperties& properties);
    
    size_t getWidth() const;
    size_t getHeight() const;

    GLFWwindow * getNativeHandle() const;

    bool shouldClose() const;

    void checkError() const;

    void swapBuffers() const;

    void poolEvents() const;

    ~Application();
};