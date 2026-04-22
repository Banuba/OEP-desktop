#include <bnb/player_api/interfaces/output/texture_output.hpp>

namespace
{

    using namespace bnb::player_api;

    class texture_output_impl
        : public texture_output
    {
    public:
        texture_output_impl(const texture_ready_callback& callback)
            : m_callback(callback)
        {
        }

        ~texture_output_impl() = default;

        void attach() override
        {
        }

        void detach() override
        {
        }

        void present(const render_target_sptr& render_target) override
        {
            m_callback(render_target->get_output_texture());
        }

    private:
        texture_ready_callback m_callback;
    }; // class texture_output_impl

} // namespace

using t = bnb::player_api::texture_output;
std::shared_ptr<t> t::create(const t::texture_ready_callback& callback)
{
    return std::make_shared<texture_output_impl>(callback);
}
