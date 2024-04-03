#include <bnb/player_api/interfaces/player/player.hpp>

#include <bnb/player_api/utils/common.hpp>
#include <bnb/effect_player/interfaces/all.hpp>
#include <bnb/types/interfaces/all.hpp>

#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include <chrono>

namespace
{

    using namespace bnb::player_api;

    class player_impl
        : public bnb::player_api::player
    {
    public:
        player_impl(uint32_t fps, const render_target_sptr& render_target, const render_delegate_sptr& render_delegate)
            : m_render_target(render_target)
            , m_render_delegate(render_delegate)
        {
            if (fps <= 0) {
                throw std::logic_error("FPS must be greater than zero.");
            }
            validate_not_null(m_render_target);
            validate_not_null(m_render_delegate);

            constexpr auto ns = std::chrono::nanoseconds(std::chrono::seconds(1)).count();
            const auto interval = std::chrono::nanoseconds(static_cast<uint64_t>(static_cast<double>(ns) * 1.1 / static_cast<double>(fps)));

            auto thread_func = [this, interval]() {
                m_render_delegate->activate();
                m_render_target->attach();

                while (m_thread_started) {
                    if (m_render_mode == render_mode::loop) {
                        const auto next_frame_time = std::chrono::steady_clock::now() + interval;
                        m_render_delegate->activate();
                        run_tasks();
                        draw();
                        std::this_thread::sleep_until(next_frame_time);
                    } else { // render_mode::manual
                        m_render_delegate->activate();
                        run_tasks();
                        std::unique_lock<std::mutex> lock(m_manual_render_mutex);
                        m_condition.wait(lock);
                    }
                }

                run_tasks();
                m_render_target->detach();
            };

            m_thread = std::thread(thread_func);

            enqueue([this] {
                auto ep_conf = bnb::interfaces::effect_player_configuration::create(1, 1);
                m_effect_player = bnb::interfaces::effect_player::create(ep_conf);
                m_effect_player->surface_created(1, 1);
            }).get();
        }

        ~player_impl() override
        {
            enqueue([this] {
                for (auto& o : m_outputs) {
                    o->detach();
                }
                m_outputs.clear();
                m_effect_player->surface_destroyed();
            });
            m_thread_started = false;
            m_condition.notify_all();
            m_thread.join();
        }

        void set_render_mode(render_mode new_render_mode) override
        {
            enqueue([this, new_render_mode]() {
                m_render_mode = new_render_mode;
            });
        }

        void play() override
        {
            enqueue([this]() {
                m_effect_player->playback_play();
            });
        }

        void pause() override
        {
            enqueue([this]() {
                m_effect_player->playback_pause();
            });
        }

        effect_player_sptr get_effect_player() override
        {
            return m_effect_player;
        }

        player& use(const input_sptr& input) override
        {
            enqueue([this, input]() {
                m_input = input;
                m_effect_player->set_frame_processor(m_input == nullptr ? nullptr : m_input->get_frame_processor());
            });
            return *this;
        }

        player& use(const output_sptr& output) override
        {
            validate_not_null(output);
            enqueue([this, output]() {
                output->attach();
                m_outputs.push_back(output);
            });
            return *this;
        }

        player& unuse(const output_sptr& output) override
        {
            enqueue([this, output]() {
                if (output == nullptr) {
                    // remove all outputs
                    for (const auto output : m_outputs) {
                        output->detach();
                    }
                    m_outputs.clear();
                } else {
                    // remove only the specified output
                    auto compare = [output](const output_sptr& o) -> bool {
                        auto ret = o.get() == output.get();
                        if (ret) {
                            o->detach();
                        }
                        return ret;
                    };
                    m_outputs.erase(std::remove_if(m_outputs.begin(), m_outputs.end(), compare), m_outputs.end());
                }
            });
            return *this;
        }

        effect_sptr load(const std::string& url) override
        {
            // clang-format off
            return enqueue([this](const std::string& url) {
                if (auto effect_manager = m_effect_player->effect_manager()) {
                    return m_current_effect = effect_manager->load(url);
                }
                return m_current_effect = nullptr;
            }, url).get();
            // clang-format on
        }

        effect_sptr load_async(const std::string& url) override
        {
            if (auto effect_manager = m_effect_player->effect_manager()) {
                return m_current_effect = effect_manager->load_async(url);
            }
            return m_current_effect = nullptr;
        }

        void eval_js(const std::string& script, js_callback_sptr callback) override
        {
            if (m_current_effect != nullptr) {
                m_current_effect->eval_js(script, callback);
            } else if (auto effect_manager = m_effect_player->effect_manager()) {
                effect_manager->current()->eval_js(script, callback);
            }
        }

        int64_t render() override
        {
            // clang-format off
            return enqueue([this]() -> bool {
                if (m_render_mode == render_mode::loop) {
                    throw std::runtime_error("Cannot render manually in not `manual` render mode.");
                }
                return draw();
            }).get();
            // clang-format on
        }

    protected:
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
            -> std::future<typename std::invoke_result<F, Args...>::type>
        {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_tasks_mutex);

                // don't allow enqueueing after stopping the pool
                if (!m_thread_started) {
                    throw std::runtime_error("enqueue on stopped thread_pool");
                }

                m_tasks.emplace([task]() { (*task)(); });
            }
            m_condition.notify_one();

