cmake_minimum_required(VERSION 3.22)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "" FORCE)
endif()

include(CheckLanguage)

check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(USE_CUDA ON)

    project(cudaCloud
    VERSION 0.1
    DESCRIPTION "A simple cloud rendering engine using CUDA and OpenGL"
    LANGUAGES CXX CUDA
    )

    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    message(STATUS "CUDA support enabled.")

else()
    set(USE_CUDA OFF)

    project(cudaCloud
    VERSION 0.1
    DESCRIPTION "A simple cloud rendering engine using CUDA and OpenGL"
    LANGUAGES CXX
    )

    message(WARNING "CUDA support not found. CUDA features will be disabled.")

endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_NONE)

add_subdirectory(third_party)

include(cmake/imgui.cmake)
add_subdirectory(src)