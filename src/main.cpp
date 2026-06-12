#include <iostream>
#include <chrono>

#include <application.hpp>
#include <imguilayer.hpp>
#include <shader.hpp>
#include <mesh.hpp>

#include <material.hpp>

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
    planeMesh.setRenderMode(RenderMode::Triangles);
    planeMesh.setVertexProperties({{0, 3, VertexPropertyType::Float, false, sizeof(PlaneVertex), offsetof(PlaneVertex, position)},
                                    {1, 2, VertexPropertyType::Float, false, sizeof(PlaneVertex), offsetof(PlaneVertex, uv)}});

    int currentWidth = app.getWidth();
    int currentHeight = app.getHeight();

    GLuint gPosition;
    GLuint gNormal;
    GLuint gAlbedo;
    GLuint gBuffer;
    GLuint gDepth;

    // Depth texture
    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, currentWidth, currentHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // GPASS position buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, currentWidth, currentHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // GPASS normal buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, currentWidth, currentHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // GPASS albedo buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Framebuffer
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
        return -1;
    }

    const GLenum attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto updateFramebufferSizes = [&](int w, int h) {
            glBindTexture(GL_TEXTURE_2D, gDepth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, gPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, gNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindTexture(GL_TEXTURE_2D, gAlbedo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

            glDrawBuffers(3, attachments);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, w, h);
        };

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 backgroundColor(0.0f);

    glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(cameraTarget - cameraPos);

    Material material;
    material.albedo = glm::vec3(0.8f, 0.3f, 0.3f);
    material.metallic = 0.5f;
    material.roughness = 0.5f;

    auto lastTime = std::chrono::high_resolution_clock::now();
    float accumulatedTime = 0.0f;
    int frameCount = 0;

    int lastFps = 0;
    float lastDeltaTime = 0.0f;
    
    Shader gPassShader("assets/shaders/gpass_vertex.glsl", "assets/shaders/gpass_fragment.glsl");
    Shader screenPassShader("assets/shaders/screen_vertex.glsl", "assets/shaders/screen_fragment.glsl");

    struct alignas(16) FramebufferDataBuffer
    {
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
        glm::vec4 eyePosition;
        glm::vec2 resolution;
        glm::vec2 _pad1;
        float time;
        glm::vec3 _pad2;
    };

    struct LightDataBuffer
    {
        glm::vec3 position[10];
        glm::vec3 color[10];
    };

    std::vector<glm::vec3> lightPositions = {
        {0.0f, 5.0f, 5.0f},
        {5.0f, 5.0f, 0.0f},
        {-5.0f, 5.0f, 0.0f},
        {0.0f, 5.0f, -5.0f},
        {3.5f, 5.0f, 3.5f},
        {-3.5f, 5.0f, 3.5f},
        {3.5f, 5.0f, -3.5f},
        {-3.5f, 5.0f, -3.5f},
        {7.0f, 5.0f, 7.0f},
        {-7.0f, 5.0f, -7.0f}
    };

    Mesh lightMesh;
    lightMesh.setVertices(lightPositions.data(), lightPositions.size(), sizeof(glm::vec3));
    lightMesh.setRenderMode(RenderMode::Points);
    lightMesh.setVertexProperties({{0, 3, VertexPropertyType::Float, false, sizeof(glm::vec3), 0}});

    FramebufferDataBuffer framebufferDataBuffer;
    LightDataBuffer lightDataBuffer;

    GLuint framebufferDataBufferIndex = 0;
    GLuint lightDataBufferIndex = 0;

    framebufferDataBufferIndex = glGetUniformBlockIndex(gPassShader.getId(), "FramebufferDataBuffer");
    glUniformBlockBinding(gPassShader.getId(), framebufferDataBufferIndex, 0);

    lightDataBufferIndex = glGetUniformBlockIndex(screenPassShader.getId(), "LightDataBuffer");
    glUniformBlockBinding(screenPassShader.getId(), lightDataBufferIndex, 1);

    GLuint uniformBuffer;
    glGenBuffers(1, &uniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(FramebufferDataBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint lightUniformBuffer;
    glGenBuffers(1, &lightUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightDataBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBuffer, 0, sizeof(FramebufferDataBuffer));
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightUniformBuffer, 0, sizeof(LightDataBuffer));

    for (int i = 0; i < 10; i++)
    {
        float angle = glfwGetTime() + i * 0.5f;

        lightDataBuffer.position[i] = lightPositions[i];
        lightDataBuffer.color[i] = glm::vec3((cos(angle) + 1.0f) * 0.5f, (sin(angle) + 1.0f) * 0.5f, 0.5f);
    }

    while (!app.shouldClose())
    {
        // Timing
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

        lastTime = currentTime;

        framebufferDataBuffer.projectionMatrix = glm::perspective(glm::radians(45.0f), (float)currentWidth / (float)currentHeight, 0.1f, 1000.0f);
        framebufferDataBuffer.viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraForward, cameraUp);
        framebufferDataBuffer.eyePosition = glm::vec4(cameraPos, 1.0f);
        framebufferDataBuffer.resolution = glm::vec2(currentWidth, currentHeight);
        framebufferDataBuffer.time = glfwGetTime();

        glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
        GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, &framebufferDataBuffer, sizeof(FramebufferDataBuffer));
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
        ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, &lightDataBuffer, sizeof(LightDataBuffer));
        glUnmapBuffer(GL_UNIFORM_BUFFER);

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

        // First pass: render to texture
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, currentWidth, currentHeight);
        glEnable(GL_DEPTH_TEST);

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gPassShader.use();
        
        gPassShader.setMat4("modelMatrix", model);
        cubeMesh.render();

        gPassShader.setMat4("modelMatrix", glm::mat4(1.0f));
        lightMesh.render();

        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, currentWidth, currentHeight);

        // Second pass: render to screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenPassShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        screenPassShader.setInt("gPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        screenPassShader.setInt("gNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        screenPassShader.setInt("gAlbedo", 2);

        screenPassShader.setVec3("mAlbedo", material.albedo);
        screenPassShader.setFloat("mMetallic", material.metallic);
        screenPassShader.setFloat("mRoughness", material.roughness);

        screenPassShader.setVec3("cameraPos", cameraPos);

        planeMesh.render();

        imgui.begin();

        ImGui::Begin("Parameters");
        ImGui::DragFloat3("Camera Position", &cameraPos.x, 0.1f);
        ImGui::ColorEdit3("Albedo", &material.albedo.x);
        ImGui::DragFloat("Metallic", &material.metallic, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Roughness", &material.roughness, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Background Color", &backgroundColor.x);
        ImGui::End();

        ImGui::Begin("Debug View");

        ImGui::Text("Position (HDR)");
        ImGui::Image((void*)(intptr_t)gPosition, ImVec2(256, 256), ImVec2(0,1), ImVec2(1,0));

        ImGui::Text("Normal (needs remap)");
        ImGui::Image((void*)(intptr_t)gNormal, ImVec2(256, 256), ImVec2(0,1), ImVec2(1,0));

        ImGui::Text("Albedo");
        ImGui::Image((void*)(intptr_t)gAlbedo, ImVec2(256, 256), ImVec2(0,1), ImVec2(1,0));

        ImGui::End();

        renderFooter(lastFps, lastDeltaTime);
        imgui.end();

        app.swapBuffers();
        app.poolEvents();
        app.checkError();
    }

    return 0;
}