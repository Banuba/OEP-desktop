set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(glfw_utils STATIC ${srcs})

target_include_directories(glfw_utils PUBLIC
    ${include_dirs}
)

target_link_libraries(glfw_utils
    Async++
    bnb_effect_player
    glad
    glfw
)