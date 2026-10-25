#ifndef UI_PARAGRAPH_SCREEN_HPP
#define UI_PARAGRAPH_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"
#include "text_scroll.hpp"

namespace ui {

    class ParagraphScreen : public Screen {
        public:
            using DismissCallback = void (*)(void* userData);

            ParagraphScreen(const char* const* lines, int lineCount,
                            DismissCallback onDismiss = nullptr, void* userData = nullptr);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;
            void onEnter() override;
            // Optional title bar (stable pointer); without it the lines start at the top
            void setTitle(const char* title) { title_ = title; markDirty(); }
            // Lines longer than the display scroll (all together, each stops at its own end)
            void tickScroll(int elapsedMs) override;

        private:
            const char* const* lines_;
            int lineCount_;
            DismissCallback onDismiss_;
            void* userData_;
            const char* title_ = nullptr;

            TextScroll scroll_;
            int maxChars_ = 0; // characters per line, known after the first render
    };

}

#endif
