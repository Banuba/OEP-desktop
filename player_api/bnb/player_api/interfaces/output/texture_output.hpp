#pragma once

#include <bnb/player_api/interfaces/output.hpp>

namespace bnb::player_api
{

    class BNB_EXPORT texture_output
        : public bnb::player_api::interfaces::output
    {
    public:
        using texture_ready_callback = std::function<void(const texture_t pb)>;

        static std::shared_ptr<texture_output> create(const texture_ready_callback& callback);
    }; // class texture_output

} // namespace bnb::player_api
