#include "games/breakout/BreakoutPowerUps.h"
#include <algorithm>
namespace games::breakout {
void PowerUpState::Advance(float seconds) { wideSeconds = std::max(0.0f, wideSeconds - seconds); }
void PowerUpState::Activate(PowerUpKind kind) {
    if (kind == PowerUpKind::WidePaddle) wideSeconds = rules::wideDuration;
    if (kind == PowerUpKind::Shield) shield = true;
}
bool PowerUpState::ConsumeShield() {
    if (!shield) return false;
    shield = false;
    return true;
}
bool DropTouchesPaddle(Point drop, Bounds paddle) {
    const float dx = drop.x - std::clamp(drop.x, paddle.x, paddle.x + paddle.width);
    const float dy = drop.y - std::clamp(drop.y, paddle.y, paddle.y + paddle.height);
    return dx * dx + dy * dy <= rules::dropRadius * rules::dropRadius;
}
} // namespace games::breakout
