#pragma once
#include <algorithm>
#include <array>
#include "ui/UiLayout.h"
#include "ui/MusicPlayerConfig.h"
namespace ui {
enum class PlayerDock { None, Left, Right, Top, Bottom };
class MusicPlayerInput {
public:
    Rectangle DesignBounds() const { return {position_.x, position_.y, config::musicPlayer.width, config::musicPlayer.height}; }
    bool Collapsed() const { return dock_ != PlayerDock::None; }
    PlayerDock Dock() const { return dock_; }
    bool SuppressControls() const { return suppressControls_; }
    Rectangle VisibleBounds() const {
        if (!Collapsed()) return DesignBounds();
        const auto full = DesignBounds();
        if (dock_ == PlayerDock::Left || dock_ == PlayerDock::Right) {
            return {dock_ == PlayerDock::Left ? canvas_.x : canvas_.x + canvas_.width - playerConfig::tabThickness,
                std::clamp(full.y + full.height / 2 - playerConfig::tabLength / 2, canvas_.y,
                    canvas_.y + canvas_.height - playerConfig::tabLength), playerConfig::tabThickness, playerConfig::tabLength};
        }
        return {std::clamp(full.x + full.width / 2 - playerConfig::tabLength / 2, canvas_.x,
                    canvas_.x + canvas_.width - playerConfig::tabLength),
            dock_ == PlayerDock::Top ? canvas_.y : canvas_.y + canvas_.height - playerConfig::tabThickness,
            playerConfig::tabLength, playerConfig::tabThickness};
    }
    // Actual window edges include any margins around the design canvas.
    // Captured click/release frames never fall through to underlying games.
    bool Update(const UiLayout& layout, Vector2 mouse, bool pressed, bool down) {
        suppressControls_ = dragging_ || expanding_;
        canvas_ = {-layout.offsetX / layout.scale, -layout.offsetY / layout.scale,
            GetScreenWidth() / layout.scale, GetScreenHeight() / layout.scale};
        ClampPosition(); Anchor();
        const Rectangle visible = layout.Rect(VisibleBounds());
        const bool hovered = CheckCollisionPointRec(mouse, visible);
        const bool captured = dragging_ || captured_ || hovered;
        if (Collapsed()) {
            if (pressed && hovered) { dock_ = PlayerDock::None; captured_ = true; expanding_ = suppressControls_ = true; }
            if (!down) captured_ = false;
            return captured;
        }
        if (pressed && hovered) captured_ = true;
        if (pressed && CheckCollisionPointRec(mouse, {visible.x, visible.y, visible.width, playerConfig::headerHeight * layout.scale})) {
            dragging_ = true; suppressControls_ = true;
            dragOffset_ = {(mouse.x - visible.x) / layout.scale, (mouse.y - visible.y) / layout.scale};
        }
        if (dragging_ && down) {
            position_ = {(mouse.x - layout.offsetX) / layout.scale - dragOffset_.x,
                         (mouse.y - layout.offsetY) / layout.scale - dragOffset_.y};
            ClampPosition();
        }
        if (!down) {
            if (dragging_) Snap();
            dragging_ = false; captured_ = false; expanding_ = false;
        }
        return captured;
    }
private:
    void ClampPosition() {
        position_.x = std::clamp(position_.x, canvas_.x, canvas_.x + canvas_.width - config::musicPlayer.width);
        position_.y = std::clamp(position_.y, canvas_.y, canvas_.y + canvas_.height - config::musicPlayer.height);
    }
    void Anchor() {
        if (dock_ == PlayerDock::Left) position_.x = canvas_.x;
        if (dock_ == PlayerDock::Right) position_.x = canvas_.x + canvas_.width - config::musicPlayer.width;
        if (dock_ == PlayerDock::Top) position_.y = canvas_.y;
        if (dock_ == PlayerDock::Bottom) position_.y = canvas_.y + canvas_.height - config::musicPlayer.height;
    }
    void Snap() {
        const std::array<float, 4> distances{position_.x - canvas_.x,
            canvas_.x + canvas_.width - position_.x - config::musicPlayer.width,
            position_.y - canvas_.y, canvas_.y + canvas_.height - position_.y - config::musicPlayer.height};
        const auto nearest = std::min_element(distances.begin(), distances.end());
        if (*nearest <= playerConfig::snapDistance) {
            dock_ = static_cast<PlayerDock>(1 + std::distance(distances.begin(), nearest)); Anchor();
        }
    }
    Rectangle canvas_{0, 0, static_cast<float>(config::designWidth), static_cast<float>(config::designHeight)};
    Vector2 position_{config::musicPlayer.x, config::musicPlayer.y}, dragOffset_{};
    PlayerDock dock_ = PlayerDock::None;
    bool dragging_ = false, captured_ = false, expanding_ = false, suppressControls_ = false;
};
}  // namespace ui
