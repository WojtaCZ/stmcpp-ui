#include <ui/paragraph_screen.hpp>
#include <ui/list.hpp>
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
        int maxChars = (theme.displayWidth - theme.leftMargin) / fw;
        if (maxChars > 31) maxChars = 31;
        maxChars_ = maxChars;

        // With a title: lines packed under the title bar (5 lines fit); without: 2 px gaps
        bool titled = title_ && title_[0] != '\0';
        int lineSpacing = titled ? fh : fh + 2;
        int yPos = 0;
        if (titled) {
            drawTitleBar(fb, theme, title_);
            yPos = theme.titleBarHeight + 1;
        }

        for (int i = 0; i < lineCount_; ++i) {
            if (!lines_[i]) continue;

            int len = static_cast<int>(std::strlen(lines_[i]));
            int offset = scroll_.offset(len, maxChars);
            char buf[32];
            int copyLen = len - offset > maxChars ? maxChars : len - offset;
            std::memcpy(buf, lines_[i] + offset, static_cast<std::size_t>(copyLen));
            buf[copyLen] = '\0';

            gfx::text::drawText(fb, *theme.smallFont, buf, theme.leftMargin, yPos,
                                gfx::Anchor::TopLeft, uint8_t(1));
            yPos += lineSpacing;
        }

        clearDirty();
    }

    void ParagraphScreen::onEnter() {
        scroll_.reset();
    }

    void ParagraphScreen::tickScroll(int elapsedMs) {
        if (!lines_) return;

        // The longest line sets the cycle; shorter overlong lines stop at their end
        int longest = 0;
        for (int i = 0; i < lineCount_; ++i) {
            if (!lines_[i]) continue;
            int len = static_cast<int>(std::strlen(lines_[i]));
            if (len > longest) longest = len;
        }

        if (scroll_.tick(elapsedMs, longest, maxChars_))
            markDirty();
    }

}
