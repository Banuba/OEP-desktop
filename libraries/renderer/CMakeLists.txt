set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_library(renderer STATIC ${srcs})

target_include_directories(renderer PUBLIC
    ${include_dirs}
)

target_include_directories(renderer SYSTEM PUBLIC ${LIBYUV_INCLUDE_DIRS})

target_link_libraries(renderer
    bnb_effect_player
    glad
    glfw
    bnb_oep_opengl_program_target
)
