#include "games/breakout/BreakoutGame.h"

#include <algorithm>
#include <cmath>

namespace games::breakout {

BreakoutGame::BreakoutGame() { Reset(); }

void BreakoutGame::Reset() {
    bricks_.fill(true);
    score_ = 0;
    lives_ = rules::initialLives;
    paddleX_ = (rules::boardWidth - rules::paddleWidth) / 2.0f;
    PrepareServe();
}

void BreakoutGame::PrepareServe() {
    status_ = Status::Ready;
    ball_ = {paddleX_ + rules::paddleWidth / 2.0f,
             rules::paddleY - rules::ballRadius - rules::separation};
    velocity_ = {rules::ballSpeed * rules::serveHorizontalRatio,
                -rules::ballSpeed * std::sqrt(1.0f - rules::serveHorizontalRatio *
                                             rules::serveHorizontalRatio)};
}

void BreakoutGame::DeflectFromPaddle() {
    // A small random turn prevents the ball from repeating the same paddle path.
    randomState_ = randomState_ * 1664525u + 1013904223u;
    const float unit = static_cast<float>(randomState_ >> 8) / 16777216.0f;
    const float magnitude = rules::paddleDeflectionMin + unit *
        (rules::paddleDeflectionMax - rules::paddleDeflectionMin);
    const float angle = (randomState_ & 1u) == 0 ? magnitude : -magnitude;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    velocity_ = {velocity_.x * cosine - velocity_.y * sine,
                 velocity_.x * sine + velocity_.y * cosine};
}

void BreakoutGame::Start() {
    if (status_ == Status::Ready) status_ = Status::Playing;
}

void BreakoutGame::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}

Bounds BreakoutGame::BrickBounds(int index) {
    return {rules::brickMargin + (index % rules::brickColumns) *
                                    (rules::brickWidth + rules::brickGap),
            rules::brickTop + (index / rules::brickColumns) *
                                 (rules::brickHeight + rules::brickGap),
            rules::brickWidth, rules::brickHeight};
}

bool BreakoutGame::BounceOffBrick(Bounds brick, Point previous) {
    const float closestX = std::clamp(ball_.x, brick.x, brick.x + brick.width);
    const float closestY = std::clamp(ball_.y, brick.y, brick.y + brick.height);
    const float dx = ball_.x - closestX;
    const float dy = ball_.y - closestY;
    if (dx * dx + dy * dy > rules::ballRadius * rules::ballRadius) return false;

    // Separate along the incoming face to prevent a second bounce on the same brick.
    const float distance = rules::ballRadius + rules::separation;
    if (previous.y <= brick.y - rules::ballRadius) {
        ball_.y = brick.y - distance;
        velocity_.y = -std::abs(velocity_.y);
    } else if (previous.y >= brick.y + brick.height + rules::ballRadius) {
        ball_.y = brick.y + brick.height + distance;
        velocity_.y = std::abs(velocity_.y);
    } else if (previous.x <= brick.x - rules::ballRadius) {
        ball_.x = brick.x - distance;
        velocity_.x = -std::abs(velocity_.x);
    } else if (previous.x >= brick.x + brick.width + rules::ballRadius) {
        ball_.x = brick.x + brick.width + distance;
        velocity_.x = std::abs(velocity_.x);
    } else if (std::abs(dy) >= std::abs(dx)) {
        ball_.y = dy < 0.0f ? brick.y - distance : brick.y + brick.height + distance;
        velocity_.y = dy < 0.0f ? -std::abs(velocity_.y) : std::abs(velocity_.y);
    } else {
        ball_.x = dx < 0.0f ? brick.x - distance : brick.x + brick.width + distance;
        velocity_.x = dx < 0.0f ? -std::abs(velocity_.x) : std::abs(velocity_.x);
    }
    return true;
}

void BreakoutGame::Step(float seconds, float horizontalInput) {
    const float previousPaddleX = paddleX_;
    paddleX_ = std::clamp(paddleX_ + horizontalInput * rules::paddleSpeed * seconds,
                          0.0f, rules::boardWidth - rules::paddleWidth);
    if (status_ == Status::Ready) {
        ball_.x = paddleX_ + rules::paddleWidth / 2.0f;
        return;
    }
    const Point previous = ball_;
    ball_.x += velocity_.x * seconds;
    ball_.y += velocity_.y * seconds;
    if (ball_.x < rules::ballRadius) {
        ball_.x = rules::ballRadius;
        velocity_.x = std::abs(velocity_.x);
    } else if (ball_.x > rules::boardWidth - rules::ballRadius) {
        ball_.x = rules::boardWidth - rules::ballRadius;
        velocity_.x = -std::abs(velocity_.x);
    }
    if (ball_.y < rules::ballRadius) {
        ball_.y = rules::ballRadius;
        velocity_.y = std::abs(velocity_.y);
    }

    const float paddleVelocityX = (paddleX_ - previousPaddleX) / seconds;
    if (BounceOffPaddle(ball_, velocity_, Paddle(), paddleVelocityX)) {
        DeflectFromPaddle();
    }
    for (int index = 0; index < rules::brickCount; ++index) {
        if (bricks_[index] && BounceOffBrick(BrickBounds(index), previous)) {
            bricks_[index] = false;
            score_ += rules::pointsPerBrick;
            if (std::none_of(bricks_.begin(), bricks_.end(), [](bool brick) { return brick; })) {
                status_ = Status::Won;
            }
            break;
        }
    }
    if (ball_.y - rules::ballRadius > rules::boardHeight) {
        --lives_;
        if (lives_ == 0) status_ = Status::GameOver;
        else PrepareServe();
    }
}

void BreakoutGame::Update(float elapsedSeconds, float horizontalInput) {
    if ((status_ != Status::Ready && status_ != Status::Playing) ||
        !std::isfinite(elapsedSeconds) || !std::isfinite(horizontalInput)) return;
    float remaining = std::clamp(elapsedSeconds, 0.0f, rules::maxFrameSeconds);
    const float movement = std::clamp(horizontalInput, -1.0f, 1.0f);
    while (remaining > 0.0f) {
        const float seconds = std::min(remaining, rules::physicsStepSeconds);
        Step(seconds, movement);
        remaining -= seconds;
        if (status_ != Status::Ready && status_ != Status::Playing) break;
    }
}

}  // namespace games::breakout
