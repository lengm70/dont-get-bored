#include "games/breakout/BreakoutGame.h"
#include <algorithm>
#include <cmath>
namespace games::breakout {
void BreakoutGame::MovePaddle(float seconds, float input, std::optional<float> target) {
    float desired = input * rules::paddleSpeed;
    if (input == 0 && target)
        desired = std::clamp((*target - (paddleX_ + powers_.PaddleWidth() / 2)) * rules::mouseTracking,
                             -rules::mousePaddleSpeed, rules::mousePaddleSpeed);
    const float acceleration = (desired == 0 || desired * paddleVelocity_ < 0) ? rules::paddleDeceleration : rules::paddleAcceleration;
    paddleVelocity_ += std::clamp(desired - paddleVelocity_, -acceleration * seconds, acceleration * seconds);
    const float previous = paddleX_;
    paddleX_ = std::clamp(paddleX_ + paddleVelocity_ * seconds, 0.0f, rules::boardWidth - powers_.PaddleWidth());
    paddleVelocity_ = (paddleX_ - previous) / seconds;
}
void BreakoutGame::MoveBall(BallState& ball, float seconds, float paddleVelocity) {
    auto& pos = ball.position; auto& velocity = ball.velocity;
    pos.x += velocity.x * seconds; pos.y += velocity.y * seconds;
    bool wall = false;
    if (pos.x < rules::ballRadius) { pos.x = rules::ballRadius; velocity.x = std::abs(velocity.x); wall = true; }
    else if (pos.x > rules::boardWidth - rules::ballRadius) {
        pos.x = rules::boardWidth - rules::ballRadius; velocity.x = -std::abs(velocity.x); wall = true;
    }
    if (pos.y < rules::ballRadius) { pos.y = rules::ballRadius; velocity.y = std::abs(velocity.y); wall = true; }
    if (wall) AddImpact(pos, ImpactKind::Wall);
    const float speed = rules::ballSpeed + level_ * rules::levelSpeedIncrease;
    if (BounceOffPaddle(pos, velocity, Paddle(), paddleVelocity, speed)) AddImpact(pos, ImpactKind::Paddle);
    for (int index = 0; index < rules::brickCount; ++index) {
        if (bricks_[index].Alive() && BounceOffRectangle(pos, velocity, CurrentBrick(index),
                                                       BrickVelocity(bricks_, index, seconds_))) {
            if (bricks_[index].Destructible()) HitBrick(index);
            else AddImpact(pos, ImpactKind::Wall);
            break;
        }
    }
    if (velocity.y > 0 && pos.y + rules::ballRadius >= rules::shieldY && powers_.ConsumeShield()) {
        pos.y = rules::shieldY - rules::ballRadius - rules::separation;
        velocity.y = -std::abs(velocity.y);
        AddImpact(pos, ImpactKind::Shield);
    }
    EnsureBallAngle(velocity);
    const float actualSpeed = std::hypot(velocity.x, velocity.y);
    if (actualSpeed > 0) { velocity.x *= speed / actualSpeed; velocity.y *= speed / actualSpeed; }
    ball.trailSeconds += seconds;
    if (ball.trailSeconds >= rules::trailInterval) {
        ball.trailSeconds -= rules::trailInterval;
        for (int i = rules::trailLength - 1; i > 0; --i) ball.trail[i] = ball.trail[i - 1];
        ball.trail[0] = pos;
    }
}
void BreakoutGame::Step(float seconds, float input, std::optional<float> target) {
    for (auto& impact : impacts_) impact.age += seconds;
    impacts_.erase(std::remove_if(impacts_.begin(), impacts_.end(), [](const Impact& event) {
        return event.age >= rules::effectDuration;
    }), impacts_.end());
    if (status_ == Status::Playing) {
        const float previousWidth = powers_.PaddleWidth();
        powers_.Advance(seconds);
        paddleX_ = std::clamp(paddleX_ + (previousWidth - powers_.PaddleWidth()) / 2,
                             0.0f, rules::boardWidth - powers_.PaddleWidth());
    }
    MovePaddle(seconds, input, target);
    if (status_ == Status::Ready) {
        balls_.front().position.x = paddleX_ + powers_.PaddleWidth() / 2;
        balls_.front().trail.fill(balls_.front().position);
        return;
    }
    seconds_ += seconds;
    for (auto& ball : balls_) { MoveBall(ball, seconds, paddleVelocity_); lastBall_ = ball.position; }
    if (RemainingBricks(bricks_) == 0) { CompleteLevel(); return; }
    balls_.erase(std::remove_if(balls_.begin(), balls_.end(), [](const BallState& ball) {
        return ball.position.y - rules::ballRadius > rules::boardHeight;
    }), balls_.end());
    if (balls_.empty()) {
        --lives_; combo_ = 0;
        AddImpact({rules::boardWidth / 2, rules::paddleY}, ImpactKind::Lost);
        if (lives_ == 0) { status_ = Status::GameOver; drops_.clear(); powers_ = {}; }
        else PrepareServe();
        return;
    }
    UpdateDrops(seconds);
}
} // namespace games::breakout
