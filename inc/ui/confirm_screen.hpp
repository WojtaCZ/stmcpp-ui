#ifndef UI_CONFIRM_SCREEN_HPP
#define UI_CONFIRM_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"

namespace ui {

    class ConfirmScreen : public Screen {
        public:
            using ResultCallback = void (*)(bool confirmed, void* userData);

            ConfirmScreen(const char* prompt, ResultCallback onResult, void* userData = nullptr);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;

        private:
            const char* prompt_;
            ResultCallback onResult_;
            void* userData_;
            bool selectedYes_ = true;
    };

}

#endif
