#pragma once
#include <array>
#include <cstdint>
#include "games/bomber/BomberRules.h"
namespace games::bomber {
struct Cell {
    int x = 0, y = 0;
    bool operator==(Cell other) const { return x == other.x && y == other.y; }
    bool operator!=(Cell other) const { return !(*this == other); }
};
struct Point { float x = 0, y = 0; };
enum class Direction { None, Up, Right, Down, Left };
enum class Tile { Floor, Wall, Crate };
enum class Upgrade { None, Capacity, Range, Speed };
enum class Mode { Solo, Duel };
enum class Status { Ready, Playing, Paused, RoundOver, StageComplete, GameOver, MatchOver, Won };
enum class Outcome { SoloVictory, SoloDefeat, PlayerOne, PlayerTwo, Draw };
struct Options { Mode mode = Mode::Solo; int targetWins = rules::defaultTargetWins; };
struct Input { Direction movement = Direction::None; bool plant = false; };
struct Actor {
    Point position;
    Direction facing = Direction::Down;
    bool alive = true, human = true;
    int capacity = rules::initialCapacity, range = rules::initialRange;
    float speed = rules::playerSpeed;
};
struct Bomb { Cell cell; int owner = 0, range = rules::initialRange; float remaining = rules::fuseSeconds; unsigned passMask = 0; };
struct Flame { Cell cell; float remaining = rules::flameSeconds; };
struct Pickup { Cell cell; Upgrade kind = Upgrade::None; float protection = rules::pickupProtection; };
struct Debris { Cell cell; float remaining = rules::flameSeconds; Upgrade upgrade = Upgrade::None; };
inline constexpr std::array<Cell,4> directions{{{0,-1},{1,0},{0,1},{-1,0}}};
inline Cell Offset(Direction direction) {
    return direction == Direction::None ? Cell{} : directions[static_cast<int>(direction) - 1];
}
inline Direction AsDirection(Cell offset) {
    for (int i = 0; i < 4; ++i) if (directions[i] == offset) return static_cast<Direction>(i + 1);
    return Direction::None;
}
inline std::uint32_t NextRandom(std::uint32_t& state) {
    if (state == 0) state = 0x6d2b79f5u;
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return state;
}
Cell OccupiedCell(Point position);
bool TouchesCell(Point position, Cell cell);
} // namespace games::bomber
