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
- **`list.hpp`** — shared look of every screen: `drawTitleBar()` (titleFont at `leftMargin` + separator line at `titleBarHeight - 1`), `drawHint()`/`hintY()` (smallFont on the bottom line, also the splash comment row), `drawList()` + `listTop()` (`visibleItems` rows in itemFont with icons, the selected row scrolled, arrows at `scrollArrowX` only when rows are hidden in that direction: up on the first row, down at the bottom of the last). Application screens with their own lists (the remote's song list) use these so all lists look the same.
- **`menu_screen.hpp`** — Item list (`drawList`) with title bar; long selected item scrolls (`TextScroll`, `theme.maxItemChars`). `setTitle()` changes the title (pointer compared).
- **`splash_screen.hpp`** — Title/subtitle/comment full-screen display with optional dismiss callback; subtitle and comments scroll (`TextScroll`), the comment sits on the hint line.
- **`paragraph_screen.hpp`** — Multi-line `const char*` text display with dismiss callback; `setTitle()` adds a title bar (5 lines still fit under it).
- **`number_screen.hpp`** — Numeric value entry. Supports rotary (Up/Down ±step) and direct digit input (Digit0-9). Enter confirms, Back cancels.
- **`confirm_screen.hpp`** — Yes/No dialog. Up/Down toggles, Enter confirms.
- **`progress.hpp`** — `drawProgress()`: title, bordered bar with fill and hint on a cleared framebuffer; exports `kProgressBarY`/`kProgressBarH` (the bootloaders derive their bar-only redraw rows from them). Depends on `lib/gfx` only because the bootloaders link it.
- **`progress_screen.hpp`** — `ProgressScreen`: progress bar screen (shutdown, firmware update), ignores input. `setPercent()`, `setHint()`; `setHint()` compares **pointers**, so hints must be string literals (or otherwise stable pointers).
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
