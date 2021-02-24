set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(offscreen_rt STATIC ${srcs})

target_include_directories(offscreen_rt PUBLIC
    ${include_dirs}
)

target_include_directories(offscreen_rt SYSTEM PUBLIC ${LIBYUV_INCLUDE_DIRS})
target_include_directories(offscreen_rt PUBLIC ${PROJECT_SOURCE_DIR})

if (APPLE)
    set(apple_includes
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/include/
    )

    file(GLOB_RECURSE apple_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/src/*.mm
    )

    target_sources(offscreen_rt PUBLIC ${apple_srcs})
    target_include_directories(offscreen_rt PUBLIC ${apple_includes})
endif()

target_link_libraries(offscreen_rt
    glad
    glfw
    ogl_utils
)