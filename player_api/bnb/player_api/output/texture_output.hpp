#pragma once

#include <bnb/player_api/output/output_base.hpp>

namespace bnb::player_api
{

    class texture_output : public output_base
    {
    public:
        using texture_ready_callback = std::function<void(const texture_t pb)>;

    public:
        texture_output(const texture_ready_callback& callback);

        ~texture_output() = default;

        void present(const output_sptr&, const render_target_sptr& render_target) override;

    private:
        texture_ready_callback m_callback;
    }; /* class texture_output */

} // namespace bnb::player_api
