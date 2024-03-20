#include <bnb/player_api/opengl/opengl_shader_program.hpp>

#include <bnb/player_api/opengl/opengl.hpp>
#include <string>

namespace bnb::player_api
{

    /* opengl_shader_program::opengl_shader_program */
    opengl_shader_program::opengl_shader_program(const std::string_view& vertex_shader_source, const std::string_view& fragmant_shader_source)
    {
        int success;
        char info_log[512];

        const char* const vertex_full_shader_source[] = {BNB_GLSL_VERSION_STRING, vertex_shader_source.data()};
        int32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GL_CALL(glShaderSource(vertex_shader, 2, vertex_full_shader_source, NULL));
        GL_CALL(glCompileShader(vertex_shader));

        // check for shader compile errors
        GL_CALL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success));
        if (!success) {
            GL_CALL(glGetShaderInfoLog(vertex_shader, 512, NULL, info_log));
            throw std::runtime_error(info_log);
        }

        // fragment shader
        const char* const fragment_full_shader_source[] = {BNB_GLSL_VERSION_STRING, fragmant_shader_source.data()};
        int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        GL_CALL(glShaderSource(fragment_shader, 2, fragment_full_shader_source, NULL));
        GL_CALL(glCompileShader(fragment_shader));

        // check for shader compile errors
        GL_CALL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success));
        if (!success) {
            GL_CALL(glGetShaderInfoLog(fragment_shader, 512, NULL, info_log));
            throw std::runtime_error(info_log);
        }

        // link shaders
        uint32_t shader_program = glCreateProgram();
        GL_CALL(glAttachShader(shader_program, vertex_shader));
        GL_CALL(glAttachShader(shader_program, fragment_shader));
        GL_CALL(glLinkProgram(shader_program));

        // check for linking errors
        GL_CALL(glGetProgramiv(shader_program, GL_LINK_STATUS, &success));
        if (!success) {
            GL_CALL(glDeleteShader(vertex_shader));
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);
            throw std::runtime_error(info_log);
        }

        GL_CALL(glDeleteShader(vertex_shader));
        GL_CALL(glDeleteShader(fragment_shader));

        m_program = shader_program;
    }

    /* opengl_shader_program::~opengl_shader_program */
    opengl_shader_program::~opengl_shader_program()
    {
        GL_CALL(glDeleteProgram(m_program));
    }

    /* opengl_shader_program::use */
    void opengl_shader_program::use() const
    {
        GL_CALL(glUseProgram(m_program));
    }

    /* opengl_shader_program::unuse */
    void opengl_shader_program::unuse()
    {
        GL_CALL(glUseProgram(0));
    }

    /* opengl_shader_program::get_program_id */
    int32_t opengl_shader_program::get_program_id() const
    {
        return m_program;
    }

    /* opengl_shader_program::get_attribute_location */
    int32_t opengl_shader_program::get_attribute_location(const std::string_view& attribute_name) const
    {
        int32_t attribute = glGetAttribLocation(m_program, attribute_name.data());
        if (attribute == -1) {
            auto msg = std::string("Unknown attribute name: ");
            msg.append(attribute_name);
            throw std::runtime_error(msg);
        }
        return attribute;
    }

    /* opengl_shader_program::get_uniform_location */
    int32_t opengl_shader_program::get_uniform_location(const std::string_view& uniform_name) const
    {
        return glGetUniformLocation(m_program, uniform_name.data());
    }

    /* opengl_shader_program::set_uniform_texture_unit */
    void opengl_shader_program::set_uniform_texture_unit(int32_t uniform, uint32_t texture_unit_index) const
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit_index));
        GL_CALL(glUniform1i(uniform, texture_unit_index));
    }

    /* opengl_shader_program::set_uniform_vec2 */
    void opengl_shader_program::set_uniform_vec2(int32_t uniform, const float* const vec) const
    {
        GL_CALL(glUniform2fv(uniform, 1, vec));
    }

    /* opengl_shader_program::set_uniform_vec4 */
    void opengl_shader_program::set_uniform_vec4(int32_t uniform, const float* const vec) const
    {
        GL_CALL(glUniform4fv(uniform, 1, vec));
    }

    /* opengl_shader_program::set_uniform_mat4 */
    void opengl_shader_program::set_uniform_mat4(int32_t uniform, const float* const mat, bool transpose) const
    {
        GL_CALL(glUniformMatrix4fv(uniform, 1, static_cast<GLboolean>(transpose), mat));
    }

    /* opengl_shader_program::set_uniform_int */
    void opengl_shader_program::set_uniform_int(int32_t uniform, int32_t val) const
    {
        GL_CALL(glUniform1i(uniform, val));
    }

} // namespace bnb::player_api
