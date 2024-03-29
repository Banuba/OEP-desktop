#pragma once

#include <stdexcept>

namespace bnb::player_api
{

    template <class T>
    void validate_not_null(const T& ptr) {
        if (ptr == nullptr) {
            throw std::runtime_error("Pointer cannot be `nullptr`.");
        }
    }

} // namespace bnb::player_api
