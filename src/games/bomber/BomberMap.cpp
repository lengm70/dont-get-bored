#include "games/bomber/BomberMap.h"
#include <algorithm>
#include <cmath>
namespace games::bomber {
Cell OccupiedCell(Point position) { return {static_cast<int>(std::floor(position.x)),static_cast<int>(std::floor(position.y))}; }
bool TouchesCell(Point position, Cell cell) {
    const float dx = position.x - std::clamp(position.x,static_cast<float>(cell.x),static_cast<float>(cell.x + 1));
    const float dy = position.y - std::clamp(position.y,static_cast<float>(cell.y),static_cast<float>(cell.y + 1));
    return dx * dx + dy * dy < rules::actorRadius * rules::actorRadius;
}
bool Arena::Inside(Cell cell) { return cell.x >= 0 && cell.x < rules::columns && cell.y >= 0 && cell.y < rules::rows; }
Tile Arena::At(Cell cell) const { return Inside(cell) ? tiles[Index(cell)] : Tile::Wall; }
Point SpawnPosition(int actor) {
    constexpr std::array<Cell,5> spawns{{{1,1},{rules::columns-2,rules::rows-2},{rules::columns-2,1},{1,rules::rows-2},{rules::columns/2,rules::rows/2}}};
    const Cell cell = spawns[std::clamp(actor,0,4)];
    return {cell.x + 0.5f,cell.y + 0.5f};
}
Arena GenerateArena(std::uint32_t seed, Mode mode, int stage) {
    Arena map;
    for (int y = 0; y < rules::rows; ++y) for (int x = 0; x < rules::columns; ++x) {
        const Cell cell{x,y}; const int index = Arena::Index(cell);
        if (x == 0 || y == 0 || x == rules::columns - 1 || y == rules::rows - 1 || (x % 2 == 0 && y % 2 == 0)) {
            map.tiles[index] = Tile::Wall; continue;
        }
        bool safe = false;
        for (int actor = 0; actor < (mode == Mode::Solo && stage == rules::stages ? 5 : 4); ++actor) {
            const Cell spawn = OccupiedCell(SpawnPosition(actor));
            if (std::abs(x - spawn.x) <= 2 && std::abs(y - spawn.y) <= 2) safe = true;
        }
        // Rotation symmetry makes both starting sides equally equipped in duels.
        const int sample = mode == Mode::Duel ? std::min(index,rules::cellCount - 1 - index) : index;
        std::uint32_t state = seed ^ (static_cast<std::uint32_t>(sample + 1) * 0x9e3779b9u);
        const int density = rules::cratePercent + (mode == Mode::Solo ? std::clamp(stage-1,0,rules::stages-1) * rules::crateStageIncrease : 0);
        map.tiles[index] = !safe && static_cast<int>(NextRandom(state) % 100) < density ? Tile::Crate : Tile::Floor;
        if (map.tiles[index] == Tile::Crate && NextRandom(state) % 100 < rules::dropPercent)
            map.hidden[index] = static_cast<Upgrade>(1 + NextRandom(state) % 3);
    }
    return map;
}
} // namespace games::bomber
