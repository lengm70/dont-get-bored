#include "games/tank/TankGame.h"
#include <algorithm>
#include <cmath>
namespace games::tank {
namespace {
constexpr int dx[]{0, 1, 0, -1}, dy[]{-1, 0, 1, 0};
}
void TankGame::Reset(bool coop) {
    coop_ = coop;
    state_ = Status::Ready;
    wave_ = 1;
    score_ = 0;
    baseHealth_ = rules::baseHealth;
    spawnIndex_ = 0;
    enemies_.clear();
    bullets_.clear();
    impacts_.clear();
    walls_.fill(0);
    for (int y = 0; y < rules::rows; ++y)
        for (int x = 0; x < rules::columns; ++x) {
            if (x == 0 || x == rules::columns - 1 || y == 0 || y == rules::rows - 1)
                walls_[y * rules::columns + x] = 2;
            else if ((y == 3 || y == 8) && x % 5 >= 2 && x % 5 <= 3)
                walls_[y * rules::columns + x] = 1;
            else if (y == 6 && (x == 5 || x == 14))
                walls_[y * rules::columns + x] = 2;
        }
    for (int x = 8; x <= 11; ++x)
        walls_[(rules::rows - 1) * rules::columns + x] = 0;
    players_[0] = {255, 315, 0, rules::playerHealth};
    players_[1] = {345, 315, 0, coop ? rules::playerHealth : 0};
    NewWave();
}
void TankGame::NewWave() {
    spawnLeft_ = 1 + wave_ * 2;
    spawnTimer_ = 0;
    bullets_.clear();
}
void TankGame::Start() {
    if (state_ == Status::Ready)
        state_ = Status::Playing;
}
void TankGame::TogglePause() {
    if (state_ == Status::Playing)
        state_ = Status::Paused;
    else if (state_ == Status::Paused)
        state_ = Status::Playing;
}
int TankGame::RemainingEnemies() const {
    return spawnLeft_ + static_cast<int>(enemies_.size());
}
bool TankGame::Walkable(float x, float y) const {
    if (x < rules::radius || x > rules::width - rules::radius || y < rules::radius ||
        y > rules::height - rules::radius)
        return false;
    for (int row = std::max(0, int((y - rules::radius) / rules::cell));
         row <= std::min(rules::rows - 1, int((y + rules::radius) / rules::cell)); ++row)
        for (int col = std::max(0, int((x - rules::radius) / rules::cell));
             col <= std::min(rules::columns - 1, int((x + rules::radius) / rules::cell)); ++col)
            if (walls_[row * rules::columns + col])
                return false;
    return std::hypot(x - 300, y - 330) > rules::radius + 14;
}
void TankGame::Move(Tank &t, int dir, float speed, float dt) {
    if (dir < 0 || dir > 3)
        return;
    t.direction = dir;
    float x = t.x + dx[dir] * speed * dt, y = t.y + dy[dir] * speed * dt;
    if (Walkable(x, y)) {
        t.x = x;
        t.y = y;
    }
}
void TankGame::Fire(Tank &t, bool enemy) {
    if (t.health <= 0 || t.cooldown > 0)
        return;
    t.cooldown = enemy ? rules::enemyReload : rules::playerReload;
    bullets_.push_back(
        {t.x + dx[t.direction] * 14, t.y + dy[t.direction] * 14, t.direction, enemy});
}
void TankGame::Projectiles(float dt) {
    for (auto &b : bullets_) {
        b.x += dx[b.direction] * rules::bulletSpeed * dt;
        b.y += dy[b.direction] * rules::bulletSpeed * dt;
        if (b.x < 0 || b.x >= rules::width || b.y < 0 || b.y >= rules::height) {
            b.alive = false;
            continue;
        }
        int index = int(b.y / rules::cell) * rules::columns + int(b.x / rules::cell);
        if (walls_[index]) {
            if (walls_[index] == 1)
                walls_[index] = 0;
            b.alive = false;
        } else if (b.enemy && std::hypot(b.x - 300, b.y - 330) < 16) {
            --baseHealth_;
            b.alive = false;
        } else if (b.enemy) {
            for (auto &p : players_)
                if (p.health > 0 && std::hypot(b.x - p.x, b.y - p.y) < rules::radius + 2) {
                    --p.health;
                    p.flash = .2f;
                    b.alive = false;
                    break;
                }
        } else {
            for (auto &e : enemies_)
                if (e.health > 0 && std::hypot(b.x - e.x, b.y - e.y) < rules::radius + 2) {
                    --e.health;
                    e.flash = .2f;
                    b.alive = false;
                    if (e.health == 0)
                        score_ += rules::enemyPoints;
                    break;
                }
        }
        if (!b.alive)
            impacts_.push_back({b.x, b.y});
    }
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(), [](const Bullet &b) { return !b.alive; }),
        bullets_.end());
    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
                                  [](const Tank &e) { return e.health <= 0; }),
                   enemies_.end());
}
void TankGame::Step(float dt, std::array<Input, 2> input) {
    for (auto &e : impacts_)
        e.age += dt;
    impacts_.erase(std::remove_if(impacts_.begin(), impacts_.end(),
                                  [](const Impact &i) { return i.age > .4f; }),
                   impacts_.end());
    for (int i = 0; i < 2; ++i) {
        auto &p = players_[i];
        p.cooldown = std::max(0.f, p.cooldown - dt);
        p.flash = std::max(0.f, p.flash - dt);
        if (p.health > 0) {
            Move(p, input[i].direction, rules::playerSpeed, dt);
            if (input[i].fire)
                Fire(p, false);
        }
    }
    EnemyAI(dt);
    Projectiles(dt);
    if (baseHealth_ <= 0 || (players_[0].health <= 0 && players_[1].health <= 0)) {
        state_ = Status::GameOver;
        return;
    }
    if (RemainingEnemies() == 0) {
        score_ += rules::wavePoints;
        if (wave_ == rules::waves)
            state_ = Status::Won;
        else {
            ++wave_;
            NewWave();
        }
    }
}
void TankGame::Update(float seconds, std::array<Input, 2> input) {
    if (!std::isfinite(seconds) || state_ != Status::Playing)
        return;
    float left = std::clamp(seconds, 0.f, rules::maxFrame);
    while (left > 0 && state_ == Status::Playing) {
        float dt = std::min(left, rules::step);
        Step(dt, input);
        left -= dt;
    }
}
} // namespace games::tank
