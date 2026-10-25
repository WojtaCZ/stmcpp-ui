#include <ui/list.hpp>
#include <gfx/text.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    void drawTitleBar(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const char* title) {
        if (title && title[0] != '\0' && theme.titleFont) {
            gfx::text::drawText(fb, *theme.titleFont, title, theme.leftMargin, 0,
                                gfx::Anchor::TopLeft, uint8_t(1));
        }
        gfx::drawHLine(fb, 0, theme.titleBarHeight - 1, theme.displayWidth, uint8_t(1));
    }

    int hintY(const Theme& theme) {
        int h = theme.smallFont ? static_cast<int>(theme.smallFont->height()) : 10;
        return theme.displayHeight - h;
    }

    void drawHint(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const char* hint) {
        if (!hint || hint[0] == '\0' || !theme.smallFont) return;
        gfx::text::drawText(fb, *theme.smallFont, hint, theme.leftMargin, hintY(theme),
                            gfx::Anchor::TopLeft, uint8_t(1));
    }

    int listTop(int selected, int count, int visible) {
        if (visible > count) visible = count;
        if (visible <= 0) return 0;
        int shift;
        if (selected <= 0) shift = 0;
        else if (selected >= count - 1) shift = visible - 1;
        else shift = visible > 1 ? 1 : 0;
        int top = selected - shift;
        return top < 0 ? 0 : top;
    }

    void drawList(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const ListView& view) {
        if (view.count <= 0 || !view.row || !theme.itemFont) return;

        int visible = theme.visibleItems;
        if (visible > view.count) visible = view.count;
        int top = listTop(view.selected, view.count, visible);

        const int fontW = static_cast<int>(theme.itemFont->width());
        const int fontH = static_cast<int>(theme.itemFont->height());
        const int rowPitch = fontH + theme.itemSpacing;

        for (int i = 0; i < visible; ++i) {
            int index = top + i;
            bool selected = index == view.selected;
            ListRow row = view.row(index, selected, theme, view.ctx);
            int y = theme.itemStartY + i * rowPitch;

            int textX = theme.leftMargin;
            if (row.icon) {
                row.icon.render(fb, theme.leftMargin, y + (fontH - row.icon.h) / 2,
                                gfx::Anchor::TopLeft, uint8_t(1));
                textX = theme.leftMargin + row.icon.w + theme.iconTextGap;
            }

            const char* text = row.text ? row.text : "";
            int len = static_cast<int>(std::strlen(text));
            int offset = selected ? view.scrollOffset : 0;
            if (offset > len) offset = 0;
            int maxChars = theme.maxItemChars;
            int maxFit = (theme.scrollArrowX - textX) / fontW;
            if (maxChars > maxFit) maxChars = maxFit;
            if (maxChars > 31) maxChars = 31;

            char buf[32];
            int copyLen = len - offset;
            if (copyLen > maxChars) copyLen = maxChars;
            if (copyLen < 0) copyLen = 0;
            std::memcpy(buf, text + offset, static_cast<std::size_t>(copyLen));
            buf[copyLen] = '\0';
            gfx::text::drawText(fb, *theme.itemFont, buf, textX, y, gfx::Anchor::TopLeft, uint8_t(1));
        }

        // Arrows: up at the top of the first row, down at the bottom of the last row
        bool above = top > 0 || view.moreAbove;
        bool below = top + visible < view.count || view.moreBelow;
        if (above && theme.arrowUp) {
            theme.arrowUp.render(fb, theme.scrollArrowX, theme.itemStartY,
                                 gfx::Anchor::TopLeft, uint8_t(1));
        }
        if (below && theme.arrowDown) {
            int bottom = theme.itemStartY + visible * rowPitch - theme.itemSpacing;
            theme.arrowDown.render(fb, theme.scrollArrowX, bottom - theme.arrowDown.h,
                                   gfx::Anchor::TopLeft, uint8_t(1));
        }
    }

}
