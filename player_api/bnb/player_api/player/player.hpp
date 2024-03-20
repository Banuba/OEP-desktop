#pragma once

#include <bnb/player_api/interfaces/player.hpp>
#include <bnb/player_api/interfaces/render_target.hpp>

#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>

namespace bnb::player_api
{

    class player : public bnb::player_api::interfaces::player
    {
    public:
        player(const render_target_sptr& render_target);

        ~player();

        void set_render_mode(render_mode new_render_mode) override;

        void set_render_status_callback(const render_status_callback& callback) override;

        void play() override;

        void pause() override;

        effect_player_sptr get_effect_player() override;

        void use(const input_sptr input) override;

        void use(const output_sptr output) override;

        void use(const std::vector<output_sptr> outputs) override;

        void use(const input_sptr input, const output_sptr output) override;

        void use(const input_sptr input, const std::vector<output_sptr> outputs) override;

        void add_output(const output_sptr output) override;

        void remove_output(const output_sptr output) override;

        effect_sptr load(const std::string& url) override;

        effect_sptr load_async(const std::string& url) override;

        void eval_js(const std::string& script, js_callback_sptr callback) override;

        bool render() override;

    protected:
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
            -> std::future<typename std::invoke_result<F, Args...>::type>;

        bool draw();

        void run_tasks();

        void resize(const bnb::interfaces::full_image_format& format);

        void clear_outputs();

    private:
        std::thread m_thread;
        std::atomic_bool m_thread_started{true};

        std::queue<std::function<void()>> m_tasks;
        std::mutex m_tasks_mutex;
        std::mutex m_manual_render_mutex;
        std::condition_variable m_condition;

        render_target_sptr m_render_target;

        effect_player_sptr m_effect_player;
        bool m_player_initialized{false};

        input_sptr m_input;
        std::vector<output_sptr> m_outputs;

        effect_sptr m_current_effect;

        render_mode m_render_mode{render_mode::loop};

        render_status_callback m_render_callback;
    };


    // add new work item to the pool
    template<class F, class... Args>
    auto player::enqueue(F&& f, Args&&... args)
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

} // namespace bnb::player_api
