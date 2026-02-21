#ifndef UI_NUMBER_SCREEN_HPP
#define UI_NUMBER_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"
#include <cstdint>

namespace ui {

    class NumberScreen : public Screen {
        public:
            using ResultCallback = void (*)(int32_t value, bool confirmed, void* userData);

            NumberScreen(const char* prompt, int32_t initialValue,
                         int32_t minValue, int32_t maxValue, int32_t step,
                         ResultCallback onResult, void* userData = nullptr);

            bool handleInput(InputEvent event) override;
            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override;

            int32_t value() const { return value_; }

        private:
            const char* prompt_;
            int32_t value_;
            int32_t minValue_;
            int32_t maxValue_;
            int32_t step_;
            ResultCallback onResult_;
            void* userData_;
            bool digitEntry_ = false;
            int32_t digitAccum_ = 0;

            void clamp();
    };

}

#endif
