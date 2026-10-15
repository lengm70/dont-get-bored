#pragma once
#include "games/bomber/BomberTypes.h"
namespace games::bomber {
struct Arena {
    std::array<Tile,rules::cellCount> tiles{};
    std::array<Upgrade,rules::cellCount> hidden{};
    static bool Inside(Cell cell);
    static int Index(Cell cell) { return cell.y * rules::columns + cell.x; }
    Tile At(Cell cell) const;
};
Arena GenerateArena(std::uint32_t seed, Mode mode, int stage);
Point SpawnPosition(int actor);
} // namespace games::bomber
