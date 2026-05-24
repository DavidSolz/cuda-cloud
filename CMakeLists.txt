cmake_minimum_required(VERSION 3.22)


include(CheckLanguage)

check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    
    project(cudaCloud
    VERSION 0.1
    DESCRIPTION "A simple cloud rendering engine using CUDA and OpenGL"
    LANGUAGES CXX CUDA
    )

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    message(STATUS "CUDA support enabled.")

else()
    project(cudaCloud
    VERSION 0.1
    DESCRIPTION "A simple cloud rendering engine using CUDA and OpenGL"
    LANGUAGES CXX
    )

    message(WARNING "CUDA support not found. CUDA features will be disabled.")

endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party)

include(cmake/imgui.cmake)
add_subdirectory(src)