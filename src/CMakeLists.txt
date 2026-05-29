

add_subdirectory(core)

if(USE_CUDA)
    add_subdirectory(cuda)
endif()

add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(scene)
add_subdirectory(utils)
add_subdirectory(noise)
 
add_executable(${PROJECT_NAME} main.cpp
    core/application.cpp
    ui/imguilayer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    core
    ui
    graphics
    noise
)

if(USE_CUDA)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        cuda
    )
else()
    message(WARNING "CUDA support not found. CUDA features will be disabled.")
endif()

message(STATUS "Copying assets to build directory")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)