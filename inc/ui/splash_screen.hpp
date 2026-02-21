#ifndef UI_SPLASH_SCREEN_HPP
#define UI_SPLASH_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"

namespace ui {

    class SplashScreen : public Screen {
        public:
            using DismissCallback = void (*)(void* userData);

            SplashScreen(const char* title, const char* subtitle = "",
                         const char* comment = "", DismissCallback onDismiss = nullptr,
                         void* userData = nullptr);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;
            void tickScroll(int elapsedMs) override;

            void setTitle(const char* t) { title_ = t; markDirty(); }
            void setSubtitle(const char* s) { subtitle_ = s; markDirty(); }
            void setComment(const char* c) { comment_ = c; markDirty(); }

        private:
            const char* title_;
            const char* subtitle_;
            const char* comment_;
            DismissCallback onDismiss_;
            void* userData_;

            int scrollOffset_ = 0;
            int scrollTimerMs_ = 0;
            bool scrollPaused_ = true;
            bool hadScrollPause_ = false;
    };

}

#endif
