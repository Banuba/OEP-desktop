file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(renderer STATIC ${srcs})

target_link_libraries(renderer
    glfw_utils
    bnb_oep_opengl_program_target
)
