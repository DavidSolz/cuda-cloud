#include <iostream>
#include <chrono>

#include <application.hpp>
#include <imguilayer.hpp>
#include <shader.hpp>
#include <mesh.hpp>

#include <glm/gtc/matrix_transform.hpp>

void renderFooter(int fps, float frameTime)
{
    ImGuiViewport* viewport =
        ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(
            viewport->Pos.x,
            viewport->Pos.y + viewport->Size.y - 30.0f));

    ImGui::SetNextWindowSize(
        ImVec2(
            viewport->Size.x,
            30.0f));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 6));
    ImGui::Begin("Footer", nullptr, flags);
    ImGui::Text("FPS: %d\t|\tFrame Time: %.3f ms", fps, frameTime * 1000.0f);
    ImGui::End();
    ImGui::PopStyleVar();
}

int main()
{

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
        glm::vec2 uv;
    };

    std::vector<Vertex> vertices = {
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    Shader shader("assets/shaders/default_vertex.glsl", "assets/shaders/default_fragment.glsl");

    Mesh mesh;
    mesh.setVertices(vertices.data(), vertices.size(), sizeof(Vertex));
    mesh.setRenderMode(RenderMode::TriangleFan);

    mesh.setVertexProperties({{0, 3, VertexPropertyType::Float, false, sizeof(Vertex), offsetof(Vertex, position)},
                              {1, 2, VertexPropertyType::Float, false, sizeof(Vertex), offsetof(Vertex, uv)}});


    const size_t textureSize = 512;

    GLuint texture2d;
    glGenTextures(1, &texture2d);
    glBindTexture(GL_TEXTURE_2D, texture2d);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    auto lastTime = std::chrono::high_resolution_clock::now();
    float accumulatedTime = 0.0f;
    int frameCount = 0;

    int lastFps = 0;
    float lastDeltaTime = 0.0f;

    glEnable(GL_DEPTH_TEST);

    while (!app.shouldClose())
    {
        // Timing
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        deltaTime = glm::min(deltaTime, 0.1f);

        if(accumulatedTime >= 1.0f)
        {
            accumulatedTime = 0.0f;
            lastFps = frameCount;
            frameCount = 0;
        }
       
        accumulatedTime += deltaTime;
        lastDeltaTime = deltaTime;
        frameCount++;

        // Render
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)properties.width / properties.height, 0.1f, 100.0f);

        shader.use();
        
        shader.setMat4("modelMatrix", model);
        shader.setMat4("viewMatrix", view);
        shader.setMat4("projectionMatrix", projection);

        glBindTexture(GL_TEXTURE_2D, texture2d);

        mesh.render();

        glBindTexture(GL_TEXTURE_2D, 0);

        imgui.begin();
        renderFooter(lastFps, lastDeltaTime);
        imgui.end();

        app.swapBuffers();
        app.poolEvents();

        // Check for errors
        app.checkError();
    }

    return 0;
}