#include <ui/paragraph_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    ParagraphScreen::ParagraphScreen(const char* const* lines, int lineCount,
                                     DismissCallback onDismiss, void* userData)
        : lines_(lines), lineCount_(lineCount), onDismiss_(onDismiss), userData_(userData) {}

    bool ParagraphScreen::handleInput(InputEvent event) {
        if (event == InputEvent::Enter || event == InputEvent::Back) {
            if (onDismiss_)
                onDismiss_(userData_);
            return true;
        }
        return false;
    }

    void ParagraphScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));

        if (!theme.smallFont || !lines_) return;

        int fw = static_cast<int>(theme.smallFont->width());
        int fh = static_cast<int>(theme.smallFont->height());
        int maxChars = theme.displayWidth / fw;
        int lineSpacing = fh + 2;
        int yPos = 0;

        for (int i = 0; i < lineCount_; ++i) {
            if (!lines_[i]) continue;

            int len = static_cast<int>(std::strlen(lines_[i]));
            char buf[32];
            int copyLen = len > maxChars ? maxChars : len;
            if (copyLen > 31) copyLen = 31;
            std::memcpy(buf, lines_[i], static_cast<std::size_t>(copyLen));
            buf[copyLen] = '\0';

            gfx::text::drawText(fb, *theme.smallFont, buf, 2, yPos,
                                gfx::Anchor::TopLeft, uint8_t(1));
            yPos += lineSpacing;
        }

        clearDirty();
    }

}
