#pragma once

#include <glad/glad.h>
#include <algorithm>
#include <interfaces/image_format.hpp>

#define BNB_GLSL_VERSION "#version 330 core \n"

namespace bnb::render
{
    class frame_surface_handler
    {
    private:
        static const auto v_size = static_cast<uint32_t>(bnb::oep::interfaces::rotation::deg270) + 1;

    public:
        /**
         * First array determines texture orientation for vertical flip transformation
         * Second array determines texture's orientation
         * Third one determines the plane vertices` positions in correspondence to the texture coordinates
         */
        static const float vertices[2][v_size][5 * 4];

        explicit frame_surface_handler(bnb::oep::interfaces::rotation orientation, bool is_y_flip);
        virtual ~frame_surface_handler() final;

        frame_surface_handler(const frame_surface_handler&) = delete;
        frame_surface_handler(frame_surface_handler&&) = delete;

        frame_surface_handler& operator=(const frame_surface_handler&) = delete;
        frame_surface_handler& operator=(frame_surface_handler&&) = delete;

        void draw();

    private:
        uint32_t m_orientation = 0;
        uint32_t m_y_flip = 0;
        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        unsigned int m_ebo = 0;
    };
} // namespace bnb::render
