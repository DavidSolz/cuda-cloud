
add_library(cuda STATIC
    worleyalgorithm.cu
    cudaglbuffer.cu
)

target_include_directories(cuda PUBLIC
    .
)

target_link_libraries(cuda PUBLIC
    glad::glad
)