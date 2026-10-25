#ifndef UI_SPLASH_SCREEN_HPP
#define UI_SPLASH_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"
#include "text_scroll.hpp"

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
            void setSubtitle(const char* s) { subtitle_ = s; subtitleScroll_.reset(); markDirty(); }
            void setComment(const char* c) { comment_ = c; commentScroll_.reset(); markDirty(); }
            // Optional second comment line under the first; with it the title and subtitle move up
            void setComment2(const char* c) { comment2_ = c; comment2Scroll_.reset(); markDirty(); }

        private:
            void drawComment(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme,
                             const char* text, const TextScroll& scroll, int y);

            const char* title_;
            const char* subtitle_;
            const char* comment_;
            const char* comment2_ = nullptr;
            DismissCallback onDismiss_;
            void* userData_;

            // Subtitle and comments longer than the display scroll; characters per line known after render
            TextScroll subtitleScroll_;
            int subtitleMaxChars_ = 0;
            TextScroll commentScroll_;
            TextScroll comment2Scroll_;
            int commentMaxChars_ = 0;
    };

}

#endif
