#include "graphical_user_interface.hpp"

#include <bnb/effect_player/interfaces/effect_manager.hpp>

extern int nvgCreateImage(NVGcontext* ctx, const char* filename, int imageFlags);

namespace bnb::example
{

    /* graphical_user_interface::graphical_user_interface */
    graphical_user_interface::graphical_user_interface(const std::shared_ptr<glfw_window>& window)
        : m_glfw_window(window)
    {
        m_glfw_window->make_context_current();
        m_screen = new nanogui::Screen();
        m_screen->initialize(m_glfw_window->get_window(), false);
        m_gui = new nanogui::FormHelper(m_screen);

        m_screen->setVisible(true);
        m_screen->performLayout();

        m_glfw_window->make_nothing_current();
    }

    /* graphical_user_interface::on_glfw_event */
    void graphical_user_interface::on_glfw_event(const glfw_event& e)
    {
        if (!m_active) {
            return;
        }
        using t = glfw_event_t;
        switch (e.type) {
            case glfw_event_t::framebuffer_resize:
                m_screen->resizeCallbackEvent(e.size_width, e.size_height);
                break;
            case glfw_event_t::mouse_move:
                m_screen->cursorPosCallbackEvent(e.pos_x, e.pos_y);
                break;
            case glfw_event_t::scroll:
                m_screen->scrollCallbackEvent(e.scroll_x, e.scroll_y);
                break;
            case t::drop_files:
                m_screen->dropCallbackEvent(e.file_count, e.file_paths);
                break;
            case glfw_event_t::input_codepoint:
                m_screen->charCallbackEvent(e.codepoint);
                break;
            case glfw_event_t::key_press:
                m_screen->keyCallbackEvent(e.keyboard_key, e.keyboard_scancode, GLFW_PRESS, e.keyboard_mods);
                break;
            case glfw_event_t::key_repeat:
                m_screen->keyCallbackEvent(e.keyboard_key, e.keyboard_scancode, GLFW_REPEAT, e.keyboard_mods);
                break;
            case glfw_event_t::key_release:
                m_screen->keyCallbackEvent(e.keyboard_key, e.keyboard_scancode, GLFW_RELEASE, e.keyboard_mods);
                break;
            case glfw_event_t::mouse_press:
                m_screen->mouseButtonCallbackEvent(e.mouse_button, GLFW_PRESS, e.mouse_mods);
                break;
            case glfw_event_t::mouse_release:
                m_screen->mouseButtonCallbackEvent(e.mouse_button, GLFW_RELEASE, e.mouse_mods);
                break;
            default:
                break;
        }
    }

    /* graphical_user_interface::add_player_control */
    void graphical_user_interface::add_player_control(const bnb::player_api::player_sptr& player)
    {
//        m_glfw_window->make_context_current();
//
//        auto window = m_gui->addWindow(get_new_screen_pos(), "Common parameters");
//        m_gui->addButton("Reload current effect", [player]() {
//            auto em = player->get_effect_player()->effect_manager();
//            if (em) {
//                player->load_async(em->current()->url());
//            }
//        });
//
//        m_gui->addButton("Show texture output", [player]() {
//            auto em = player->get_effect_player()->effect_manager();
//            if (em) {
//                player->load_async(em->current()->url());
//            }
//        });
//
//        auto window_texture = m_gui->addWindow(get_new_screen_pos(), "Texture output");
//        window_texture->setLayout(new nanogui::GroupLayout());
//        window_texture->setSize(Eigen::Vector2i(200, 200));
//
//        auto image_view = new nanogui::ImageView(window_texture, 0);
//        image_view->setFixedSize(Eigen::Vector2i(200, 200));
//        auto output_texture = std::make_shared<bnb::player_api::texture_output>([image_view](bnb::player_api::texture_t texture) {
//            image_view->bindImage(static_cast<GLuint>(reinterpret_cast<uint64_t>(texture)));
//        });
//        player->add_output(output_texture);
//
//        m_screen->performLayout();
//
//        m_glfw_window->make_nothing_current();
    }
    

