#pragma once
#include "games/tank/TankRules.h"
#include <array>
#include <vector>
namespace games::tank {
enum class Status { Ready, Playing, Paused, Won, GameOver };
struct Input {
    int direction = -1;
    bool fire = false;
};
struct Tank {
    float x = 0, y = 0;
    int direction = 0, health = 3;
    float cooldown = 0, think = 0, flash = 0;
};
struct Bullet {
    float x, y;
    int direction;
    bool enemy, alive = true;
};
struct Impact {
    float x, y, age = 0;
};
class TankGame {
  public:
    TankGame() {
        Reset();
    }
    void Reset(bool cooperative = false);
    void Start();
    void TogglePause();
    void Update(float seconds, std::array<Input, 2> input = {});
    Status State() const {
        return state_;
    }
    bool Cooperative() const {
        return coop_;
    }
    const auto &Players() const {
        return players_;
    }
    const auto &Enemies() const {
        return enemies_;
    }
    const auto &Bullets() const {
        return bullets_;
    }
    const auto &Walls() const {
        return walls_;
    }
    const auto &Impacts() const {
        return impacts_;
    }
    int Wave() const {
        return wave_;
    }
    int Score() const {
        return score_;
    }
    int BaseHealth() const {
        return baseHealth_;
    }
    int RemainingEnemies() const;
    bool Walkable(float x, float y) const;

  private:
    void Step(float dt, std::array<Input, 2> input);
    void NewWave();
    void Move(Tank &tank, int direction, float speed, float dt);
    void Fire(Tank &tank, bool enemy);
    void Projectiles(float dt);
    void EnemyAI(float dt);
    Status state_ = Status::Ready;
    bool coop_ = false;
    std::array<Tank, 2> players_{};
    std::array<int, rules::count> walls_{};
    std::vector<Tank> enemies_;
    std::vector<Bullet> bullets_;
    std::vector<Impact> impacts_;
    int wave_ = 1, score_ = 0, baseHealth_ = rules::baseHealth, spawnLeft_ = 0, spawnIndex_ = 0;
    float spawnTimer_ = 0;
};
} // namespace games::tank
