#pragma once
#include "games/bomber/BomberAI.h"
#include "games/bomber/BomberMatch.h"
namespace games::bomber {
class BomberGame {
public:
    explicit BomberGame(std::uint32_t seed = 0);
    void Reset(Options options = {});
    void Start();
    void TogglePause() { match_.TogglePause(); }
    void Update(float elapsedSeconds, const std::array<Input,2>& input = {});
    const Arena& Map() const { return map_; }
    const std::vector<Actor>& Actors() const { return actors_; }
    const std::vector<Bomb>& Bombs() const { return bombs_; }
    const std::vector<Flame>& Flames() const { return flames_; }
    const std::vector<Pickup>& Pickups() const { return pickups_; }
    const std::vector<Debris>& Fragments() const { return debris_; }
    const Match& Progress() const { return match_; }
    Status State() const { return match_.State(); }
    int Score() const { return match_.Score(); }
    int EnemyCount() const;
    float RemainingSeconds() const { return remaining_; }
    float Clock() const { return clock_; }
    bool Settling() const { return settling_ >= 0; }
private:
    struct Navigation { Cell target; };
    void PrepareArena();
    void Step(float seconds, const std::array<Input,2>& input);
    bool Plant(int actor);
    bool CanOccupy(Point point, int actor) const;
    void MoveHuman(int actor, Direction direction, float seconds);
    void MoveEnemy(int actor, float seconds);
    void UpdatePassMasks();
    void TickExplosions(float seconds);
    void ApplyDamage();
    void CollectUpgrades(float seconds);
    void CheckRound(float seconds);
    Arena map_;
    Match match_;
    std::vector<Actor> actors_;
    std::vector<Navigation> navigation_;
    std::vector<Bomb> bombs_;
    std::vector<Flame> flames_;
    std::vector<Pickup> pickups_;
    std::vector<Debris> debris_;
    std::uint32_t random_;
    float remaining_ = rules::roundSeconds, clock_ = 0, settling_ = -1;
};
} // namespace games::bomber
