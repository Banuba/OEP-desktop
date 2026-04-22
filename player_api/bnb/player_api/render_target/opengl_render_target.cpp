#include <bnb/player_api/interfaces/render_target/opengl_render_target.hpp>

#include <bnb/player_api/opengl/opengl.hpp>
#include <bnb/player_api/opengl/opengl_shader_program.hpp>
#include <bnb/player_api/opengl/opengl_frame_surface_handler.hpp>
#include <bnb/player_api/opengl/opengl_renderbuffer.hpp>
#include <bnb/effect_player/interfaces/effect_player.hpp>
#include <bnb/effect_player/utility.hpp>

namespace
{

    using namespace bnb::player_api;

    // clang-format off
    constexpr std::string_view vertex_shader_source =
        "precision highp float;\n "
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 vTexCoord;\n"
        "uniform mat4 uMatrix;\n"
        "void main() {\n"
        "  gl_Position = uMatrix * vec4(aPos, 1.0);\n"
        "  vTexCoord = aTexCoord;\n"
        "}\n";

    constexpr std::string_view fragment_shader_source =
        "precision highp float;\n"
        "in vec2 vTexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "  FragColor = texture(uTexture, vTexCoord);\n"
        "}\n";
    // clang-format on

    class opengl_render_target_impl
        : public bnb::player_api::opengl_render_target
    {
    public:
        opengl_render_target_impl()
        {
            // This particular example relies on OpenGL, so it should be explicitly requested
            bnb::interfaces::effect_player::set_render_backend(::bnb::interfaces::render_backend_type::opengl);

            bnb::utility::load_gl_functions();
        }

        ~opengl_render_target_impl() = default;

        void attach() override
        {
            m_shader = std::make_unique<opengl_shader_program>(vertex_shader_source, fragment_shader_source);
            m_frame_handler = std::make_unique<opengl_frame_surface_handler>();

            m_shader->use();
            m_uniform_texture = m_shader->get_uniform_location("uTexture");
            m_uniform_matrix = m_shader->get_uniform_location("uMatrix");
            opengl_shader_program::unuse();

            m_renderbuffer = std::make_unique<opengl_renderbuffer>();
        }

        void detach() override
        {
            m_shader = nullptr;
            m_frame_handler = nullptr;
            m_renderbuffer = nullptr;
        }

        void prepare_to_offscreen_render(int32_t width, int32_t height) override
        {
            m_renderbuffer->prepare(width, height);

            GL_CALL(glViewport(0, 0, width, height));
            GL_CALL(glDisable(GL_CULL_FACE));
            GL_CALL(glDisable(GL_DEPTH_TEST));
            GL_CALL(glEnable(GL_PROGRAM_POINT_SIZE));
        }

        void prepare_to_screen_render() override
        {
            opengl_renderbuffer::unbind();
        }

        void set_frame_time_us(uint64_t time_us) noexcept override
        {
            m_frame_time_us = time_us;
        }

        uint64_t get_frame_time_us() const noexcept override
        {
            return m_frame_time_us;
        }

        int32_t get_render_width() const noexcept override
        {
            return m_renderbuffer->get_width();
        }

        int32_t get_render_height() const noexcept override
        {
            return m_renderbuffer->get_height();
        }

        texture_t get_output_texture() const noexcept override
        {
            return static_cast<texture_t>(m_renderbuffer->get_texture());
        }

        void present(int32_t left, int32_t top, int32_t width, int32_t height, const float* const mat4) override
        {
            constexpr uint32_t texture_unit = 0;

            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glDisable(GL_CULL_FACE));
            GL_CALL(glDisable(GL_DEPTH_TEST));
            GL_CALL(glViewport(left, top, width, height));

            m_shader->use();
            m_shader->set_uniform_mat4(m_uniform_matrix, mat4, true);
            m_shader->set_uniform_texture_unit(m_uniform_texture, texture_unit);
            GL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_renderbuffer->get_texture()));
            m_frame_handler->draw_surface();
            opengl_shader_program::unuse();
        }

    private:
        std::unique_ptr<opengl_shader_program> m_shader;
        std::unique_ptr<opengl_frame_surface_handler> m_frame_handler;
        std::unique_ptr<opengl_renderbuffer> m_renderbuffer;

        uint64_t m_frame_time_us{0};

        int32_t m_uniform_texture{0};
        int32_t m_uniform_matrix{0};
        int32_t m_uniform_invert_y{0};
    }; // class opengl_render_target_impl

} // namespace

std::shared_ptr<bnb::player_api::opengl_render_target> bnb::player_api::opengl_render_target::create()
{
    return std::make_shared<opengl_render_target_impl>();
}
