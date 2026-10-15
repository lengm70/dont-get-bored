#pragma once
#include <array>
#include <algorithm>
#include <vector>
#include <optional>
#include "games/breakout/BreakoutLevels.h"
#include "games/breakout/BreakoutPowerUps.h"
namespace games::breakout {
enum class Status { Ready, Playing, Paused, GameOver, LevelComplete, Won };
enum class ImpactKind { Wall, Paddle, Brick, Explosion, Pickup, Shield, Lost };
struct Impact { Point position; ImpactKind kind; float age; int serial; };
struct BallState {
    Point position{}, velocity{};
    std::array<Point, rules::trailLength> trail{};
    float trailSeconds = 0;
};
class BreakoutGame {
public:
    BreakoutGame();
    void Reset(int level = 0);
    void Start();
    void TogglePause();
    void Update(float elapsedSeconds, float horizontalInput,
                std::optional<float> mouseTarget = std::nullopt);
    void ActivatePowerUp(PowerUpKind kind);
    void ClearMouseTarget() { mouseTarget_.reset(); }
    static Bounds BrickBounds(int index) { return BaseBrickBounds(index); }
    Bounds CurrentBrick(int index) const { return CurrentBrickBounds(bricks_, index, seconds_); }
    Bounds Paddle() const { return {paddleX_, rules::paddleY, powers_.PaddleWidth(), rules::paddleHeight}; }
    Point Ball() const { return balls_.empty() ? lastBall_ : balls_.front().position; }
    Point Velocity() const { return balls_.empty() ? Point{} : balls_.front().velocity; }
    const std::vector<BallState>& Balls() const { return balls_; }
    const std::array<bool, rules::brickCount>& Bricks() const { return alive_; }
    const BrickArray& BrickStates() const { return bricks_; }
    const std::vector<PowerUpDrop>& Drops() const { return drops_; }
    const std::vector<Impact>& Impacts() const { return impacts_; }
    const PowerUpState& Powers() const { return powers_; }
    Status State() const { return status_; }
    int Score() const { return score_; }
    int Lives() const { return lives_; }
    int Level() const { return level_; }
    int LevelScore() const { return score_ - levelStartScore_; }
    int Combo() const { return combo_; }
    int Multiplier() const { return std::min(rules::maximumMultiplier, 1 + combo_ / rules::comboStep); }
    int MaxCombo() const { return maxCombo_; }
    int LevelMaxCombo() const { return levelMaxCombo_; }
    int CompletionBonus() const { return completionBonus_; }
    bool CanSelectLevel() const { return status_ == Status::Ready && score_ == 0 && lives_ == rules::initialLives; }
private:
    void LoadLevel();
    void PrepareServe();
    void Step(float seconds, float horizontalInput, std::optional<float> mouseTarget);
    void MovePaddle(float seconds, float horizontalInput, std::optional<float> mouseTarget);
    void MoveBall(BallState& ball, float seconds, float paddleVelocity);
    void HitBrick(int index);
    void UpdateDrops(float seconds);
    void AddImpact(Point position, ImpactKind kind);
    void CompleteLevel();
    BrickArray bricks_{};
    std::array<bool, rules::brickCount> alive_{};
    std::vector<BallState> balls_;
    std::vector<PowerUpDrop> drops_;
    std::vector<Impact> impacts_;
    PowerUpState powers_;
    Status status_ = Status::Ready;
    Point lastBall_{};
    std::optional<float> mouseTarget_;
    float paddleX_ = 0, paddleVelocity_ = 0, seconds_ = 0;
    int level_ = 0, score_ = 0, lives_ = rules::initialLives;
    int combo_ = 0, maxCombo_ = 0, levelMaxCombo_ = 0;
    int levelStartScore_ = 0, completionBonus_ = 0, destroyed_ = 0, impactSerial_ = 0;
};
} // namespace games::breakout
