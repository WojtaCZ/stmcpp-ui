#include <ui/confirm_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    ConfirmScreen::ConfirmScreen(const char* prompt, ResultCallback onResult, void* userData)
        : prompt_(prompt), onResult_(onResult), userData_(userData) {}

    bool ConfirmScreen::handleInput(InputEvent event) {
        switch (event) {
            case InputEvent::Up:
            case InputEvent::Down:
            case InputEvent::Left:
            case InputEvent::Right:
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

        if (!theme.smallFont) {
            clearDirty();
            return;
        }

        const auto& font = *theme.smallFont;
        int fw = static_cast<int>(font.width());
        int fh = static_cast<int>(font.height());
        int margin = 4;
        int maxLineWidth = theme.displayWidth - 2 * margin;

        // Word-wrap the prompt into lines
        const int maxLines = 4;
        char lines[maxLines][32];
        int lineCount = 0;

        if (prompt_ && prompt_[0] != '\0') {
            const char* src = prompt_;
            int srcLen = static_cast<int>(std::strlen(src));
            int maxCharsPerLine = maxLineWidth / fw;
            if (maxCharsPerLine > 31) maxCharsPerLine = 31;

            int pos = 0;
            while (pos < srcLen && lineCount < maxLines) {
                // Find how many chars fit on this line
                int lineEnd = pos + maxCharsPerLine;
                if (lineEnd >= srcLen) {
                    // Rest fits on one line
                    int len = srcLen - pos;
                    std::memcpy(lines[lineCount], src + pos, len);
                    lines[lineCount][len] = '\0';
                    lineCount++;
                    break;
                }

                // Find last space within the fitting range for word wrap
                int breakAt = lineEnd;
                for (int i = lineEnd; i > pos; --i) {
                    if (src[i] == ' ') {
                        breakAt = i;
                        break;
                    }
                }

                // If no space found, force break at maxCharsPerLine
                if (breakAt == lineEnd && src[lineEnd] != ' ') {
                    breakAt = lineEnd;
                }

                int len = breakAt - pos;
                std::memcpy(lines[lineCount], src + pos, len);
                lines[lineCount][len] = '\0';
                lineCount++;

                // Skip the space
                pos = breakAt;
                if (pos < srcLen && src[pos] == ' ') pos++;
            }
        }

        // Center the prompt text vertically in the upper portion
        int optionsY = theme.displayHeight - fh - 4;
        int textBlockHeight = lineCount * fh;
        int textStartY = (optionsY - textBlockHeight) / 2;
        if (textStartY < 0) textStartY = 0;

        for (int i = 0; i < lineCount; i++) {
            int textW = static_cast<int>(std::strlen(lines[i])) * fw;
            int textX = (theme.displayWidth - textW) / 2;
            gfx::text::drawText(fb, font, lines[i], textX, textStartY + i * fh,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Draw options at bottom, same Y
        const char* yesText = "Yes";
        const char* noText = "No";
        int yesW = 3 * fw;
        int noW = 2 * fw;
        int pad = 2;

        int yesX = theme.displayWidth / 3 - yesW / 2;
        int noX = 2 * theme.displayWidth / 3 - noW / 2;

        if (selectedYes_) {
            // Inverted Yes: white rect + black text
            gfx::drawFilledRect(fb, yesX - pad, optionsY - 1, yesW + 2 * pad, fh + 2, uint8_t(1));
            gfx::text::drawText(fb, font, yesText, yesX, optionsY,
                                gfx::Anchor::TopLeft, uint8_t(0));
            // Normal No
            gfx::text::drawText(fb, font, noText, noX, optionsY,
                                gfx::Anchor::TopLeft, uint8_t(1));
        } else {
            // Normal Yes
            gfx::text::drawText(fb, font, yesText, yesX, optionsY,
                                gfx::Anchor::TopLeft, uint8_t(1));
            // Inverted No: white rect + black text
            gfx::drawFilledRect(fb, noX - pad, optionsY - 1, noW + 2 * pad, fh + 2, uint8_t(1));
            gfx::text::drawText(fb, font, noText, noX, optionsY,
                                gfx::Anchor::TopLeft, uint8_t(0));
        }

        clearDirty();
    }

}
