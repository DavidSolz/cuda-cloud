#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

struct ApplicationProperties {
    int width;
    int height;
    int frameInterval;
    int majorVersion;
    int minorVersion;
    std::string title;
};

using NativeWindowHandle = GLFWwindow*;

class Application {
private:

    static bool _initialized;

    NativeWindowHandle _window;

    size_t _width;
    size_t _height;

    static void onResize(NativeWindowHandle window, int width, int height);

public:
    Application();
    
    NativeWindowHandle open(const ApplicationProperties& properties);

    void close() const;

    NativeWindowHandle getNativeHandle() const;

    size_t getWidth() const;
    
    size_t getHeight() const;

    bool isClosed() const;

    void checkError() const;

    void swapBuffers() const;

    void poolEvents() const;

    ~Application();
};