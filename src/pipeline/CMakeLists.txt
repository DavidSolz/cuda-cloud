add_library(pipeline STATIC
    graphbuilder.cpp
    graphnodedescriptor.cpp
)

target_include_directories(pipeline PUBLIC
    .
)   

target_link_libraries(pipeline PUBLIC
    glm
    graphics
)