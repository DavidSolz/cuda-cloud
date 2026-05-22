cmake_minimum_required(VERSION 3.10)
project(glad
    VERSION 0.1
    DESCRIPTION "GLAD OpenGL loader"
    LANGUAGES C
)

set(glad_SOURCES
    "src/glad.c"
)

add_library(glad SHARED ${glad_SOURCES})

target_include_directories(glad PUBLIC
    "include"
)

add_library(glad::glad ALIAS glad)

message(STATUS "GLAD: Library configured successfully")