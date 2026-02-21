#include <ui/navigator.hpp>
#include <ui/theme.hpp>
#include <ui/menu_screen.hpp>

namespace ui {

    Navigator::Navigator(const Theme& theme) : theme_(&theme) {}

    void Navigator::push(Screen* screen) {
        if (depth_ >= MAX_DEPTH) return;
        stack_[depth_++] = screen;
        screen->onEnter();
        screen->markDirty();
    }

    void Navigator::pop() {
        if (depth_ <= 0) return;
        stack_[depth_ - 1]->onExit();
        --depth_;
        if (depth_ > 0)
            stack_[depth_ - 1]->markDirty();
    }

    void Navigator::reset(Screen* screen) {
        while (depth_ > 0) {
            stack_[depth_ - 1]->onExit();
            --depth_;
        }
        push(screen);
    }

    Screen* Navigator::current() const {
        if (depth_ <= 0) return nullptr;
        return stack_[depth_ - 1];
    }

    void Navigator::handleInput(InputEvent event) {
        Screen* cur = current();
        if (!cur) return;

        bool consumed = cur->handleInput(event);

        // Auto-handle submenu push on Enter for MenuScreen items
        if (event == InputEvent::Enter && consumed) {
            auto* menu = static_cast<MenuScreen*>(nullptr);
            // Check if the current screen is a MenuScreen by trying its selectedItem
            // We use a simple approach: if current screen has a selectedItem with submenu type
            // Dynamic cast alternative for -fno-rtti: we track screen type via a tag
            // For simplicity, we check the item type after the callback
            // The user's callback or item submenu handling is done by the parent project
        }

        // Auto-handle Back
        if (event == InputEvent::Back && !consumed) {
            if (depth_ > 1) {
                pop();
            }
            return;
        }

        // Auto-handle MenuScreen Enter → Submenu push
        if (event == InputEvent::Enter && consumed) {
            // Check if current is a MenuScreen with a submenu-type selected item
            // Since we don't have RTTI, the parent project should handle submenu push
            // via the item callback or by checking item type after handleInput
        }
    }

    bool Navigator::render(gfx::IFrameBuffer<uint8_t>& fb) {
        Screen* cur = current();
        if (!cur || !theme_) return false;

        if (!cur->isDirty()) return false;

        cur->render(fb, *theme_);
        return true;
    }

    void Navigator::tickScroll(int elapsedMs) {
        Screen* cur = current();
        if (cur) cur->tickScroll(elapsedMs);
    }

    void Navigator::forceRender() {
        Screen* cur = current();
        if (cur) cur->markDirty();
    }

}
