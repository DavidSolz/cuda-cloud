
add_library(ui STATIC
    imguilayer.cpp
)

target_include_directories(ui PUBLIC
    .
)

target_link_libraries(ui PUBLIC
    imgui::imgui
    glfw
)