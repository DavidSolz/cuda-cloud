
add_library(core STATIC
    application.cpp
)

target_include_directories(core PUBLIC
    .
)

target_link_libraries(core PUBLIC
    glad::glad
    glfw
)