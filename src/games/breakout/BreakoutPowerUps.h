#pragma once
#include "games/breakout/BreakoutCollision.h"
#include "games/breakout/BreakoutRules.h"
namespace games::breakout {
enum class PowerUpKind { WidePaddle, MultiBall, Shield };
struct PowerUpDrop { Point position; PowerUpKind kind; };
struct PowerUpState {
    float wideSeconds = 0;
    bool shield = false;
    float PaddleWidth() const { return rules::paddleWidth * (wideSeconds > 0 ? rules::widePaddleRatio : 1); }
    void Advance(float seconds);
    void Activate(PowerUpKind kind);
    bool ConsumeShield();
};
bool DropTouchesPaddle(Point drop, Bounds paddle);
} // namespace games::breakout