    /* graphical_user_interface::add_output_control */
    void graphical_user_interface::add_output_control(const std::shared_ptr<bnb::player_api::window_output>& output, const std::string_view& window_name)
    {
        auto find_result = m_output_controls.find(output);
        if (find_result != m_output_controls.end()) {
            throw std::runtime_error("Output already exists.");
        }

        auto window = m_gui->addWindow(get_new_screen_pos(), window_name.data());
        output_control control {window, bnb::player_api::orientation::up, false, bnb::player_api::content_mode::aspect_fit};
        auto it = m_output_controls.emplace(output, control).first;

        output->set_orientation(control.orient, control.mirror);
        output->set_content_mode(control.mode);

        // orientation callbacks
        m_gui->addVariable<bnb::player_api::orientation>(
            "Orientation:",
            [output, this](const bnb::player_api::orientation& v) {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }

                using o = bnb::player_api::orientation;
                static const bnb::player_api::orientation orients[] = {o::up, o::left, o::down, o::right};

                result->second.orient = orients[static_cast<int32_t>(v)];
                output->set_orientation(result->second.orient, result->second.mirror);
            },
            [output, this]() -> bnb::player_api::orientation {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }
                return result->second.orient;
            },
            true
        )->setItems({"up", "left", "down", "right"});

        // mirroring
        m_gui->addVariable<bool>(
            "Mirroring:",
            [output, this](const bool& v) {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }

                result->second.mirror = v;
                output->set_orientation(result->second.orient, result->second.mirror);
            },
            [output, this]() -> bool {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }
                return result->second.mirror;
            },
            true
        )->setTooltip("Mirroring is applied after orientation.");
        
        // content mode
        m_gui->addVariable<bnb::player_api::content_mode>(
            "Content mode:",
            [output, this](const bnb::player_api::content_mode& v) {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }

                using o = bnb::player_api::content_mode;
                static const bnb::player_api::content_mode modes[] = {o::aspect_fit, o::aspect_fill};

                result->second.mode = modes[static_cast<int32_t>(v)];
                output->set_content_mode(result->second.mode);
            },
            [output, this]() -> bnb::player_api::content_mode {
                auto result = m_output_controls.find(output);
                if (result == m_output_controls.end()) {
                    throw std::runtime_error("Output not found.");
                }
                return result->second.mode;
            },
            true
        )->setItems({"aspect_fit", "aspect_fill"});

        // show
        m_screen->performLayout();
    }

    /* graphical_user_interface::remoove_output_control */
    void graphical_user_interface::remoove_output_control(const bnb::player_api::output_sptr& output)
    {
        auto result = m_output_controls.find(output);
        if (result == m_output_controls.end()) {
            throw std::runtime_error("Output not found.");
        }

        m_screen->removeChild(result->second.window.get());
        m_screen->performLayout();

        m_output_controls.erase(result);
    }

    /* graphical_user_interface::draw */
    void graphical_user_interface::draw()
    {
        if (!m_active) {
            return;
        }

        m_screen->drawContents();
        m_screen->drawWidgets();
    }

    /* graphical_user_interface::get_new_screen_pos */
    Eigen::Vector2i graphical_user_interface::get_new_screen_pos()
    {
        auto ret = m_screen_pos;
        
        if (m_screen_pos.x() >= 300) {
            m_screen_pos.x() = 30;
        } else {
            m_screen_pos.x() += 30;
        }
        
        if (m_screen_pos.y() >= 200) {
            m_screen_pos.y() = 30;
        } else {
            m_screen_pos.y() += 30;
        }
        
        return ret;
    }

//    void graphical_user_interface::show_texture_output()
//    {
//        m_texture_output_window = m_gui->addWindow(get_new_screen_pos(), "Texture output");
//        m_texture_output_window->setPosition(get_new_screen_pos());
//        m_texture_output_window->setLayout(new nanogui::GroupLayout());
//
//        auto image_view = new nanogui::ImageView(m_texture_output_window, 0);
//        image_view->setFixedSize(Eigen::Vector2i(300, 200));
//        m_texture_output = std::make_shared<bnb::player_api::texture_output>([image_view](bnb::player_api::texture_t texture) {
//            image_view->bindImage(static_cast<GLuint>(reinterpret_cast<uint64_t>(texture)));
//        });
//        m_player->add_output(output_texture);
//
//        m_screen->performLayout();
//    }
//
//    void graphical_user_interface::hide_texture_output()
//    {
//        m_player->remove_output(m_texture_output);
//        m_screen->removeChild(m_texture_output_window);
//        m_screen->performLayout();
//    }

//    bool graphical_user_interface::is_effect(const std::filesystem::path& path)
//    {
//        if (!std::filesystem::is_directory(path)) {
//            return false;
//        }
//        auto effect_config_file = path / 'config.js';
//        if (!std::filesystem::exists(effect_config_file)) {
//            return false;
//        }
//        auto preview_file = path / 'preview.png';
//        return std::filesystem::exists(preview_file);
//    }
//
//    void graphical_user_interface::add_effect(const std::filesystem::path& path_to_effect)
//    {
//        m_glfw_window->make_context_current();
//
//        if (is_effect(path_to_effect)) {
//            auto i = create_effect_info(path_to_effect);
//            m_effects.push_back(i);
//        } else {
//            std::vector<std:filesystem:::path> subdirs;
//            for(auto& p : std::filesystem::recursive_directory_iterator(path_to_effect)) {
//                if (p.is_directory()) {
//                    if (is_effect(p.path())) {
//                        auto i = create_effect_info(p.path());
//                        m_effects.push_back(i);
//                    }
//                }
//            }
//        }
//    }
//
//    /* graphical_user_interface::create_effect_info */
//    effect_info graphical_user_interface::create_effect_info(const std::filesystem::path& path_to_effect)
//    {
//        GLuint texture = nvgCreateImage(ctx, path_to_effect.string().c_str(), 0);
//        return {texture, path_to_effect};
//    }

} /* namespace bnb::example */

