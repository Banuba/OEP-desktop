#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/types/content_mode.hpp>
#include <bnb/player_api/types/orientation.hpp>

namespace bnb::player_api
{

    class output_base
    {
    public:
        output_base() = default;

        void set_content_mode(content_mode new_content_mode);

        void set_orientation(orientation orient, bool mirroring = false);

    protected:
        const float* const get_orientation_matrix(bool y_invert = false);

        void oriented_frame_size(const render_target_sptr& render_target, int32_t& oriented_width, int32_t& oriented_height) const;

        void oriented_frame_layout(const render_target_sptr& rt, int32_t surface_width, int32_t surface_height, int32_t& left, int32_t& top, int32_t& width, int32_t& height) const;

    protected:
        content_mode m_content_mode {content_mode::aspect_fit};
        orientation m_orientation {orientation::up};
        bool m_mirroring {false};
    }; /* class output_base */

} /* namespace bnb::player_api */
