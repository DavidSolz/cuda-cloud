#include <iostream>
#include <chrono>

#include <application.hpp>
#include <imguilayer.hpp>
#include <shader.hpp>
#include <mesh.hpp>

#include<worleynoise.hpp>

#include <glm/gtc/matrix_transform.hpp>

void renderFooter(int fps, float frameTime)
{
    ImGuiViewport *viewport =
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
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 6));
    ImGui::Begin("Footer", nullptr, flags);
    ImGui::Text("FPS: %d\t|\tFrame Time: %.3f ms", fps, frameTime * 1000.0f);
    ImGui::End();
    ImGui::PopStyleVar();
}

int main()
{

    ApplicationProperties properties;
    properties.title = "Volume Renderer";
    properties.width = 800;
    properties.height = 600;
    properties.frameInterval = 0;

    Application app(properties);

    ImGuiLayer imgui(app.getNativeHandle());

    auto renderer = glGetString(GL_RENDERER);
    auto version = glGetString(GL_VERSION);

    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    struct CubeVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    std::vector<CubeVertex> vertices = {
        {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 0
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 1
        {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // 2
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 3
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},   // 4
        {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},    // 5
        {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},    // 6
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}    // 7
    };

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0, // Bottom face
        4, 5, 6, 6, 7, 4, // Top face
        0, 1, 5, 5, 4, 0, // Front face
        1, 2, 6, 6, 5, 1, // Right face
        2, 3, 7, 7, 6, 2, // Back face
        3, 0, 4, 4, 7, 3  // Left face
    };

    Mesh cubeMesh;
    cubeMesh.setVertices(vertices.data(), vertices.size(), sizeof(CubeVertex));
    cubeMesh.setIndices(indices);
    cubeMesh.setRenderMode(RenderMode::Triangles);

    cubeMesh.setVertexProperties({{0, 3, VertexPropertyType::Float, false, sizeof(CubeVertex), offsetof(CubeVertex, position)},
                              {1, 3, VertexPropertyType::Float, false, sizeof(CubeVertex), offsetof(CubeVertex, normal)},
                              {2, 2, VertexPropertyType::Float, false, sizeof(CubeVertex), offsetof(CubeVertex, uv)}});

    vertices.clear();
    indices.clear();

    struct PlaneVertex
    {
        glm::vec3 position;
        glm::vec2 uv;
    };

    std::vector<PlaneVertex> pvertices = {
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    indices = {0, 1, 2, 2, 3, 0};

    Mesh planeMesh;
    planeMesh.setVertices(pvertices.data(), pvertices.size(), sizeof(PlaneVertex));
    planeMesh.setIndices(indices);
    planeMesh.setRenderMode(RenderMode::TriangleFan);
    planeMesh.setVertexProperties({{0, 3, VertexPropertyType::Float, false, sizeof(PlaneVertex), offsetof(PlaneVertex, position)},
                                    {1, 2, VertexPropertyType::Float, false, sizeof(PlaneVertex), offsetof(PlaneVertex, uv)}});

    constexpr size_t latticeSize = 10;
    constexpr size_t textureSize = 32;

    std::vector<float> textureData = WorleyNoise::generate(glm::vec3(textureSize), glm::vec3(latticeSize));

    size_t currentWidth = app.getWidth();
    size_t currentHeight = app.getHeight();

    // Depth texture
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, currentWidth, currentHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Color texture
    GLuint colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto updateFramebufferSizes = [&](int w, int h) {
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glBindTexture(GL_TEXTURE_2D, colorTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        };

    // Density texture
    GLuint densityTexture;
    glGenTextures(1, &densityTexture);
    glBindTexture(GL_TEXTURE_3D, densityTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, textureSize, textureSize, textureSize, 0, GL_RED, GL_FLOAT, textureData.data());
    glBindTexture(GL_TEXTURE_3D, 0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 lightPos(0.0f, 5.0f, 5.0f);
    glm::vec3 backgroundColor(0.0f);

    glm::vec3 boundingBoxMin(-2.0f);
    glm::vec3 boundingBoxMax(2.0f);

    glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    float darknessThreshold = 0.2f;
    float densityBias = 0.2f;
    float absorption = 1.8f;
    int numStepsLight = 8;
    int numStepsView = 32;

    auto lastTime = std::chrono::high_resolution_clock::now();
    float accumulatedTime = 0.0f;
    int frameCount = 0;

    int lastFps = 0;
    float lastDeltaTime = 0.0f;

    Shader shader("../assets/shaders/vanilla_vertex.glsl", "../assets/shaders/vanilla_fragment.glsl");
    Shader densityShader("../assets/shaders/density_vertex.glsl", "../assets/shaders/density_fragment.glsl");

    while (!app.shouldClose())
    {
        // Timing
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        deltaTime = glm::min(deltaTime, 0.1f);

        if (accumulatedTime >= 1.0f)
        {
            accumulatedTime = 0.0f;
            lastFps = frameCount;
            frameCount = 0;
        }

        accumulatedTime += deltaTime;
        lastDeltaTime = deltaTime;
        frameCount++;

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(app.getNativeHandle(), true);
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * 5.0f;
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * deltaTime * 5.0f;
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(0.0f, 0.0f, -1.0f) * deltaTime * 5.0f;
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime * 5.0f;
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(-1.0f, 0.0f, 0.0f) * deltaTime * 5.0f;
        }

        if(glfwGetKey(app.getNativeHandle(), GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * 5.0f;
        }

        if (app.getWidth() != currentWidth || app.getHeight() != currentHeight) {
            currentWidth = app.getWidth();
            currentHeight = app.getHeight();
            updateFramebufferSizes(currentWidth, currentHeight);
        }

        model = glm::rotate(model, deltaTime * glm::radians(20.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
        view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        projection = glm::perspective(glm::radians(45.0f), (float)currentWidth / (float)currentHeight, 0.1f, 1000.0f);

        // First pass: render to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, currentWidth, currentHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("modelMatrix", model);
        shader.setMat4("viewMatrix", view);
        shader.setMat4("projectionMatrix", projection);
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("cameraPos", cameraPos);

        cubeMesh.render();

        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // // Second pass: render to screen
        // glViewport(0, 0, currentWidth, currentHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        densityShader.use();

        densityShader.setMat4("invViewMatrix", glm::inverse(view));
        densityShader.setMat4("invProjectionMatrix", glm::inverse(projection));
        densityShader.setVec3("boundingBoxMin", boundingBoxMin);
        densityShader.setVec3("boundingBoxMax", boundingBoxMax);
        densityShader.setVec2("resolution", glm::vec2(currentWidth, currentHeight));
        densityShader.setVec3("lightPos", lightPos);
        densityShader.setVec3("cameraPos", cameraPos);
        densityShader.setInt("numStepsLight", numStepsLight);
        densityShader.setInt("numStepsView", numStepsView);
        densityShader.setFloat("darknessThreshold", darknessThreshold);
        densityShader.setFloat("densityBias", densityBias);
        densityShader.setFloat("absorption", absorption);   

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, densityTexture);
        densityShader.setInt("densityTexture", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        densityShader.setInt("depthTexture", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        densityShader.setInt("colorTexture", 2);

        planeMesh.render();
        glBindTexture(GL_TEXTURE_3D, 0);

        imgui.begin();

        ImGui::Begin("Parameters");
        ImGui::DragFloat3("Light Position", &lightPos.x, 0.1f);
        ImGui::DragFloat3("Camera Position", &cameraPos.x, 0.1f);
        ImGui::DragFloat3("Bounding Box Min", &boundingBoxMin.x, 0.1f);
        ImGui::DragFloat3("Bounding Box Max", &boundingBoxMax.x, 0.1f);
        ImGui::DragFloat("Darkness Threshold", &darknessThreshold, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Density Bias", &densityBias, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Absorption", &absorption, 0.1f, 0.0f, 10.0f);
        ImGui::DragInt("Steps Light", &numStepsLight, 1, 1, 256);
        ImGui::DragInt("Steps View", &numStepsView, 1, 1, 256);
        ImGui::ColorEdit3("Background Color", &backgroundColor.x);
        ImGui::End();

        renderFooter(lastFps, lastDeltaTime);
        imgui.end();

        app.swapBuffers();
        app.poolEvents();
        app.checkError();
    }

    return 0;
}