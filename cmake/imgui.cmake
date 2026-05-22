set(imgui_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/misc/cpp/imgui_stdlib.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui STATIC ${imgui_SOURCES})

target_include_directories(imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends"
)

find_package(OpenGL REQUIRED)

target_link_libraries(imgui PRIVATE
    glfw
    OpenGL::GL
)

add_library(imgui::imgui ALIAS imgui)

message(STATUS "IMGUI: Library configured successfully")
