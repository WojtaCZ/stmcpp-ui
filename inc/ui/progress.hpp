#ifndef UI_PROGRESS_HPP
#define UI_PROGRESS_HPP

#include <cstdint>
#include <gfx/framebuffer.hpp>
#include <gfx/text.hpp>

namespace ui {

    // Vykresli celou obrazovku s ukazatelem postupu: smaze fb, nahore vycentrovany nazev,
    // uprostred ramecek s vyplni, dole vycentrovana napoveda. percent se omezi na 100,
    // nullptr u title/hint znamena, ze se radek nekresli.
    // Zavisi jen na lib/gfx (linkuji ji i bootloadery).
    void drawProgress(gfx::IFrameBuffer<uint8_t>& fb, const gfx::text::font& font, int width,
                      const char* title, const char* hint, uint8_t percent);

}

#endif
