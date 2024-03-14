#pragma once

#include <string_view>

namespace bnb::player_api
{

    class opengl_shader_program
    {
    public:
        opengl_shader_program(const std::string_view& vertex_shader_source, const std::string_view& fragmant_shader_source);
        ~opengl_shader_program();

        void use() const;
        static void unuse();

        int32_t get_program_id() const;
        int32_t get_attribute_location(const std::string_view& attribute_name) const;
        int32_t get_uniform_location(const std::string_view& uniform_name) const;

        void set_uniform_texture(int32_t uniform, uint32_t texture, uint32_t texture_index = 0) const;
        void set_uniform_vec2(int32_t uniform, const float* const vec) const;
        void set_uniform_vec4(int32_t uniform, const float* const vec) const;
        void set_uniform_mat4(int32_t uniform, const float* const mat) const;

    private:
        uint32_t m_program {0};
    }; /* class bnb::player_api::opengl_shader_program */

} /* namespace bnb::player_api */
