#include <application.hpp>

bool Application::_initialized = false;

void Application::onResize(GLFWwindow* window, int width, int height) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->_width = width;
        app->_height = height;
        glViewport(0, 0, width, height);
    }
}

Application::Application(const ApplicationProperties& properties) {

    if (!_initialized) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        _initialized = true;
    }

    _width = properties.width;
    _height = properties.height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, properties.majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, properties.minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);

    _window = glfwCreateWindow(_width, _height, properties.title.c_str(), nullptr, nullptr);

    if (!_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(_window);
    glfwSwapInterval(properties.frameInterval);

    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, onResize);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, _width, _height);

}

size_t Application::getWidth() const {
    return _width;
}

size_t Application::getHeight() const {
    return _height;
}

GLFWwindow* Application::getNativeHandle() const {
    return _window;
}

bool Application::shouldClose() const {
    return glfwWindowShouldClose(_window);
}

void Application::checkError() const {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        throw std::runtime_error("OpenGL error: " + std::to_string(error));
    }
}

void Application::swapBuffers() const {
    glfwSwapBuffers(_window);
}

void Application::poolEvents() const {
    glfwPollEvents();
}

Application::~Application() {

    if(!_initialized)
        return;

    if (_window) {
        glfwDestroyWindow(_window);
    }

    glfwTerminate();
    _initialized = false;
}