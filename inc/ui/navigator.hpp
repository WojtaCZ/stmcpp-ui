#ifndef UI_NAVIGATOR_HPP
#define UI_NAVIGATOR_HPP

#include "screen.hpp"
#include "input.hpp"
#include "item.hpp"
#include <cstdint>

namespace ui {

    class Navigator {
        public:
            static constexpr int MAX_DEPTH = 8;

            Navigator() = default;
            explicit Navigator(const Theme& theme);

            void setTheme(const Theme& theme) { theme_ = &theme; }

            void push(Screen* screen);
            void pop();
            void reset(Screen* screen);

            void handleInput(InputEvent event);
            bool render(gfx::IFrameBuffer<uint8_t>& fb);
            void tickScroll(int elapsedMs);
            void forceRender();

            Screen* current() const;
            int depth() const { return depth_; }

        private:
            const Theme* theme_ = nullptr;
            Screen* stack_[MAX_DEPTH] = {};
            int depth_ = 0;
    };

}

#endif
