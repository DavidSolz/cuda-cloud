#include <iostream>
#include <chrono>

#include <application.hpp>
#include <imguilayer.hpp>
#include <shader.hpp>
#include <mesh.hpp>

#include <glm/gtc/matrix_transform.hpp>

int main() {

    ApplicationProperties properties;
    properties.title = "CUDA Cloud Renderer";
    properties.width = 800;
    properties.height = 600;
    properties.frameInterval = 0;

    Application app(properties);

    ImGuiLayer imgui(app.getNativeHandle());

    auto renderer = glGetString(GL_RENDERER);
    auto version = glGetString(GL_VERSION);

    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    Shader shader("assets/shaders/default_vertex.glsl", "assets/shaders/default_fragment.glsl");

    Mesh mesh;
    mesh.setVertices(vertices.data(), vertices.size(), sizeof(Vertex));
    
    mesh.setVertexProperties({
        {0, 3, VertexPropertyType::Float, false, sizeof(Vertex), offsetof(Vertex, position)},
        {1, 3, VertexPropertyType::Float, false, sizeof(Vertex), offsetof(Vertex, color)}
    });

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    auto lastTime = std::chrono::high_resolution_clock::now();

    while(!app.shouldClose()) {

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        model = glm::rotate(model, deltaTime.count() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.setMat4("modelMatrix", model);
        shader.setMat4("viewMatrix", view);
        shader.setMat4("projectionMatrix", projection);

        mesh.render();

        app.swapBuffers();
        app.poolEvents();

        app.checkError();
    }

    return 0;
}