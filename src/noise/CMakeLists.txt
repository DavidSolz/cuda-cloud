add_library(noise STATIC
    worleynoise.cpp
)

target_include_directories(noise PUBLIC
    .
)

target_link_libraries(noise PUBLIC
    glm
)

find_package(OpenMP OPTIONAL)

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found, enabling parallelism in noise generation")
    target_compile_options(noise PUBLIC ${OpenMP_CXX_FLAGS})

target_link_libraries(noise PUBLIC
    OpenMP::OpenMP_CXX
)
endif()