#pragma once

#include <array>
#include <cstdint>

#include "games/breakout/BreakoutCollision.h"
#include "games/breakout/BreakoutRules.h"

namespace games::breakout {

enum class Status { Ready, Playing, Paused, GameOver, Won };

class BreakoutGame {
public:
    BreakoutGame();

    void Reset();
    void Start();
    void TogglePause();
    void Update(float elapsedSeconds, float horizontalInput);

    static Bounds BrickBounds(int index);
    Bounds Paddle() const { return {paddleX_, rules::paddleY,
                                   rules::paddleWidth, rules::paddleHeight}; }
    Point Ball() const { return ball_; }
    Point Velocity() const { return velocity_; }
    const std::array<bool, rules::brickCount>& Bricks() const { return bricks_; }
    Status State() const { return status_; }
    int Score() const { return score_; }
    int Lives() const { return lives_; }

private:
    void PrepareServe();
    void Step(float seconds, float horizontalInput);
    bool BounceOffBrick(Bounds brick, Point previous);
    void DeflectFromPaddle();

    std::array<bool, rules::brickCount> bricks_{};
    Status status_ = Status::Ready;
    Point ball_{};
    Point velocity_{};
    float paddleX_ = 0.0f;
    std::uint32_t randomState_ = 0x9e3779b9u;
    int score_ = 0;
    int lives_ = rules::initialLives;
};

}  // namespace games::breakout
