#ifndef UI_PROGRESS_SCREEN_HPP
#define UI_PROGRESS_SCREEN_HPP

#include "screen.hpp"
#include "input.hpp"
#include "theme.hpp"
#include "progress.hpp"

namespace ui {

    // Obrazovka s ukazatelem postupu (vypnuti, aktualizace). Ignoruje vstup.
    class ProgressScreen : public Screen {
        public:
            ProgressScreen(const char* title, const char* hint) : title_(title), hint_(hint) {}

            void setPercent(uint8_t percent) {
                if (percent > 100) percent = 100;
                if (percent != percent_) {
                    percent_ = percent;
                    markDirty();
                }
            }

            void setHint(const char* hint) {
                if (hint != hint_) {
                    hint_ = hint;
                    markDirty();
                }
            }

            // Klavesy behem postupu nic nedelaji
            bool handleInput(InputEvent event) override {
                (void)event;
                return true;
            }

            void render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) override {
                if (theme.smallFont)
                    drawProgress(fb, *theme.smallFont, theme.displayWidth, title_, hint_, percent_);
                clearDirty();
            }

        private:
            const char* title_;
            const char* hint_;
            uint8_t percent_ = 0;
    };

}

#endif
