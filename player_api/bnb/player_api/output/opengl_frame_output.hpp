#pragma once

#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/output/output_base.hpp>
#include <bnb/player_api/pixel_buffer/pixel_buffer.hpp>

#include <functional>

namespace bnb::player_api
{

    class opengl_frame_output : public bnb::player_api::interfaces::output, public output_base
    {
    public:
        using pixel_buffer_callback = std::function<void(const pixel_buffer_sptr& pb)>;

    public:
        opengl_frame_output(const pixel_buffer_callback& callback, pixel_buffer_format format);

        ~opengl_frame_output() = default;

        void present(const render_target_sptr& render_target) override;

    private:
        void prepare_rgba_data_storage(int32_t width, int32_t height);

        pixel_buffer_sptr convert();

    private:
        pixel_buffer_callback m_pixel_buffer_callback;
        pixel_buffer_format m_format;
        std::unique_ptr<opengl_shader_program> m_shader;

        int32_t m_uniform_texture {0};
        int32_t m_uniform_matrix {0};
    }; /* class frame_output */

} /* namespace bnb::player_api */
