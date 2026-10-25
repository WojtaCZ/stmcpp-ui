#include <ui/progress.hpp>
#include <gfx/shapes.hpp>
#include <gfx/text.hpp>
#include <cstring>

namespace ui {

    static void drawCentred(gfx::IFrameBuffer<uint8_t>& fb, const gfx::text::font& font, int width,
                            const char* text, int y) {
        if (!text) return;
        int textWidth = static_cast<int>(std::strlen(text) * font.width());
        gfx::text::drawText(fb, font, text, (width - textWidth) / 2, y, gfx::Anchor::TopLeft, static_cast<uint8_t>(1));
    }

    void drawProgress(gfx::IFrameBuffer<uint8_t>& fb, const gfx::text::font& font, int width,
                      const char* title, const char* hint, uint8_t percent) {
        gfx::fill(fb, static_cast<uint8_t>(0));
        if (percent > 100) percent = 100;

        drawCentred(fb, font, width, title, 8);
        drawCentred(fb, font, width, hint, 48);

        // Ukazatel postupu
        const int x = 4, y = 26, w = width - 8, h = 12;
        gfx::drawRect(fb, x, y, w, h, static_cast<uint8_t>(1));
        int fill = (w - 4) * percent / 100;
        if (fill > 0) gfx::drawFilledRect(fb, x + 2, y + 2, fill, h - 4, static_cast<uint8_t>(1));
    }

}
