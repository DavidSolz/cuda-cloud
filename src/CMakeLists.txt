

add_subdirectory(core)
add_subdirectory(cuda)
add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(scene)
add_subdirectory(utils)
 
add_executable(${PROJECT_NAME} main.cpp
    core/application.cpp
    ui/imguilayer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    core
    ui
    graphics
)

if(CMAKE_CUDA_COMPILER)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        cuda
    )
endif()