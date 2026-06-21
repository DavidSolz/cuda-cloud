#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include <algorithm>
#include <chrono>
#include <iostream>

#include <application.hpp>
#include <imguilayer.hpp>
#include <mesh.hpp>
#include <shader.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <graphbuilder.hpp>
#include <material.hpp>

#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void renderFooter(int fps, float frameTime) {
  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(
      ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 30.0f));

  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 30.0f));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 6));
  ImGui::Begin("Footer", nullptr, flags);
  ImGui::Text("FPS: %d\t|\tFrame Time: %.3f ms", fps, frameTime * 1000.0f);
  ImGui::End();
  ImGui::PopStyleVar();
}

int main() {

  ApplicationProperties properties;
  
  properties.title = "Tiny Renderer";
  properties.width = 800;
  properties.height = 600;
  properties.frameInterval = 0;
  properties.majorVersion = 4;
  properties.minorVersion = 1;

  Application app(properties);

  ImGuiLayer imgui(app.getNativeHandle());

  auto renderer = glGetString(GL_RENDERER);
  auto version = glGetString(GL_VERSION);

  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "OpenGL version supported: " << version << std::endl;

  struct {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
  } modelData;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, nullptr,
                   "assets/models/suzanne.obj");

  for (size_t i = 0; i < shapes.size(); ++i) {
    const auto &shape = shapes[i];

    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      int fv = shape.mesh.num_face_vertices[f];

      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

        modelData.positions.push_back(
            {attrib.vertices[3 * idx.vertex_index + 0],
             attrib.vertices[3 * idx.vertex_index + 1],
             attrib.vertices[3 * idx.vertex_index + 2]});

        if (idx.normal_index >= 0) {
          modelData.normals.push_back(
              {attrib.normals[3 * idx.normal_index + 0],
               attrib.normals[3 * idx.normal_index + 1],
               attrib.normals[3 * idx.normal_index + 2]});
        } else {
          modelData.normals.push_back({0.0f, 0.0f, 0.0f});
        }

        if (idx.texcoord_index >= 0) {
          modelData.uvs.push_back(
              {attrib.texcoords[2 * idx.texcoord_index + 0],
               attrib.texcoords[2 * idx.texcoord_index + 1]});
        } else {
          modelData.uvs.push_back({0.0f, 0.0f});
        }

        modelData.indices.push_back(
            static_cast<unsigned int>(modelData.indices.size()));
      }
      index_offset += fv;
    }
  }

  glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);

  std::for_each(modelData.positions.begin(), modelData.positions.end(),
                [&](const glm::vec3 &pos) {
                  minBounds = glm::min(minBounds, pos);
                  maxBounds = glm::max(maxBounds, pos);
                });

  std::transform(modelData.positions.begin(), modelData.positions.end(),
                 modelData.positions.begin(),
                 [minBounds, maxBounds](const glm::vec3 &pos) {
                   return (pos - minBounds) / (maxBounds - minBounds) * 2.0f -
                          1.0f;
                 });

  struct {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;
  } planeVertices;

  planeVertices.positions = {{-1.0f, -1.0f, 0.0f},
                             {1.0f, -1.0f, 0.0f},
                             {1.0f, 1.0f, 0.0f},
                             {-1.0f, 1.0f, 0.0f}};
  planeVertices.uvs = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
  planeVertices.indices = {0, 1, 2, 2, 3, 0};

  struct LightDataBuffer {
    glm::vec3 position[10];
    glm::vec3 color[10];
  };

  std::vector<glm::vec3> lightPositions = {
      {0.0f, 5.0f, 5.0f},  {5.0f, 5.0f, 0.0f},   {-5.0f, 5.0f, 0.0f},
      {0.0f, 5.0f, -5.0f}, {3.5f, 5.0f, 3.5f},   {-3.5f, 5.0f, 3.5f},
      {3.5f, 5.0f, -3.5f}, {-3.5f, 5.0f, -3.5f}, {7.0f, 5.0f, 7.0f},
      {-7.0f, 5.0f, -7.0f}};

  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  std::vector<Vertex> globalVertices;
  std::vector<uint32_t> globalIndices;

  uint32_t baseVertex_model = globalVertices.size();
  uint32_t firstIndex_model = globalIndices.size();

  for (size_t i = 0; i < modelData.positions.size(); i++) {
    globalVertices.push_back(
        {modelData.positions[i], modelData.normals[i], modelData.uvs[i]});
  }

  for (auto idx : modelData.indices) {
    globalIndices.push_back(idx + baseVertex_model);
  }

  uint32_t modelIndexCount = modelData.indices.size();

  uint32_t baseVertex_light = globalVertices.size();
  uint32_t firstIndex_light = globalIndices.size();

  for (size_t i = 0; i < lightPositions.size(); i++) {
    globalVertices.push_back(
        {lightPositions[i], glm::vec3(0, 0, 1), glm::vec2(0, 0)});
  }

  for (size_t i = 0; i < lightPositions.size(); i++) {
    globalIndices.push_back(i + baseVertex_light);
  }

  uint32_t lightIndexCount = lightPositions.size();

  GLuint vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, globalVertices.size() * sizeof(Vertex),
               globalVertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, globalIndices.size() * sizeof(uint32_t),
               globalIndices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));

  glBindVertexArray(0);

  Mesh planeMesh;
  planeMesh.setAttribute(VertexAttribute::Position, planeVertices.positions);
  planeMesh.setAttribute(VertexAttribute::TexCoord, planeVertices.uvs);
  planeMesh.setIndices(planeVertices.indices);
  planeMesh.setRenderMode(RenderMode::Triangles);

  struct DrawElementsIndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
  };

  std::vector<DrawElementsIndirectCommand> commands;

  commands.push_back(
      {static_cast<GLuint>(modelIndexCount), 1, firstIndex_model, 0, 0});

  commands.push_back(
      {static_cast<GLuint>(lightIndexCount), 1, firstIndex_light, 0, 0});

  GLuint indirectBuffer;
  glGenBuffers(1, &indirectBuffer);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

  glBufferData(GL_DRAW_INDIRECT_BUFFER,
               commands.size() * sizeof(DrawElementsIndirectCommand),
               commands.data(), GL_STATIC_DRAW);

  struct ObjectData {
    glm::mat4 model;
  };

  std::vector<ObjectData> objectData;

  objectData.push_back({glm::mat4(1.0f)}); // model
  objectData.push_back({glm::mat4(1.0f)}); // light

  GLuint instanceVBO;

  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

  glBufferData(GL_ARRAY_BUFFER, objectData.size() * sizeof(ObjectData),
               objectData.data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

  for (int i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(ObjectData),
                          (void *)(sizeof(float) * 4 * i));
    glVertexAttribDivisor(3 + i, 1);
  }

  int currentWidth = app.getWidth();
  int currentHeight = app.getHeight();

  GraphContext graphContext;

  Texture2DDescriptor depthDesc;
  depthDesc.width = currentWidth;
  depthDesc.height = currentHeight;
  depthDesc.internalFormat = GL_DEPTH_COMPONENT32F;
  depthDesc.format = GL_DEPTH_COMPONENT;
  depthDesc.type = GL_FLOAT;

  Texture2DDescriptor rgba16FDesc;
  rgba16FDesc.width = currentWidth;
  rgba16FDesc.height = currentHeight;
  rgba16FDesc.internalFormat = GL_RGBA16F;
  rgba16FDesc.format = GL_RGBA;
  rgba16FDesc.type = GL_FLOAT;

  Texture2DDescriptor rgbaDesc;
  rgbaDesc.width = currentWidth;
  rgbaDesc.height = currentHeight;
  rgbaDesc.internalFormat = GL_RGBA;
  rgbaDesc.format = GL_RGBA;
  rgbaDesc.type = GL_UNSIGNED_BYTE;

  Texture2DDescriptor rg16FDesc;
  rg16FDesc.width = currentWidth;
  rg16FDesc.height = currentHeight;
  rg16FDesc.internalFormat = GL_RG16F;
  rg16FDesc.format = GL_RG;
  rg16FDesc.type = GL_FLOAT;

  Texture2D gDepthTexture = graphContext.textureRegister.create(depthDesc);
  Texture2D gPositionTexture = graphContext.textureRegister.create(rgba16FDesc);
  Texture2D gNormalTexture = graphContext.textureRegister.create(rgba16FDesc);
  Texture2D gAlbedoTexture = graphContext.textureRegister.create(rgba16FDesc);
  Texture2D gTextureCoordTexture =
      graphContext.textureRegister.create(rg16FDesc);

  // depth, position, normal, albedo, texture coord
  GLuint gBuffer;

  // Framebuffer
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         gDepthTexture.getNativeHandle(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPositionTexture.getNativeHandle(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormalTexture.getNativeHandle(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoTexture.getNativeHandle(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                         gTextureCoordTexture.getNativeHandle(), 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer not complete!" << std::endl;
    return -1;
  }

  const GLenum attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                 GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
  glDrawBuffers(4, attachments);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto updateFramebufferSizes = [&](int w, int h) {

    graphContext.textureRegister.dispose(gDepthTexture);
    graphContext.textureRegister.dispose(gPositionTexture);
    graphContext.textureRegister.dispose(gNormalTexture);
    graphContext.textureRegister.dispose(gAlbedoTexture);
    graphContext.textureRegister.dispose(gTextureCoordTexture);

    depthDesc.width = w;
    depthDesc.height = h;
    
    rgba16FDesc.width = w;
    rgba16FDesc.height = h;
    
    rg16FDesc.width = w;
    rg16FDesc.height = h;
    
    gDepthTexture = graphContext.textureRegister.create(depthDesc);
    gPositionTexture = graphContext.textureRegister.create(rgba16FDesc);
    gNormalTexture = graphContext.textureRegister.create(rgba16FDesc);
    gAlbedoTexture = graphContext.textureRegister.create(rgba16FDesc);
    gTextureCoordTexture = graphContext.textureRegister.create(rg16FDesc);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           gDepthTexture.getNativeHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           gPositionTexture.getNativeHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           gNormalTexture.getNativeHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           gAlbedoTexture.getNativeHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                           gTextureCoordTexture.getNativeHandle(), 0);

    glDrawBuffers(4, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
  };

  glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraTarget(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraForward = glm::normalize(cameraTarget - cameraPos);

  Material material;
  material.albedo = glm::vec3(0.5f, 0.5f, 0.3f);
  material.metallic = 0.5f;
  material.roughness = 0.5f;
  material.clearcoat = 0.5f;

  auto lastTime = std::chrono::high_resolution_clock::now();
  float accumulatedTime = 0.0f;
  int frameCount = 0;

  int lastFps = 0;
  float lastDeltaTime = 0.0f;

  Shader gPassShader("assets/shaders/gpass_vertex.glsl",
                     "assets/shaders/gpass_fragment.glsl");
  Shader screenPassShader("assets/shaders/screen_vertex.glsl",
                          "assets/shaders/screen_fragment.glsl");

  struct alignas(16) FramebufferDataBuffer {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec4 eyePosition;
    glm::vec2 resolution;
    glm::vec2 _pad1;
    float time;
    glm::vec3 _pad2;
  };

  FramebufferDataBuffer framebufferDataBuffer;
  LightDataBuffer lightDataBuffer;

  GLuint framebufferDataBufferIndex = 0;
  GLuint lightDataBufferIndex = 0;

  framebufferDataBufferIndex =
      glGetUniformBlockIndex(gPassShader.getId(), "FramebufferDataBuffer");
  glUniformBlockBinding(gPassShader.getId(), framebufferDataBufferIndex, 0);

  lightDataBufferIndex =
      glGetUniformBlockIndex(screenPassShader.getId(), "LightDataBuffer");
  glUniformBlockBinding(screenPassShader.getId(), lightDataBufferIndex, 1);

  GLuint uniformBuffer;
  glGenBuffers(1, &uniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(FramebufferDataBuffer), NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  GLuint lightUniformBuffer;
  glGenBuffers(1, &lightUniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightDataBuffer), NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBuffer, 0,
                    sizeof(FramebufferDataBuffer));
  glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightUniformBuffer, 0,
                    sizeof(LightDataBuffer));

  for (int i = 0; i < 10; i++) {
    float angle = glfwGetTime() + i * 0.5f;

    lightDataBuffer.position[i] = lightPositions[i];
    lightDataBuffer.color[i] =
        glm::vec3((cos(angle) + 1.0f) * 0.5f, (sin(angle) + 1.0f) * 0.5f, 0.5f);
  }

  while (!app.shouldClose()) {
    // Timing
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime =
        std::chrono::duration<float>(currentTime - lastTime).count();

    lastTime = currentTime;

    framebufferDataBuffer.projectionMatrix = glm::perspective(
        glm::radians(45.0f), (float)currentWidth / (float)currentHeight, 0.1f,
        1000.0f);
    framebufferDataBuffer.viewMatrix =
        glm::lookAt(cameraPos, cameraPos + cameraForward, cameraUp);
    framebufferDataBuffer.eyePosition = glm::vec4(cameraPos, 1.0f);
    framebufferDataBuffer.resolution = glm::vec2(currentWidth, currentHeight);
    framebufferDataBuffer.time = glfwGetTime();

    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
    GLvoid *ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &framebufferDataBuffer, sizeof(FramebufferDataBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
    ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &lightDataBuffer, sizeof(LightDataBuffer));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    deltaTime = glm::min(deltaTime, 0.1f);

    if (accumulatedTime >= 1.0f) {
      accumulatedTime = 0.0f;
      lastFps = frameCount;
      frameCount = 0;
    }

    accumulatedTime += deltaTime;
    lastDeltaTime = deltaTime;
    frameCount++;

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(app.getNativeHandle(), true);
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_SPACE) == GLFW_PRESS) {
      cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime * 5.0f;
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * deltaTime * 5.0f;
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_W) == GLFW_PRESS) {
      cameraPos += glm::vec3(0.0f, 0.0f, -1.0f) * deltaTime * 5.0f;
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_S) == GLFW_PRESS) {
      cameraPos += glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime * 5.0f;
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_A) == GLFW_PRESS) {
      cameraPos += glm::vec3(-1.0f, 0.0f, 0.0f) * deltaTime * 5.0f;
    }

    if (glfwGetKey(app.getNativeHandle(), GLFW_KEY_D) == GLFW_PRESS) {
      cameraPos += glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * 5.0f;
    }

    if (app.getWidth() != currentWidth || app.getHeight() != currentHeight) {
      currentWidth = app.getWidth();
      currentHeight = app.getHeight();
      updateFramebufferSizes(currentWidth, currentHeight);
    }

    // First pass: render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, currentWidth, currentHeight);
    glEnable(GL_DEPTH_TEST);

    // Clear the framebuffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gPassShader.use();

    gPassShader.setFloat("mMetallic", material.metallic);
    gPassShader.setFloat("mRoughness", material.roughness);
    gPassShader.setVec3("mAlbedo", material.albedo);
    gPassShader.setFloat("mClearcoat", material.clearcoat);

    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0,
                                commands.size(), 0);

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWidth, currentHeight);

    // Second pass: render to screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenPassShader.use();

    gPositionTexture.bind(TextureSlot::SLOT_0);
    screenPassShader.setInt("gPosition", 0);

    gNormalTexture.bind(TextureSlot::SLOT_1);
    screenPassShader.setInt("gNormal", 1);

    gAlbedoTexture.bind(TextureSlot::SLOT_2);
    screenPassShader.setInt("gDiffuse", 2);

    gTextureCoordTexture.bind(TextureSlot::SLOT_3);
    screenPassShader.setInt("gTextureCoord", 3);

    screenPassShader.setVec3("cameraPos", cameraPos);
    planeMesh.render();

    imgui.begin();

    ImGui::Begin("Parameters");
    ImGui::DragFloat3("Camera Position", &cameraPos.x, 0.1f);
    ImGui::ColorEdit3("Albedo", &material.albedo.x);
    ImGui::DragFloat("Metallic", &material.metallic, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Roughness", &material.roughness, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Clearcoat", &material.clearcoat, 0.01f, 0.0f, 1.0f);
    ImGui::End();

    ImGui::Begin("Debug View");

    ImGui::Text("Position (HDR)");
    ImGui::Image((void *)(intptr_t)gPositionTexture.getNativeHandle(),
                 ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::Text("Normal (needs remap)");
    ImGui::Image((void *)(intptr_t)gNormalTexture.getNativeHandle(),
                 ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::Text("Albedo");
    ImGui::Image((void *)(intptr_t)gAlbedoTexture.getNativeHandle(),
                 ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::Text("Texture Coord");
    ImGui::Image((void *)(intptr_t)gTextureCoordTexture.getNativeHandle(),
                 ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();

    renderFooter(lastFps, lastDeltaTime);
    imgui.end();

    app.swapBuffers();
    app.poolEvents();
    app.checkError();
  }

  return 0;
}