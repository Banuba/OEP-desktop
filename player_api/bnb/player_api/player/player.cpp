#include <bnb/player_api/interfaces/player/player.hpp>
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

    class player_impl : public bnb::player_api::player
    {
    public:
        player_impl(const render_target_sptr& render_target)
            : m_render_target(render_target)
        {
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

        void set_render_status_callback(const render_status_callback& callback) override
        {
            enqueue([this, cb = std::move(callback)]() {
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
        
        void use(const input_sptr input) override
        {
            enqueue([this, input]() {
                m_input = input;
                m_effect_player->set_frame_processor(m_input->get_frame_processor());
            });
        }

        void use(const output_sptr output) override
        {
            std::vector<output_sptr> outputs {output};
            use(outputs);
        }

        void use(const std::vector<output_sptr> outputs) override
        {
            enqueue([this, outputs]() {
                for (auto& o : m_outputs) {
                    o->detach();
                }
                m_outputs = outputs;
                for (auto& o : outputs) {
                    o->attach();
                }
            });
        }

        void use(const input_sptr input, const output_sptr output) override
        {
            std::vector<output_sptr> outputs {output};
            use(input);
            use(outputs);
        }

        void use(const input_sptr input, const std::vector<output_sptr> outputs) override
        {
            use(input);
            use(outputs);
        }

        void add_output(const output_sptr output) override
        {
            enqueue([this, output]() {
                m_outputs.push_back(output);
                output->attach();
            });
        }

        void remove_output(const output_sptr output) override
        {
            // clang-format off
            enqueue([this, output]() {
                m_outputs.erase(std::remove_if(m_outputs.begin(), m_outputs.end(), [output](const output_sptr& o) {
                    auto ret = o.get() == output.get();
                    if (ret) {
                        o->detach();
                    }
                    return ret;
                }),
                m_outputs.end());
            });
            // clang-format on
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

        bool render() override
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

        bool draw()
        {
            auto is_not_active = m_effect_player == nullptr || m_effect_player->get_playback_state() != bnb::interfaces::effect_player_playback_state::active;
            auto is_drawing_forbidden = !m_thread_started || is_not_active || m_outputs.empty() || m_input == nullptr;
            if (is_drawing_forbidden) {
                if (m_render_callback != nullptr) {
                    m_render_callback(-1);
                }
                return false;
            }

            auto frame_processor = m_input->get_frame_processor();
            auto processor_result = frame_processor->pop();
            if (processor_result.frame_data == nullptr) {
                if (m_render_callback != nullptr) {
                    m_render_callback(-1);
                }
                return false;
            }

            m_render_target->activate();
            m_render_target->set_frame_time_us(m_input->get_frame_time_us());
            resize(processor_result.frame_data->get_full_img_format());
            auto old_size = m_effect_player->effect_manager()->surface_size();
            m_render_target->prepare_to_offscreen_render(old_size.width, old_size.height);

            auto frame_number = m_effect_player->draw_with_external_frame_data(processor_result.frame_data);
            if (frame_number < 0) {
                if (m_render_callback != nullptr) {
                    m_render_callback(-1);
                }
                return false;
            }

            for (const auto output : m_outputs) {
                if (output->is_active()) {
                    output->present(output, m_render_target);
                }
            }

            if (m_render_callback != nullptr) {
                m_render_callback(frame_number);
            }
            return true;
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
        effect_sptr m_current_effect;
        render_mode m_render_mode{render_mode::loop};
        render_status_callback m_render_callback;
    }; // class player_impl

} // namespace

std::shared_ptr<bnb::player_api::player> bnb::player_api::player::create(const bnb::player_api::render_target_sptr& render_target)
{
    return std::make_shared<player_impl>(render_target);
}
