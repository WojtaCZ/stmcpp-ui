#ifndef UI_THEME_HPP
#define UI_THEME_HPP

#include <cstdint>
#include <cstddef>
#include <gfx/framebuffer.hpp>
#include <gfx/icon.hpp>
#include <gfx/text.hpp>

namespace ui {

    struct IconRef {
        using DrawFn = void (*)(gfx::IFrameBuffer<uint8_t>& fb, const void* data,
                                int x, int y, gfx::Anchor anchor, uint8_t color);

        DrawFn draw = nullptr;
        const void* data = nullptr;
        int w = 0;
        int h = 0;

        void render(gfx::IFrameBuffer<uint8_t>& fb, int x, int y,
                    gfx::Anchor anchor, uint8_t color) const {
            if (draw && data)
                draw(fb, data, x, y, anchor, color);
        }

        explicit operator bool() const { return draw != nullptr; }
    };

    template <typename IconType>
    IconRef makeIconRef(const IconType& icon) {
        return IconRef{
            [](gfx::IFrameBuffer<uint8_t>& fb, const void* data,
               int x, int y, gfx::Anchor anchor, uint8_t color) {
                gfx::drawIcon(fb, *static_cast<const IconType*>(data), x, y, anchor, color);
            },
            &icon,
            static_cast<int>(IconType::width),
            static_cast<int>(IconType::height)
        };
    }

    struct Theme {
        int displayWidth = 130;
        int displayHeight = 64;

        const gfx::text::font* titleFont = nullptr;
        const gfx::text::font* itemFont = nullptr;
        const gfx::text::font* smallFont = nullptr;

        int titleBarHeight = 12;
        int itemStartY = 21;
        int itemSpacing = 3;
        int leftMargin = 0;
        int iconTextGap = 3;
        int visibleItems = 2;
        int scrollArrowX = 117;
        int maxItemChars = 9;

        IconRef dotSelected;
        IconRef dotUnselected;
        IconRef arrowUp;
        IconRef arrowDown;
        IconRef backSelected;
        IconRef backUnselected;
        IconRef checkboxSelEmpty;
        IconRef checkboxSelChecked;
        IconRef checkboxUnselEmpty;
        IconRef checkboxUnselChecked;

        int scrollPauseMs = 1000;
        int scrollStepMs = 500;
    };

}

#endif
