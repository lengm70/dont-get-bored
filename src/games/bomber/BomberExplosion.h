#pragma once
#include <vector>
#include "games/bomber/BomberMap.h"
namespace games::bomber {
struct Detonation {
    std::vector<int> bombs;
    std::vector<Cell> flames, crates;
};
// Read-only arena snapshot: crates stop every ray in the same chain reaction.
Detonation ResolveExplosions(const Arena& map, const std::vector<Bomb>& bombs, const std::vector<int>& triggered);
std::vector<Cell> BlastCells(const Arena& map, const std::vector<Bomb>& bombs, int bombIndex);
std::array<float,rules::cellCount> PredictDanger(const Arena& map, const std::vector<Bomb>& bombs,
                                               const std::vector<Flame>& flames);
} // namespace games::bomber
