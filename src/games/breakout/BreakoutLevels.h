#pragma once
#include <array>
#include <vector>
#include "games/breakout/BreakoutCollision.h"
#include "games/breakout/BreakoutRules.h"
namespace games::breakout {
enum class BrickKind { Normal, Armored, Explosive, Moving, Obstacle };
struct Brick {
    BrickKind kind = BrickKind::Normal;
    int health = 0;
    bool Alive() const { return health > 0; }
    bool Destructible() const { return kind != BrickKind::Obstacle; }
};
using BrickArray = std::array<Brick, rules::brickCount>;
struct BrickDamage { std::vector<int> damaged; std::vector<int> destroyed; };
BrickArray MakeLevel(int level);
Bounds BaseBrickBounds(int index);
Bounds CurrentBrickBounds(const BrickArray& bricks, int index, float seconds);
float BrickVelocity(const BrickArray& bricks, int index, float seconds);
// Explosions damage eight adjacent cells, including other explosives; obstacles survive.
BrickDamage DamageBricks(BrickArray& bricks, int index);
int RemainingBricks(const BrickArray& bricks);
} // namespace games::breakout
