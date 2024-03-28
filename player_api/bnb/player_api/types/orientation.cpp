#include <bnb/player_api/interfaces/types/orientation.hpp>

#include <bnb/types/base_types.hpp>
#include <stdexcept>

namespace bnb::player_api
{

    orientation degrees_to_orientation(int32_t degrees)
    {
        degrees = degrees % 360;
        if (degrees == 0) {
            return orientation::up;
        } else if (degrees == 90) {
            return orientation::left;
        } else if (degrees == 180) {
            return orientation::down;
        } else if (degrees == 270) {
            return orientation::right;
        }
        throw std::invalid_argument("Unknown degrees value.");
    }

    orientation camera_orientation_to_orientation(bnb::camera_orientation orient)
    {
        switch (orient) {
            case bnb::camera_orientation::deg_0:
                return orientation::up;
            case bnb::camera_orientation::deg_90:
                return orientation::left;
            case bnb::camera_orientation::deg_180:
                return orientation::down;
            case bnb::camera_orientation::deg_270:
                return orientation::right;
        }
    }

    bnb::camera_orientation orientation_to_camera_orientation(orientation orient)
    {
        switch (orient) {
            case orientation::up:
                return bnb::camera_orientation::deg_0;
            case orientation::left:
                return bnb::camera_orientation::deg_90;
            case orientation::down:
                return bnb::camera_orientation::deg_180;
            case orientation::right:
                return bnb::camera_orientation::deg_270;
        }
    }

} // namespace bnb::player_api
