#include <ui/menu_screen.hpp>
#include <ui/list.hpp>
#include <ui/theme.hpp>
#include <gfx/shapes.hpp>
#include <cstring>

namespace ui {

    MenuScreen::MenuScreen(const char* title, MenuItem* items, int itemCount)
        : title_(title), items_(items), itemCount_(itemCount) {}

    void MenuScreen::onEnter() {
        selectedIndex_ = 0;
        scroll_.reset();
        markDirty();
    }

    bool MenuScreen::handleInput(InputEvent event) {
        switch (event) {
            case InputEvent::Up:
                if (selectedIndex_ > 0) {
                    --selectedIndex_;
                    scroll_.reset();
                    markDirty();
                }
                return true;

            case InputEvent::Down:
                if (selectedIndex_ < itemCount_ - 1) {
                    ++selectedIndex_;
                    scroll_.reset();
                    markDirty();
                }
                return true;

            case InputEvent::Enter: {
                MenuItem* sel = &items_[selectedIndex_];
                if (sel->type() == MenuItem::Type::Checkbox) {
                    sel->setChecked(!sel->isChecked());
                    sel->invoke();
                    markDirty();
                } else {
                    sel->invoke();
                }
                return true;
            }

            case InputEvent::Back:
                return false;

            default:
                return false;
        }
    }

    static ListRow menuRow(int index, bool selected, const Theme& theme, void* ctx) {
        MenuItem& item = static_cast<MenuItem*>(ctx)[index];
        return {item.title(), item.getIcon(theme, selected)};
    }

    void MenuScreen::render(gfx::IFrameBuffer<uint8_t>& fb, const Theme& theme) {
        gfx::fill(fb, uint8_t(0));
        drawTitleBar(fb, theme, title_);
        maxChars_ = theme.maxItemChars;

        if (itemCount_ > 0) {
            ListView view;
            view.count = itemCount_;
            view.selected = selectedIndex_;
            view.scrollOffset = scroll_.offset(static_cast<int>(std::strlen(items_[selectedIndex_].title())), maxChars_);
            view.row = menuRow;
            view.ctx = items_;
            drawList(fb, theme, view);
        }

        clearDirty();
    }

    void MenuScreen::tickScroll(int elapsedMs) {
        if (itemCount_ == 0) return;
        if (scroll_.tick(elapsedMs, static_cast<int>(std::strlen(items_[selectedIndex_].title())), maxChars_))
            markDirty();
    }

}
