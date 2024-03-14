set(BNB_PLAYER_API_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/input/stream_input.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/input/stream_input.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/input.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/output.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/pixel_buffer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/player.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/render_context.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/interfaces/render_target.hpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_frame_surface_handler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_frame_surface_handler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_renderbuffer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_renderbuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_shader_program.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/opengl/opengl_shader_program.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/opengl_frame_output.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/opengl_frame_output.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/output_base.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/output_base.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/texture_output.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/texture_output.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/window_output.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/output/window_output.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/pixel_buffer/pixel_buffer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/pixel_buffer/pixel_buffer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/player/player.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/player/player.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/render_target/opengl_render_target.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/render_target/opengl_render_target.cpp

    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/base.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/content_mode.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/orientation.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/pixel_buffer_format.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/yuv_conversion_matrices.hpp
    ${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/types/yuv_conversion_matrices.cpp
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bnb/player_api/third)

add_library(bnb_player_api STATIC ${BNB_PLAYER_API_SOURCES})
target_link_libraries(bnb_player_api yuv bnb_effect_player glad)
target_include_directories(bnb_player_api PUBLIC ${CMAKE_CURRENT_LIST_DIR})
