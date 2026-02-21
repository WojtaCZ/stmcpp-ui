#ifndef UI_SCREEN_HPP
#define UI_SCREEN_HPP

#include <cstdint>
#include <gfx/framebuffer.hpp>

namespace ui {

    struct Theme;
    enum class InputEvent : uint8_t;

    class Screen {
        public:
            virtual ~Screen() = default;

            virtual bool handleInput(InputEvent event) = 0;
            virtual void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) = 0;

            virtual void onEnter() {}
            virtual void onExit() {}

            virtual void tickScroll(int elapsedMs) { (void)elapsedMs; }

            bool isDirty() const { return dirty_; }
            void markDirty() { dirty_ = true; }
            void clearDirty() { dirty_ = false; }

        protected:
            bool dirty_ = true;
    };

}

#endif
