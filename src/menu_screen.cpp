#include <ui/menu_screen.hpp>
#include <ui/theme.hpp>
#include <gfx/text.hpp>
#include <gfx/icon.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    MenuScreen::MenuScreen(const char* title, MenuItem* items, int itemCount)
        : title_(title), items_(items), itemCount_(itemCount) {}

    void MenuScreen::onEnter() {
        selectedIndex_ = 0;
        scrollOffset_ = 0;
        scrollTimerMs_ = 0;
        scrollPaused_ = true;
        hadScrollPause_ = false;
        markDirty();
    }

    bool MenuScreen::handleInput(InputEvent event) {
        switch (event) {
            case InputEvent::Up:
                if (selectedIndex_ > 0) {
                    --selectedIndex_;
                    scrollOffset_ = 0;
                    scrollTimerMs_ = 0;
                    scrollPaused_ = true;
                    hadScrollPause_ = false;
                    markDirty();
                }
                return true;

            case InputEvent::Down:
                if (selectedIndex_ < itemCount_ - 1) {
                    ++selectedIndex_;
                    scrollOffset_ = 0;
                    scrollTimerMs_ = 0;
                    scrollPaused_ = true;
                    hadScrollPause_ = false;
                    markDirty();
                }
                return true;

            case InputEvent::Enter: {
                MenuItem* sel = &items_[selectedIndex_];
                if (sel->type() == MenuItem::Type::Checkbox) {
                    sel->setChecked(!sel->isChecked());
                    sel->invoke();
                    markDirty();
                } else {
                    sel->invoke();
                }
                return true;
            }

            case InputEvent::Back:
                return false;

            default:
                return false;
        }
    }

    void MenuScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));

        // Draw title bar
        if (title_ && title_[0] != '\0' && theme.titleFont) {
            gfx::text::drawText(fb, *theme.titleFont, title_, 0, 0,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }

        if (itemCount_ == 0) return;

        int visible = theme.visibleItems;
        if (visible > itemCount_) visible = itemCount_;

        // Calculate index shift for display
        int indexShift = 0;
        if (selectedIndex_ == 0) {
            indexShift = 0;
        } else if (selectedIndex_ >= itemCount_ - 1) {
            indexShift = visible - 1;
            if (indexShift >= itemCount_) indexShift = itemCount_ - 1;
        } else {
            indexShift = 1;
            if (indexShift >= visible) indexShift = visible - 1;
        }

        // Draw scroll arrows
        if (itemCount_ > visible) {
            int arrowX = theme.scrollArrowX;
            int itemH = static_cast<int>(theme.itemFont ? theme.itemFont->height() : 18);

            if (selectedIndex_ > 0) {
                theme.arrowUp.render(fb, arrowX, theme.itemStartY,
                                    gfx::Anchor::TopLeft, uint8_t(1));
            }
            if (selectedIndex_ < itemCount_ - 1) {
                theme.arrowDown.render(fb, arrowX,
                    theme.itemStartY + itemH + theme.itemSpacing,
                    gfx::Anchor::TopLeft, uint8_t(1));
            }
        }

        // Draw visible items
        int itemFontW = theme.itemFont ? static_cast<int>(theme.itemFont->width()) : 11;
        int itemFontH = theme.itemFont ? static_cast<int>(theme.itemFont->height()) : 18;

        for (int i = 0; i < visible; ++i) {
            int itemIdx = selectedIndex_ - indexShift + i;
            if (itemIdx < 0 || itemIdx >= itemCount_) continue;

            bool isSelected = (i == indexShift);
            MenuItem& item = items_[itemIdx];
            int yPos = theme.itemStartY + i * (itemFontH + theme.itemSpacing);

            // Draw icon
            IconRef icon = item.getIcon(theme, isSelected);
            int textX = theme.leftMargin;
            if (icon) {
                icon.render(fb, theme.leftMargin, yPos + (itemFontH - icon.h) / 2,
                            gfx::Anchor::TopLeft, uint8_t(1));
                textX = theme.leftMargin + icon.w + theme.iconTextGap;
            }

            // Draw item text (truncated/scrolled)
            const char* itemTitle = item.title();
            int titleLen = static_cast<int>(std::strlen(itemTitle));
            int maxChars = theme.maxItemChars;

            int offset = (isSelected) ? scrollOffset_ : 0;
            if (offset > titleLen) offset = 0;

            // Build truncated string in local buffer
            char buf[32];
            int copyLen = titleLen - offset;
            if (copyLen > maxChars) copyLen = maxChars;
            if (copyLen > 31) copyLen = 31;
            if (copyLen > 0) {
                std::memcpy(buf, itemTitle + offset, static_cast<std::size_t>(copyLen));
            }
            buf[copyLen] = '\0';

            if (theme.itemFont) {
                gfx::text::drawText(fb, *theme.itemFont, buf, textX, yPos,
                                    gfx::Anchor::TopLeft, uint8_t(1));
            }
        }

        clearDirty();
    }

    void MenuScreen::tickScroll(int elapsedMs) {
        if (itemCount_ == 0) return;

        const char* selTitle = items_[selectedIndex_].title();
        int titleLen = static_cast<int>(std::strlen(selTitle));
        int maxChars = 9; // default, could come from theme

        if (titleLen <= maxChars) {
            scrollPaused_ = true;
            return;
        }

        if (scrollPaused_) {
            scrollTimerMs_ += elapsedMs;
            if (scrollTimerMs_ >= 1000) { // pause duration
                scrollPaused_ = false;
                scrollTimerMs_ = 0;
                if (!hadScrollPause_) hadScrollPause_ = true;
            }
            return;
        }

        scrollTimerMs_ += elapsedMs;
        if (scrollTimerMs_ >= 500) { // step duration
            scrollTimerMs_ = 0;

            int maxOffset = titleLen - maxChars;
            if (scrollOffset_ < maxOffset) {
                ++scrollOffset_;
                if (scrollOffset_ == maxOffset) {
                    // Pause at end
                    scrollPaused_ = true;
                    scrollTimerMs_ = 0;
                }
            } else {
                // Reset to beginning
                scrollOffset_ = 0;
                scrollPaused_ = true;
                scrollTimerMs_ = 0;
                hadScrollPause_ = false;
            }
            markDirty();
        }
    }

}
