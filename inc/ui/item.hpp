#ifndef UI_ITEM_HPP
#define UI_ITEM_HPP

#include <cstdint>
#include "theme.hpp"

namespace ui {

    class Screen;

    class MenuItem {
        public:
            enum class Type : uint8_t {
                Action,
                Submenu,
                Back,
                Checkbox,
            };

            using Callback = void (*)(MenuItem* item, void* userData);

        private:
            const char* title_ = "";
            Type type_ = Type::Action;
            Callback callback_ = nullptr;
            void* userData_ = nullptr;
            Screen* submenu_ = nullptr;
            bool checked_ = false;

        public:
            MenuItem() = default;

            MenuItem(const char* title, Callback callback, void* userData = nullptr)
                : title_(title), type_(Type::Action), callback_(callback), userData_(userData) {}

            MenuItem(const char* title, Screen* submenu)
                : title_(title), type_(Type::Submenu), submenu_(submenu) {}

            static MenuItem back(const char* title = "Back") {
                MenuItem item;
                item.title_ = title;
                item.type_ = Type::Back;
                return item;
            }

            static MenuItem checkbox(const char* title, bool checked, Callback callback = nullptr, void* userData = nullptr) {
                MenuItem item;
                item.title_ = title;
                item.type_ = Type::Checkbox;
                item.checked_ = checked;
                item.callback_ = callback;
                item.userData_ = userData;
                return item;
            }

            const char* title() const { return title_; }
            Type type() const { return type_; }
            Screen* submenu() const { return submenu_; }
            bool isChecked() const { return checked_; }

            void setChecked(bool c) { checked_ = c; }
            void setTitle(const char* t) { title_ = t; }

            void invoke() {
                if (callback_)
                    callback_(this, userData_);
            }

            IconRef getIcon(const Theme& theme, bool selected) const {
                switch (type_) {
                    case Type::Checkbox:
                        if (selected)
                            return checked_ ? theme.checkboxSelChecked : theme.checkboxSelEmpty;
                        else
                            return checked_ ? theme.checkboxUnselChecked : theme.checkboxUnselEmpty;
                    case Type::Back:
                        return selected ? theme.backSelected : theme.backUnselected;
                    case Type::Action:
                    case Type::Submenu:
                    default:
                        return selected ? theme.dotSelected : theme.dotUnselected;
                }
            }
    };

}

#endif
