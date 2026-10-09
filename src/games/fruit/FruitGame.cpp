#include "games/fruit/FruitGame.h"
#include "games/fruit/FruitPhysics.h"
#include <algorithm>
#include <cmath>
namespace games::fruit {
FruitGame::FruitGame(std::uint32_t seed) : random_(seed) { Reset(); }
int FruitGame::RandomLevel() { return std::uniform_int_distribution<int>(0, rules::spawnLevels - 1)(random_); }
void FruitGame::Reset() {
    fruits_.clear(); state_ = Status::Ready; score_ = 0;
    current_ = RandomLevel(); next_ = RandomLevel(); aim_ = rules::width / 2;
    accumulator_ = cooldown_ = overflow_ = 0;
}
void FruitGame::Start() { if (state_ == Status::Ready) state_ = Status::Playing; }
void FruitGame::TogglePause() {
    if (state_ == Status::Playing) state_ = Status::Paused;
    else if (state_ == Status::Paused) state_ = Status::Playing;
}
void FruitGame::SetAim(float x) {
    if (!std::isfinite(x)) return;
    const float radius = rules::radii[current_];
    aim_ = std::clamp(x, radius, rules::width - radius);
}
bool FruitGame::Drop() {
    if (!CanDrop()) return false;
    if (fruits_.size() >= rules::maxFruits) { state_ = Status::GameOver; return false; }
    const Point position{aim_, rules::spawnY};
    const float radius = rules::radii[current_];
    // A blocked spawn cannot insert an overlapping fruit into the pile.
    for (const auto& fruit : fruits_) {
        const float dx = fruit.position.x - position.x, dy = fruit.position.y - position.y;
        const float sum = radius + rules::radii[fruit.level];
        if (dx * dx + dy * dy < sum * sum) return false;
    }
    Fruit dropped{current_, position, {}, 0};
    dropped.angularVelocity = std::uniform_real_distribution<float>(-0.8f, 0.8f)(random_);
    fruits_.push_back(dropped);
    current_ = next_; next_ = RandomLevel(); SetAim(aim_);
    cooldown_ = rules::dropDelay; return true;
}
bool FruitGame::MergeOnePair() {
    for (std::size_t a = 0; a < fruits_.size(); ++a) for (std::size_t b = a + 1; b < fruits_.size(); ++b) {
        const auto first = fruits_[a], second = fruits_[b];
        if (first.level != second.level || first.level >= rules::lastLevel) continue;
        const float dx = first.position.x - second.position.x, dy = first.position.y - second.position.y;
        const float touching = 2 * rules::radii[first.level] + 0.1f;
        if (dx * dx + dy * dy > touching * touching) continue;
        const int level = first.level + 1;
        Fruit merged = physics::Combine(first, second);
        // Erase high index first; never retain references across vector changes.
        fruits_.erase(fruits_.begin() + b); fruits_.erase(fruits_.begin() + a);
        fruits_.push_back(merged); score_ += (level + 1) * 10;
        if (level == rules::lastLevel) state_ = Status::Won;
        return true;
    }
    return false;
}
void FruitGame::Step() {
    cooldown_ = std::max(0.0f, cooldown_ - rules::timeStep);
    for (auto& fruit : fruits_) {
        fruit.age += rules::timeStep;
        fruit.angle = std::remainder(fruit.angle + fruit.angularVelocity * rules::timeStep, 6.2831853f);
        fruit.angularVelocity *= rules::angularDamping;
        fruit.impact = std::max(0.0f, fruit.impact - rules::impactDecay * rules::timeStep);
        fruit.velocity.y += rules::gravity * rules::timeStep;
        fruit.velocity.x *= rules::damping;
        fruit.position.x += fruit.velocity.x * rules::timeStep;
        fruit.position.y += fruit.velocity.y * rules::timeStep;
        physics::Constrain(fruit);
    }
    for (int iteration = 0; iteration < rules::solverIterations; ++iteration) {
        while (state_ == Status::Playing && MergeOnePair()) {}
        for (std::size_t a = 0; a < fruits_.size(); ++a) for (std::size_t b = a + 1; b < fruits_.size(); ++b)
            physics::Resolve(fruits_[a], fruits_[b]);
        for (auto& fruit : fruits_) physics::Constrain(fruit, false);
    }
    const bool overflow = std::any_of(fruits_.begin(), fruits_.end(), [](const Fruit& fruit) {
        return fruit.age >= rules::spawnGrace && fruit.position.y - rules::radii[fruit.level] < rules::dangerY;
    });
    overflow_ = overflow ? overflow_ + rules::timeStep : 0;
    if (overflow_ >= rules::overflowDelay && state_ == Status::Playing) state_ = Status::GameOver;
}
void FruitGame::Update(float seconds) {
    if (state_ != Status::Playing || !std::isfinite(seconds) || seconds <= 0) return;
    accumulator_ += std::min(seconds, rules::maxFrameTime);
    while (accumulator_ >= rules::timeStep && state_ == Status::Playing) {
        accumulator_ -= rules::timeStep; Step();
    }
}
}  // namespace games::fruit
