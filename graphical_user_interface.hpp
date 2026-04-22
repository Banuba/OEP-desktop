#pragma once

#include <memory>
#include <nanogui/nanogui.h>
#include "glfw_window.hpp"

#include <vector>
#include <filesystem>

#include <bnb/player_api/interfaces/player.hpp>
#include <bnb/player_api/interfaces/output/window_output.hpp>
#include <bnb/player_api/interfaces/output/texture_output.hpp>

namespace bnb::example
{
    class graphical_user_interface
    {
        private:
            struct output_control
            {
                nanogui::ref<nanogui::Window> window;
                bnb::player_api::orientation orient;
                bool mirror;
                bnb::player_api::content_mode mode;
            };

//            struct effect_info
//            {
//                std::string effect_path;
//                GLuint icon_texture;
//            };
    
        public:
            graphical_user_interface(const std::shared_ptr<glfw_window>& window);
            
            void on_glfw_event(const glfw_event& e);

            void add_player_control(const bnb::player_api::player_sptr& player);

            void add_output_control(const std::shared_ptr<bnb::player_api::window_output>& output, const std::string_view& window_name = "Output params");
            void remoove_output_control(const bnb::player_api::output_sptr& output);

            void draw();

            // path to effect or path to folder with effects
//            void add_effect(const std::filesystem::path& path_to_effect);

            void switch_show_hide_gui()
            {
                m_active = !m_active;
            }
        private:
            Eigen::Vector2i get_new_screen_pos();
            
//            bool is_effect(const std::filesystem::path& path);
            
//            void show_texture_output();
//            void hide_texture_output();
            
//            effect_info create_effect_info(const std::filesystem::path& path_to_effect);

        private:
            bool m_active {true};
            std::shared_ptr<glfw_window> m_glfw_window;
//            std::vector<effect_info> m_effects;

            nanogui::Screen* m_screen;
            nanogui::FormHelper* m_gui;
            Eigen::Vector2i m_screen_pos {Eigen::Vector2i(30, 30)};

            std::map<bnb::player_api::output_sptr, output_control> m_output_controls;

            bnb::player_api::player_sptr m_player;

            nanogui::ref<nanogui::Window> m_texture_output_window;
            std::shared_ptr<bnb::player_api::texture_output> m_texture_output;
    };

} /* namespace bnb::example */
