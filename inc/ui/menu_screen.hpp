#ifndef UI_MENU_SCREEN_HPP
#define UI_MENU_SCREEN_HPP

#include "screen.hpp"
#include "item.hpp"
#include "input.hpp"
#include <cstddef>

namespace ui {

    class MenuScreen : public Screen {
        public:
            MenuScreen(const char* title, MenuItem* items, int itemCount);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;
            void onEnter() override;
            void tickScroll(int elapsedMs) override;

            int selectedIndex() const { return selectedIndex_; }
            void setSelectedIndex(int idx) { if (idx >= 0 && idx < itemCount_) selectedIndex_ = idx; }
            MenuItem* selectedItem() { return &items_[selectedIndex_]; }
            MenuItem* itemAt(int index) { return &items_[index]; }
            int itemCount() const { return itemCount_; }
            const char* title() const { return title_; }

        private:
            const char* title_;
            MenuItem* items_;
            int itemCount_;
            int selectedIndex_ = 0;

            // Scroll state machine
            int scrollOffset_ = 0;
            int scrollTimerMs_ = 0;
            bool scrollPaused_ = true;
            bool hadScrollPause_ = false;
    };

}

#endif
