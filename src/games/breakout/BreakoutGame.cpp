#include "games/breakout/BreakoutGame.h"
#include <algorithm>
#include <cmath>
namespace games::breakout {
BreakoutGame::BreakoutGame() { Reset(); }
void BreakoutGame::Reset(int level) {
    level_ = std::clamp(level, 0, rules::levelCount - 1);
    score_ = combo_ = maxCombo_ = destroyed_ = impactSerial_ = 0;
    lives_ = rules::initialLives;
    mouseTarget_.reset();
    LoadLevel();
}
void BreakoutGame::LoadLevel() {
    bricks_ = MakeLevel(level_);
    for (int i = 0; i < rules::brickCount; ++i) alive_[i] = bricks_[i].Alive();
    levelStartScore_ = score_;
    levelMaxCombo_ = completionBonus_ = combo_ = 0;
    seconds_ = 0;
    impacts_.clear();
    paddleX_ = (rules::boardWidth - rules::paddleWidth) / 2;
    PrepareServe();
}
void BreakoutGame::PrepareServe() {
    status_ = Status::Ready;
    powers_ = {};
    drops_.clear();
    paddleVelocity_ = 0;
    paddleX_ = std::clamp(paddleX_, 0.0f, rules::boardWidth - rules::paddleWidth);
    const float speed = rules::ballSpeed + level_ * rules::levelSpeedIncrease;
    BallState ball;
    ball.position = {paddleX_ + rules::paddleWidth / 2, rules::paddleY - rules::ballRadius - rules::separation};
    ball.velocity = {speed * rules::serveHorizontalRatio, -speed * std::sqrt(1 - rules::serveHorizontalRatio * rules::serveHorizontalRatio)};
    ball.trail.fill(ball.position);
    balls_.assign(1, ball);
    lastBall_ = ball.position;
}
void BreakoutGame::Start() {
    if (status_ == Status::LevelComplete) { ++level_; LoadLevel(); }
    if (status_ == Status::Ready) status_ = Status::Playing;
}
void BreakoutGame::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}
void BreakoutGame::AddImpact(Point position, ImpactKind kind) {
    if (impacts_.size() >= rules::maximumEffects) impacts_.erase(impacts_.begin());
    impacts_.push_back({position, kind, 0, ++impactSerial_});
}
void BreakoutGame::HitBrick(int index) {
    const auto result = DamageBricks(bricks_, index);
    for (int damaged : result.damaged) {
        const auto bounds = CurrentBrick(damaged);
        AddImpact({bounds.x + bounds.width / 2, bounds.y + bounds.height / 2},
                  bricks_[damaged].kind == BrickKind::Explosive && !bricks_[damaged].Alive() ? ImpactKind::Explosion : ImpactKind::Brick);
    }
    for (int destroyed : result.destroyed) {
        alive_[destroyed] = false;
        ++combo_;
        maxCombo_ = std::max(maxCombo_, combo_);
        levelMaxCombo_ = std::max(levelMaxCombo_, combo_);
        score_ += rules::pointsPerBrick * Multiplier();
        if (++destroyed_ % rules::bricksPerDrop == 0 && drops_.size() < rules::maximumDrops) {
            const auto bounds = CurrentBrick(destroyed);
            const auto kind = static_cast<PowerUpKind>((destroyed_ / rules::bricksPerDrop - 1) % 3);
            drops_.push_back({{bounds.x + bounds.width / 2, bounds.y + bounds.height / 2}, kind});
        }
    }
}
void BreakoutGame::CompleteLevel() {
    completionBonus_ = lives_ * rules::lifeBonus;
    score_ += completionBonus_;
    drops_.clear();
    status_ = level_ == rules::levelCount - 1 ? Status::Won : Status::LevelComplete;
}
void BreakoutGame::ActivatePowerUp(PowerUpKind kind) {
    if (status_ != Status::Playing) return;
    const float previousWidth = powers_.PaddleWidth();
    powers_.Activate(kind);
    paddleX_ += (previousWidth - powers_.PaddleWidth()) / 2;
    paddleX_ = std::clamp(paddleX_, 0.0f, rules::boardWidth - powers_.PaddleWidth());
    if (kind == PowerUpKind::MultiBall && !balls_.empty()) {
        const BallState source = balls_.front();
        while (balls_.size() < rules::maximumBalls) {
            const float angle = balls_.size() == 1 ? -rules::multiballAngle : rules::multiballAngle;
            BallState ball = source;
            ball.velocity = {source.velocity.x * std::cos(angle) - source.velocity.y * std::sin(angle),
                             source.velocity.x * std::sin(angle) + source.velocity.y * std::cos(angle)};
            EnsureBallAngle(ball.velocity);
            ball.trail.fill(ball.position);
            balls_.push_back(ball);
        }
    }
    AddImpact({paddleX_ + powers_.PaddleWidth() / 2, rules::paddleY}, ImpactKind::Pickup);
}
void BreakoutGame::UpdateDrops(float seconds) {
    // Collect first, activate after iteration: multiball can reallocate the ball vector.
    std::vector<PowerUpKind> collected;
    for (auto& drop : drops_) {
        drop.position.y += rules::dropSpeed * seconds;
        if (DropTouchesPaddle(drop.position, Paddle())) {
            collected.push_back(drop.kind); drop.position.y = rules::boardHeight + rules::dropRadius * 2;
        }
    }
    drops_.erase(std::remove_if(drops_.begin(), drops_.end(), [](const PowerUpDrop& drop) {
        return drop.position.y > rules::boardHeight + rules::dropRadius;
    }), drops_.end());
    for (auto kind : collected) ActivatePowerUp(kind);
}
void BreakoutGame::Update(float elapsed, float input, std::optional<float> mouseTarget) {
    if (!std::isfinite(elapsed) || !std::isfinite(input)) return;
    if (mouseTarget && !std::isfinite(*mouseTarget)) mouseTarget.reset();
    if (status_ != Status::Playing && status_ != Status::Ready) return;
    float remaining = std::clamp(elapsed, 0.0f, rules::maxFrameSeconds);
    input = std::clamp(input, -1.0f, 1.0f);
    if (mouseTarget) mouseTarget_ = mouseTarget;
    if (input != 0) mouseTarget_.reset();
    while (remaining > 0 && (status_ == Status::Playing || status_ == Status::Ready)) {
        const float step = std::min(remaining, rules::physicsStepSeconds);
        Step(step, input, mouseTarget_);
        remaining -= step;
    }
}
} // namespace games::breakout
