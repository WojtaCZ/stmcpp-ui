#ifndef UI_PARAGRAPH_SCREEN_HPP
#define UI_PARAGRAPH_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"

namespace ui {

    class ParagraphScreen : public Screen {
        public:
            using DismissCallback = void (*)(void* userData);

            ParagraphScreen(const char* const* lines, int lineCount,
                            DismissCallback onDismiss = nullptr, void* userData = nullptr);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;

        private:
            const char* const* lines_;
            int lineCount_;
            DismissCallback onDismiss_;
            void* userData_;
    };

}

#endif
