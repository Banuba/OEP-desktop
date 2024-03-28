#pragma once

#include <bnb/player_api/interfaces/pixel_buffer.hpp>
#include <bnb/player_api/interfaces/output.hpp>
#include <bnb/player_api/output/orientable.hpp>

#include <functional>

namespace bnb::player_api
{

    class opengl_frame_output
        : public bnb::player_api::interfaces::output
        , public bnb::player_api::orientable
    {
    public:
        using pixel_buffer_callback = std::function<void(const output_sptr& self, const pixel_buffer_sptr& pb)>;

        static std::shared_ptr<opengl_frame_output> create(const pixel_buffer_callback& callback, pixel_buffer_format format);
    }; // class opengl_frame_output

} // namespace bnb::player_api
