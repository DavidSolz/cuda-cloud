
add_library(core STATIC
    application.cpp
)

find_package(OpenMP OPTIONAL)

target_include_directories(core PUBLIC
    .
)

target_link_libraries(core PUBLIC
    glad::glad
    glfw
    OpenMP::OpenMP_CXX
)