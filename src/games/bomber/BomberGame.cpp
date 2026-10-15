#include "games/bomber/BomberGame.h"
#include "games/bomber/BomberPowerUps.h"
#include <algorithm>
#include <cmath>
#include <random>
namespace games::bomber {
BomberGame::BomberGame(std::uint32_t seed) : random_(seed ? seed : std::random_device{}()) { Reset(); }
void BomberGame::Reset(Options options) { match_.Reset(options); PrepareArena(); }
void BomberGame::PrepareArena() {
    map_ = GenerateArena(NextRandom(random_),match_.Settings().mode,match_.Stage());
    bombs_.clear(); flames_.clear(); pickups_.clear(); debris_.clear(); actors_.clear(); navigation_.clear();
    remaining_ = rules::roundSeconds; clock_ = 0; settling_ = -1;
    const bool solo = match_.Settings().mode == Mode::Solo;
    const int count = solo ? match_.Stage() + 2 : 2;
    for (int i = 0; i < count; ++i) {
        Actor actor;
        actor.position = SpawnPosition(i); actor.human = !solo || i == 0;
        if (!actor.human) actor.speed = rules::enemySpeed + (match_.Stage() - 1) * rules::enemyStageSpeed;
        actors_.push_back(actor); navigation_.push_back({OccupiedCell(actor.position)});
    }
}
void BomberGame::Start() {
    const bool carryUpgrades = State() == Status::StageComplete;
    const Actor previous = actors_.front();
    if (match_.Advance()) {
        PrepareArena();
        if (carryUpgrades) {
            actors_[0].capacity = previous.capacity;
            actors_[0].range = previous.range;
            actors_[0].speed = previous.speed;
        }
    }
    match_.Start();
}
int BomberGame::EnemyCount() const {
    return static_cast<int>(std::count_if(actors_.begin(),actors_.end(),[](const Actor& actor) { return !actor.human && actor.alive; }));
}
bool BomberGame::Plant(int actorIndex) {
    auto& actor = actors_[actorIndex];
    if (!actor.alive || !actor.human || settling_ >= 0) return false;
    const int active = static_cast<int>(std::count_if(bombs_.begin(),bombs_.end(),
        [actorIndex](const Bomb& bomb) { return bomb.owner == actorIndex; }));
    const Cell cell = OccupiedCell(actor.position);
    if (active >= actor.capacity || map_.At(cell) != Tile::Floor ||
        std::any_of(bombs_.begin(),bombs_.end(),[cell](const Bomb& bomb) { return bomb.cell == cell; })) return false;
    unsigned passMask = 0;
    for (int i = 0; i < static_cast<int>(actors_.size()); ++i)
        if (actors_[i].alive && TouchesCell(actors_[i].position,cell)) passMask |= 1u << i;
    bombs_.push_back({cell,actorIndex,actor.range,rules::fuseSeconds,passMask});
    return true;
}
void BomberGame::TickExplosions(float seconds) {
    for (auto& debris : debris_) debris.remaining -= seconds;
    debris_.erase(std::remove_if(debris_.begin(),debris_.end(),[](const Debris& item) { return item.remaining <= 0; }),debris_.end());
    for (auto& flame : flames_) flame.remaining -= seconds;
    flames_.erase(std::remove_if(flames_.begin(),flames_.end(),[](const Flame& flame) { return flame.remaining <= 0; }),flames_.end());
    std::vector<int> triggered;
    for (int i = 0; i < static_cast<int>(bombs_.size()); ++i) {
        bombs_[i].remaining -= seconds;
        if (bombs_[i].remaining <= 0 || std::any_of(flames_.begin(),flames_.end(),
            [this,i](const Flame& flame) { return flame.cell == bombs_[i].cell; })) triggered.push_back(i);
    }
    if (triggered.empty()) return;
    const Detonation event = ResolveExplosions(map_,bombs_,triggered);
    for (Cell cell : event.flames) {
        auto found = std::find_if(flames_.begin(),flames_.end(),[cell](const Flame& flame) { return flame.cell == cell; });
        if (found == flames_.end()) flames_.push_back({cell,rules::flameSeconds});
        else found->remaining = rules::flameSeconds;
    }
    for (Cell cell : event.crates) {
        const int index = Arena::Index(cell);
        map_.tiles[index] = Tile::Floor; debris_.push_back({cell,rules::flameSeconds});
        match_.AddScore(rules::cratePoints);
        if (map_.hidden[index] != Upgrade::None) pickups_.push_back({cell,map_.hidden[index],rules::pickupProtection});
        map_.hidden[index] = Upgrade::None;
    }
    // Erase from the end to preserve snapshot bomb indices throughout chain resolution.
    for (auto i = event.bombs.rbegin(); i != event.bombs.rend(); ++i) bombs_.erase(bombs_.begin() + *i);
}
void BomberGame::ApplyDamage() {
    for (auto& actor : actors_) {
        if (!actor.alive) continue;
        if (std::any_of(flames_.begin(),flames_.end(),[&actor](const Flame& flame) { return TouchesCell(actor.position,flame.cell); })) {
            actor.alive = false;
            debris_.push_back({OccupiedCell(actor.position),rules::flameSeconds});
            if (!actor.human) match_.AddScore(rules::enemyPoints);
        }
    }
    if (match_.Settings().mode != Mode::Solo || !actors_[0].alive) return;
    for (const auto& enemy : actors_) {
        if (enemy.human || !enemy.alive) continue;
        if (std::hypot(enemy.position.x - actors_[0].position.x,enemy.position.y - actors_[0].position.y) < rules::actorRadius * 2) {
            actors_[0].alive = false;
            debris_.push_back({OccupiedCell(actors_[0].position),rules::flameSeconds});
            break;
        }
    }
}
void BomberGame::CollectUpgrades(float seconds) {
    for (auto& pickup : pickups_) {
        pickup.protection = std::max(0.0f,pickup.protection - seconds);
        if (pickup.protection > 0) continue;
        if (std::any_of(flames_.begin(),flames_.end(),[&pickup](const Flame& flame) { return flame.cell == pickup.cell; })) {
            pickup.kind = Upgrade::None; continue;
        }
        for (auto& actor : actors_) {
            if (!actor.alive || !actor.human || OccupiedCell(actor.position) != pickup.cell) continue;
            ApplyUpgrade(actor,pickup.kind);
            debris_.push_back({pickup.cell,rules::flameSeconds,pickup.kind});
            pickup.kind = Upgrade::None; break;
        }
    }
    pickups_.erase(std::remove_if(pickups_.begin(),pickups_.end(),[](const Pickup& pickup) { return pickup.kind == Upgrade::None; }),pickups_.end());
}
void BomberGame::CheckRound(float seconds) {
    const bool solo = match_.Settings().mode == Mode::Solo;
    const int alive = static_cast<int>(std::count_if(actors_.begin(),actors_.end(),[](const Actor& actor) { return actor.alive; }));
    const bool finished = remaining_ <= 0 || (solo ? !actors_[0].alive || EnemyCount() == 0 : alive <= 1);
    if (!finished && settling_ < 0) return;
    if (settling_ < 0) settling_ = rules::settleSeconds;
    settling_ -= seconds;
    if (settling_ > 0) return;
    if (solo) match_.Finish(actors_[0].alive && EnemyCount() == 0 ? Outcome::SoloVictory : Outcome::SoloDefeat);
    else match_.Finish(remaining_ <= 0 || alive != 1 ? Outcome::Draw : actors_[0].alive ? Outcome::PlayerOne : Outcome::PlayerTwo);
}
void BomberGame::Step(float seconds, const std::array<Input,2>& input) {
    clock_ += seconds; remaining_ = std::max(0.0f,remaining_ - seconds);
    // Damage at both ends prevents crossing a still-active flame during a movement step.
    ApplyDamage();
    for (int i = 0; i < static_cast<int>(actors_.size()); ++i) {
        if (!actors_[i].alive) continue;
        if (actors_[i].human) {
            if (i < 2 && input[i].plant) Plant(i);
            MoveHuman(i,input[i].movement,seconds);
        } else MoveEnemy(i,seconds);
    }
    UpdatePassMasks(); TickExplosions(seconds); ApplyDamage(); CollectUpgrades(seconds); CheckRound(seconds);
}
void BomberGame::Update(float elapsed, const std::array<Input,2>& input) {
    if (State() != Status::Playing || !std::isfinite(elapsed) || elapsed <= 0) return;
    auto controls = input;
    float remaining = std::min(elapsed,rules::maxFrameSeconds);
    while (remaining > 0 && State() == Status::Playing) {
        const float step = std::min(remaining,rules::physicsStep);
        Step(step,controls);
        // A key press is an edge, not a repeated request in each physics substep.
        for (auto& player : controls) player.plant = false;
        remaining -= step;
    }
}
} // namespace games::bomber
