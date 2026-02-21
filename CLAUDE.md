# CLAUDE.md — lib/ui

## Overview

Menu/UI framework for embedded OLED displays. Depends on `lib/gfx` for rendering. No heap allocation, no `std::string`/`std::vector`/`std::map`, no hardware dependencies.

Compiled as part of the parent `midicontrol-fw` CMake project (STM32G431, Cortex-M4, C++23, `-fno-exceptions`).

## Architecture

All code lives in namespace `ui::`. PixelType is fixed to `uint8_t`.

### Headers (`inc/ui/`)

- **`input.hpp`** — `InputEvent` enum: Up, Down, Enter, Back, Digit0-9.
- **`theme.hpp`** — `Theme` struct (display dimensions, font references, layout constants, `IconRef` members for all UI icons, scroll timing). `IconRef` wraps `gfx::icon<W,H>` through type erasure (function pointer + void*). `makeIconRef()` helper.
- **`screen.hpp`** — Abstract `Screen` base class: `handleInput()`, `render()`, `onEnter()`/`onExit()`, `isDirty()`/`markDirty()`, `tickScroll()`.
- **`item.hpp`** — `MenuItem` class (Action, Submenu, Back, Checkbox types). Uses `const char*` and function pointer + `void*` callbacks. Factory statics: `MenuItem::back()`, `MenuItem::checkbox()`.
- **`menu_screen.hpp`** — Scrollable item list with title bar and scroll arrows. Text scroll state machine for long item titles.
- **`splash_screen.hpp`** — Title/subtitle/comment full-screen display with optional dismiss callback.
- **`paragraph_screen.hpp`** — Multi-line `const char*` text display with dismiss callback.
- **`number_screen.hpp`** — Numeric value entry. Supports rotary (Up/Down ±step) and direct digit input (Digit0-9). Enter confirms, Back cancels.
- **`confirm_screen.hpp`** — Yes/No dialog. Up/Down toggles, Enter confirms.
- **`navigator.hpp`** — Fixed-size screen stack (depth 8). `push()`/`pop()`/`reset()`, `handleInput()`, `render()`, `tickScroll()`. Auto-pops on unhandled Back.

### Sources (`src/`)

Implementation files for each screen type and navigator.

## Integration pattern

The parent project:
1. Defines a `ui::Theme` with font/icon references
2. Creates static `MenuItem` arrays and screen instances
3. Creates a `ui::Navigator` and calls `push()` for the root screen
4. Maps hardware input to `navigator.handleInput(ui::InputEvent::...)`
5. Calls `navigator.render(frameBuffer)` periodically
6. Calls `navigator.tickScroll(elapsedMs)` for scroll animation
