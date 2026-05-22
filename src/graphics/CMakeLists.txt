
add_library(graphics STATIC
    shader.cpp
    mesh.cpp
)

target_include_directories(graphics PUBLIC
    .
)

target_link_libraries(graphics PUBLIC
    glad::glad
    glm
)