#include <ui/confirm_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>

namespace ui {

    ConfirmScreen::ConfirmScreen(const char* prompt, ResultCallback onResult, void* userData)
        : prompt_(prompt), onResult_(onResult), userData_(userData) {}

    bool ConfirmScreen::handleInput(InputEvent event) {
        switch (event) {
            case InputEvent::Up:
            case InputEvent::Down:
                selectedYes_ = !selectedYes_;
                markDirty();
                return true;

            case InputEvent::Enter:
                if (onResult_)
                    onResult_(selectedYes_, userData_);
                return true;

            case InputEvent::Back:
                if (onResult_)
                    onResult_(false, userData_);
                return true;

            default:
                return false;
        }
    }

    void ConfirmScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));

        // Draw prompt
        if (prompt_ && prompt_[0] != '\0' && theme.smallFont) {
            gfx::text::drawText(fb, *theme.smallFont, prompt_, 2, 2,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        if (theme.itemFont) {
            int fw = static_cast<int>(theme.itemFont->width());
            int fh = static_cast<int>(theme.itemFont->height());
            int centerX = theme.displayWidth / 2;
            int yBase = 30;

            // "Yes" option
            const char* yesText = selectedYes_ ? "> Yes" : "  Yes";
            int yesW = 5 * fw;
            gfx::text::drawText(fb, *theme.itemFont, yesText,
                                centerX - yesW / 2, yBase,
                                gfx::Anchor::TopLeft, uint8_t(1));

            // "No" option
            const char* noText = selectedYes_ ? "  No" : "> No";
            int noW = 4 * fw;
            gfx::text::drawText(fb, *theme.itemFont, noText,
                                centerX - noW / 2, yBase + fh + 2,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        clearDirty();
    }

}
