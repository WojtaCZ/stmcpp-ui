#ifndef UI_LIST_HPP
#define UI_LIST_HPP

#include "theme.hpp"
#include <cstdint>
#include <gfx/framebuffer.hpp>

namespace ui {

    // Shared drawing of the common screen parts, so every screen with a title, a list or a
    // key hint looks the same (MenuScreen, ParagraphScreen and the application screens).

    // Title in titleFont at leftMargin with a separator line at the bottom of the title bar
    void drawTitleBar(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const char* title);

    // y of the bottom line of the screen (key hints, splash comment), smallFont height from the bottom
    int hintY(const Theme& theme);

    // Key hint in smallFont at leftMargin on the bottom line
    void drawHint(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const char* hint);

    // One list row: its text and the icon in front of it
    struct ListRow {
        const char* text = "";
        IconRef icon;
    };

    struct ListView {
        // Returns row `index`; `selected` picks the selected icon variant
        using RowFn = ListRow (*)(int index, bool selected, const Theme& theme, void* ctx);

        int count = 0;          // rows in the list (on this page)
        int selected = 0;       // selected row
        int scrollOffset = 0;   // first character of the selected row's text (long text scrolls)
        bool moreAbove = false; // rows exist above the list (e.g. a previous page)
        bool moreBelow = false; // rows exist below the list (e.g. a next page)
        RowFn row = nullptr;
        void* ctx = nullptr;
    };

    // First visible row: the first row at the top, the last row at the bottom, otherwise the
    // selected row second (one row of context above it)
    int listTop(int selected, int count, int visible);

    // theme.visibleItems rows in itemFont from itemStartY with icons, the selected row's text
    // scrolled by scrollOffset, others cut at maxItemChars; scroll arrows at scrollArrowX
    // (up on the first row, down on the last) whenever rows are hidden in that direction
    void drawList(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme, const ListView& view);

}

#endif
