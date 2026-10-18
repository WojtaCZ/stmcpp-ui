#ifndef UI_TEXT_SCROLL_HPP
#define UI_TEXT_SCROLL_HPP

namespace ui {

    // Horizontal scroll state for one text that is longer than its window, with the same
    // timing as the menu items: pause at the start, one character per step, pause at the
    // end, then jump back to the start.
    class TextScroll {
        public:
            static constexpr int PAUSE_MS = 1000;
            static constexpr int STEP_MS  = 500;

            void reset() {
                offset_ = 0;
                timerMs_ = 0;
                paused_ = true;
            }

            // Advances the animation; returns true when the offset changed (redraw needed)
            bool tick(int elapsedMs, int textLen, int maxChars) {
                if (maxChars <= 0 || textLen <= maxChars) {
                    bool changed = offset_ != 0;
                    reset();
                    return changed;
                }

                timerMs_ += elapsedMs;
                if (paused_) {
                    if (timerMs_ < PAUSE_MS) return false;
                    paused_ = false;
                    timerMs_ = 0;
                    if (offset_ > 0) {
                        // End of the text reached: back to the start
                        offset_ = 0;
                        paused_ = true;
                        return true;
                    }
                    return false;
                }

                if (timerMs_ < STEP_MS) return false;
                timerMs_ = 0;

                int maxOffset = textLen - maxChars;
                if (offset_ < maxOffset) ++offset_;
                if (offset_ >= maxOffset) paused_ = true;
                return true;
            }

            // First character to draw for a text of textLen in a window of maxChars
            int offset(int textLen, int maxChars) const {
                int maxOffset = textLen - maxChars;
                if (maxOffset <= 0) return 0;
                return offset_ > maxOffset ? maxOffset : offset_;
            }

        private:
            int offset_ = 0;
            int timerMs_ = 0;
            bool paused_ = true;
    };

}

#endif