            return res;
        }

        int64_t draw()
        {
            m_render_delegate->started();
            auto frame_number = draw_without_notifications();
            m_render_delegate->finished(frame_number);
            return frame_number;
        }

        int64_t draw_without_notifications()
        {
            auto is_not_active = m_effect_player == nullptr || m_effect_player->get_playback_state() != bnb::interfaces::effect_player_playback_state::active;
            auto is_drawing_forbidden = !m_thread_started || is_not_active || m_outputs.empty() || m_input == nullptr;
            if (is_drawing_forbidden) {
                return -1;
            }

            auto frame_processor = m_input->get_frame_processor();
            auto processor_result = frame_processor->pop();
            if (processor_result.frame_data == nullptr) {
                return -1;
            }

            m_render_target->set_frame_time_us(m_input->get_frame_time_us());
            resize(processor_result.frame_data->get_full_img_format());
            auto old_size = m_effect_player->effect_manager()->surface_size();
            m_render_target->prepare_to_offscreen_render(old_size.width, old_size.height);

            auto frame_number = m_effect_player->draw_with_external_frame_data(processor_result.frame_data);
            if (frame_number < 0) {
                return -1;
            }

            for (const auto output : m_outputs) {
                output->present(m_render_target);
            }

            return frame_number;
        }

        void run_tasks()
        {
            std::unique_lock<std::mutex> lock(m_tasks_mutex);
            if (!m_tasks.empty()) {
                do {
                    m_tasks.front()();
                    m_tasks.pop();
                } while (!m_tasks.empty());
            }
        }

        void resize(const bnb::interfaces::full_image_format& format)
        {
            const auto is_vertical = format.orientation == bnb::interfaces::rotation::deg_0
                                     || format.orientation == bnb::interfaces::rotation::deg_180;
            const auto width = is_vertical ? format.width : format.height;
            const auto height = is_vertical ? format.height : format.width;

            const auto size = m_effect_player->effect_manager()->surface_size();
            if (size.width != width || size.height != height) {
                m_effect_player->surface_changed(width, height);
                m_effect_player->effect_manager()->set_effect_size(width, height);
            }
        }

    private:
        std::thread m_thread;
        std::atomic_bool m_thread_started{true};
        std::queue<std::function<void()>> m_tasks;
        std::mutex m_tasks_mutex;
        std::mutex m_manual_render_mutex;
        std::condition_variable m_condition;
        render_target_sptr m_render_target;
        effect_player_sptr m_effect_player;
        input_sptr m_input;
        std::vector<output_sptr> m_outputs;
        effect_sptr m_current_effect;
        render_mode m_render_mode{render_mode::loop};
        render_delegate_sptr m_render_delegate;
    }; // class player_impl

} // namespace

using namespace bnb::player_api;
std::shared_ptr<player> player::create(uint32_t fps, const render_target_sptr& render_target, const render_delegate_sptr& render_delegate)
{
    return std::make_shared<player_impl>(fps, render_target, render_delegate);
}
