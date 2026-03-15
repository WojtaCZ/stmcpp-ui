#ifndef UI_INPUT_HPP
#define UI_INPUT_HPP

#include <cstdint>

namespace ui {

    enum class InputEvent : uint8_t {
        Up,
        Down,
        Left,
        Right,
        Enter,
        Back,
        Digit0, Digit1, Digit2, Digit3, Digit4,
        Digit5, Digit6, Digit7, Digit8, Digit9,
    };

}

#endif
