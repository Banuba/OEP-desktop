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

namespace
{

    using namespace bnb::player_api;

    class player_impl
        : public bnb::player_api::player
    {
    public:
        player_impl(const render_target_sptr& render_target)
            : m_render_target(render_target)
        {
            validate_not_null(render_target);
            auto thread_func = [this]() {
                while (m_thread_started) {
                    run_tasks();
                    if (m_render_mode == render_mode::loop) {
                        draw();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    } else { // render_mode::manual
                        std::unique_lock<std::mutex> lock(m_manual_render_mutex);
                        m_condition.wait(lock);
                    }
                }

                run_tasks();
            };

            m_thread = std::thread(thread_func);

            enqueue([this] {
                m_render_target->activate();
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

        void set_rendering_process_callback(const std::shared_ptr<rendering_process>& callback) override
        {
            enqueue([this, cb = callback]() {
                m_render_callback = std::move(cb);
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
        
        player& in(const input_sptr& input) override
        {
            enqueue([this, input]() {
                m_input = input;
                m_effect_player->set_frame_processor(m_input == nullptr ? nullptr : m_input->get_frame_processor());
            });
            return *this;
        }

        player& out(const output_sptr& output) override
        {
            validate_not_null(output);
            enqueue([this, output]() {
                output->attach();
                m_outputs.push_back(output);
            });
            return *this;
        }

        player& out_once(const output_sptr& output) override
        {
            validate_not_null(output);
            enqueue([this, output]() {
                output->attach();
                m_once_outputs.push_back(output);
            });
            return *this;
        }

        player& remove_out(const output_sptr& output) override
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
            m_render_target->activate();
            if (auto cb = m_render_callback; cb) {
                cb->started();
                auto frame_number = draw_without_notifications();
                m_render_callback->frame_rendered(frame_number);
                cb->finished();
                return frame_number;
            } else {
                return draw_without_notifications();
            }
        }

        int64_t draw_without_notifications()
        {
            auto is_not_active = m_effect_player == nullptr || m_effect_player->get_playback_state() != bnb::interfaces::effect_player_playback_state::active;
            auto is_drawing_forbidden = !m_thread_started || is_not_active || (m_outputs.empty() && m_once_outputs.empty()) || m_input == nullptr;
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

            for (const auto output : m_once_outputs) {
                output->present(m_render_target);
            }
            m_once_outputs.clear();

            return frame_number;
        }

        void run_tasks()
        {
            std::unique_lock<std::mutex> lock(m_tasks_mutex);
            if (!m_tasks.empty()) {
                m_render_target->activate();
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
        std::vector<output_sptr> m_once_outputs;
        effect_sptr m_current_effect;
        render_mode m_render_mode{render_mode::loop};
        std::shared_ptr<rendering_process> m_render_callback;
    }; // class player_impl

} // namespace

std::shared_ptr<bnb::player_api::player> bnb::player_api::player::create(const bnb::player_api::render_target_sptr& render_target)
{
    return std::make_shared<player_impl>(render_target);
}
