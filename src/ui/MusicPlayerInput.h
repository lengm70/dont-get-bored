#pragma once

#include <algorithm>
#include "ui/UiLayout.h"

namespace ui {
class MusicPlayerInput {
public:
    Rectangle DesignBounds() const {
        return {position_.x, position_.y, config::musicPlayer.width, config::musicPlayer.height};
    }

    // Run before updating or drawing underlying controls, including on release frames.
    bool Update(const UiLayout& layout, Vector2 mouse, bool pressed, bool down) {
        const Rectangle panel = layout.Rect(DesignBounds());
        const bool captured = dragging_ || captured_ || CheckCollisionPointRec(mouse, panel);
        if (pressed && CheckCollisionPointRec(mouse, panel)) captured_ = true;
        if (pressed && CheckCollisionPointRec(mouse, {panel.x, panel.y, panel.width, 30 * layout.scale})) {
            dragging_ = true;
            dragOffset_ = {(mouse.x - panel.x) / layout.scale, (mouse.y - panel.y) / layout.scale};
        }
        if (dragging_ && down) {
            position_.x = std::clamp((mouse.x - layout.offsetX) / layout.scale - dragOffset_.x,
                                     0.0f, config::designWidth - config::musicPlayer.width);
            position_.y = std::clamp((mouse.y - layout.offsetY) / layout.scale - dragOffset_.y,
                                     0.0f, config::designHeight - config::musicPlayer.height);
        }
        if (!down) { dragging_ = false; captured_ = false; }
        return captured;
    }

private:
    Vector2 position_{config::musicPlayer.x, config::musicPlayer.y};
    Vector2 dragOffset_{};
    bool dragging_ = false;
    bool captured_ = false;
};
}  // namespace ui
