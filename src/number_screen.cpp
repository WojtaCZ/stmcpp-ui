#include <ui/number_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>
#include <cstdio>
#include <cstring>

namespace ui {

    NumberScreen::NumberScreen(const char* prompt, int32_t initialValue,
                               int32_t minValue, int32_t maxValue, int32_t step,
                               ResultCallback onResult, void* userData)
        : prompt_(prompt), value_(initialValue), minValue_(minValue),
          maxValue_(maxValue), step_(step), onResult_(onResult), userData_(userData) {}

    void NumberScreen::clamp() {
        if (value_ < minValue_) value_ = minValue_;
        if (value_ > maxValue_) value_ = maxValue_;
    }

    bool NumberScreen::handleInput(InputEvent event) {
        switch (event) {
            case InputEvent::Up:
                if (digitEntry_) {
                    digitEntry_ = false;
                    value_ = digitAccum_;
                    clamp();
                }
                value_ += step_;
                clamp();
                markDirty();
                return true;

            case InputEvent::Down:
                if (digitEntry_) {
                    digitEntry_ = false;
                    value_ = digitAccum_;
                    clamp();
                }
                value_ -= step_;
                clamp();
                markDirty();
                return true;

            case InputEvent::Enter:
                if (digitEntry_) {
                    value_ = digitAccum_;
                    clamp();
                    digitEntry_ = false;
                }
                if (onResult_)
                    onResult_(value_, true, userData_);
                return true;

            case InputEvent::Back:
                if (digitEntry_) {
                    // Delete last digit
                    digitAccum_ /= 10;
                    if (digitAccum_ == 0) digitEntry_ = false;
                    markDirty();
                    return true;
                }
                if (onResult_)
                    onResult_(value_, false, userData_);
                return true;

            default:
                break;
        }

        // Handle digit input
        uint8_t ev = static_cast<uint8_t>(event);
        uint8_t d0 = static_cast<uint8_t>(InputEvent::Digit0);
        if (ev >= d0 && ev <= d0 + 9) {
            int digit = ev - d0;
            if (!digitEntry_) {
                digitEntry_ = true;
                digitAccum_ = digit;
            } else {
                int32_t next = digitAccum_ * 10 + digit;
                if (next <= maxValue_)
                    digitAccum_ = next;
            }
            markDirty();
            return true;
        }

        return false;
    }

    void NumberScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));

        // Draw prompt
        if (prompt_ && prompt_[0] != '\0' && theme.smallFont) {
            gfx::text::drawText(fb, *theme.smallFont, prompt_, 2, 0,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Draw value centered
        if (theme.itemFont) {
            char buf[16];
            int32_t displayVal = digitEntry_ ? digitAccum_ : value_;
            std::snprintf(buf, sizeof(buf), "%ld", static_cast<long>(displayVal));

            int fw = static_cast<int>(theme.itemFont->width());
            int textW = static_cast<int>(std::strlen(buf)) * fw;
            int x = (theme.displayWidth - textW) / 2;
            gfx::text::drawText(fb, *theme.itemFont, buf, x, 25,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Draw hint
        if (theme.smallFont) {
            const char* hint = digitEntry_ ? "[digits] Enter/Back" : "Up/Down Enter/Back";
            gfx::text::drawText(fb, *theme.smallFont, hint, 2, 53,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        clearDirty();
    }

}
