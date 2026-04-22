#pragma once

#include <bnb/player_api/interfaces/render_target.hpp>
#include <bnb/player_api/interfaces/types/content_mode.hpp>
#include <bnb/player_api/interfaces/types/orientation.hpp>

namespace bnb::player_api
{

    class orientable_scalable_base
    {
    protected:
        void update_content_mode(content_mode new_content_mode);

        void update_orientation(orientation orient, bool mirroring = false);

        const float* const get_orientation_matrix(bool y_invert = false);

        void oriented_frame_size(const render_target_sptr& rt, int32_t& oriented_width, int32_t& oriented_height) const;

        void oriented_frame_layout(const render_target_sptr& rt, int32_t surface_width, int32_t surface_height, int32_t& left, int32_t& top, int32_t& width, int32_t& height) const;

    protected:
        content_mode m_content_mode{content_mode::aspect_fit};
        orientation m_orientation{orientation::up};
        bool m_mirroring{false};
    }; /* class output_base */

} // namespace bnb::player_api
