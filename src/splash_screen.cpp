#include <ui/splash_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    SplashScreen::SplashScreen(const char* title, const char* subtitle,
                               const char* comment, DismissCallback onDismiss,
                               void* userData)
        : title_(title), subtitle_(subtitle), comment_(comment),
          onDismiss_(onDismiss), userData_(userData) {}

    bool SplashScreen::handleInput(InputEvent event) {
        if (event == InputEvent::Enter || event == InputEvent::Back) {
            if (onDismiss_)
                onDismiss_(userData_);
            return true;
        }
        return false;
    }

    void SplashScreen::drawComment(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme,
                                   const char* text, const TextScroll& scroll, int y) {
        if (!text || text[0] == '\0') return;

        int fw = static_cast<int>(theme.smallFont->width());
        int maxChars = commentMaxChars_;
        int len = static_cast<int>(std::strlen(text));
        int offset = scroll.offset(len, maxChars);

        char buf[32];
        int copyLen = len - offset > maxChars ? maxChars : len - offset;
        std::memcpy(buf, text + offset, static_cast<std::size_t>(copyLen));
        buf[copyLen] = '\0';

        int x = (theme.displayWidth - copyLen * fw) / 2;
        gfx::text::drawText(fb, *theme.smallFont, buf, x, y,
                            gfx::Anchor::TopLeft, uint8_t(1));
    }

    void SplashScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));

        bool hasComment2 = comment2_ && comment2_[0] != '\0';
        int subtitleY = hasComment2 ? 20 : 25;

        // Title centered at top, using item font (11x18)
        if (title_ && title_[0] != '\0' && theme.itemFont) {
            int fw = static_cast<int>(theme.itemFont->width());
            int titleLen = static_cast<int>(std::strlen(title_));
            int maxChars = theme.displayWidth / fw;
            char buf[32];
            int copyLen = titleLen > maxChars ? maxChars : titleLen;
            if (copyLen > 31) copyLen = 31;
            std::memcpy(buf, title_, static_cast<std::size_t>(copyLen));
            buf[copyLen] = '\0';

            int textW = copyLen * fw;
            int x = (theme.displayWidth - textW) / 2;
            gfx::text::drawText(fb, *theme.itemFont, buf, x, 0,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Subtitle centered at y=25, with scrolling
        if (subtitle_ && subtitle_[0] != '\0' && theme.itemFont) {
            int fw = static_cast<int>(theme.itemFont->width());
            int maxChars = theme.displayWidth / fw;
            int subLen = static_cast<int>(std::strlen(subtitle_));

            int offset = scrollOffset_;
            if (offset > subLen) offset = 0;

            char buf[32];
            int copyLen = subLen - offset;
            if (copyLen > maxChars) copyLen = maxChars;
            if (copyLen > 31) copyLen = 31;
            if (copyLen > 0)
                std::memcpy(buf, subtitle_ + offset, static_cast<std::size_t>(copyLen));
            buf[copyLen] = '\0';

            int textW = copyLen * fw;
            int x = (theme.displayWidth - textW) / 2;
            gfx::text::drawText(fb, *theme.itemFont, buf, x, subtitleY,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Comment(s), centered when they fit, scrolled otherwise (small font 7x10)
        if (theme.smallFont) {
            int fw = static_cast<int>(theme.smallFont->width());
            int maxChars = theme.displayWidth / fw;
            if (maxChars > 31) maxChars = 31;
            commentMaxChars_ = maxChars;

            drawComment(fb, theme, comment_, commentScroll_, hasComment2 ? 42 : 53);
            if (hasComment2) drawComment(fb, theme, comment2_, comment2Scroll_, 53);
        }

        clearDirty();
    }

    void SplashScreen::tickScroll(int elapsedMs) {
        if (comment_ && commentScroll_.tick(elapsedMs, static_cast<int>(std::strlen(comment_)), commentMaxChars_))
            markDirty();
        if (comment2_ && comment2Scroll_.tick(elapsedMs, static_cast<int>(std::strlen(comment2_)), commentMaxChars_))
            markDirty();

        if (!subtitle_ || !subtitle_[0]) return;

        int subLen = static_cast<int>(std::strlen(subtitle_));
        int maxChars = 11; // approx display width / 11px font

        if (subLen <= maxChars) {
            scrollPaused_ = true;
            return;
        }

        if (scrollPaused_) {
            scrollTimerMs_ += elapsedMs;
            if (scrollTimerMs_ >= 1000) {
                scrollPaused_ = false;
                scrollTimerMs_ = 0;
                if (!hadScrollPause_) hadScrollPause_ = true;
            }
            return;
        }

        scrollTimerMs_ += elapsedMs;
        if (scrollTimerMs_ >= 500) {
            scrollTimerMs_ = 0;

            int maxOffset = subLen - maxChars;
            if (scrollOffset_ < maxOffset) {
                ++scrollOffset_;
                if (scrollOffset_ == maxOffset) {
                    scrollPaused_ = true;
                    scrollTimerMs_ = 0;
                }
            } else {
                scrollOffset_ = 0;
                scrollPaused_ = true;
                scrollTimerMs_ = 0;
                hadScrollPause_ = false;
            }
            markDirty();
        }
    }

}
