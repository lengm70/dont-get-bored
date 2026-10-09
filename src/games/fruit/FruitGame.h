#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "games/fruit/FruitRules.h"
namespace games::fruit {
struct Point { float x = 0, y = 0; };
struct Fruit { int level = 0; Point position{}, velocity{}; float age = 0; float angle = 0, angularVelocity = 0, impact = 0; };
enum class Status { Ready, Playing, Paused, GameOver, Won };
class FruitGame {
public:
    explicit FruitGame(std::uint32_t seed = std::random_device{}());
    void Reset();
    void Start();
    void TogglePause();
    void SetAim(float x);
    bool Drop();
    void Update(float seconds);
    const std::vector<Fruit>& Fruits() const { return fruits_; }
    Status State() const { return state_; }
    int Score() const { return score_; }
    int CurrentLevel() const { return current_; }
    int NextLevel() const { return next_; }
    float Aim() const { return aim_; }
    float DangerProgress() const { return overflow_ / rules::overflowDelay; }
    bool CanDrop() const { return state_ == Status::Playing && cooldown_ <= 0; }
private:
    void Step();
    bool MergeOnePair();
    int RandomLevel();
    std::mt19937 random_;
    std::vector<Fruit> fruits_;
    Status state_ = Status::Ready;
    int score_ = 0, current_ = 0, next_ = 0;
    float aim_ = rules::width / 2, accumulator_ = 0, cooldown_ = 0, overflow_ = 0;
};
}  // namespace games::fruit
