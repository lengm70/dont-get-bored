#include "games/bomber/BomberExplosion.h"
#include <algorithm>
#include <limits>
namespace games::bomber {
namespace {
int BombAt(const std::vector<Bomb>& bombs, Cell cell) {
    for (int i = 0; i < static_cast<int>(bombs.size()); ++i) if (bombs[i].cell == cell) return i;
    return -1;
}
void AppendUnique(std::vector<Cell>& cells, Cell cell) {
    if (std::find(cells.begin(),cells.end(),cell) == cells.end()) cells.push_back(cell);
}
}
std::vector<Cell> BlastCells(const Arena& map, const std::vector<Bomb>& bombs, int index) {
    if (index < 0 || index >= static_cast<int>(bombs.size())) return {};
    const Bomb& bomb = bombs[index];
    std::vector<Cell> cells{bomb.cell};
    for (Cell direction : directions) for (int distance = 1; distance <= std::clamp(bomb.range,1,rules::maxRange); ++distance) {
        const Cell cell{bomb.cell.x + direction.x * distance,bomb.cell.y + direction.y * distance};
        if (map.At(cell) == Tile::Wall) break;
        cells.push_back(cell);
        if (map.At(cell) == Tile::Crate || BombAt(bombs,cell) >= 0) break;
    }
    return cells;
}
Detonation ResolveExplosions(const Arena& map, const std::vector<Bomb>& bombs, const std::vector<int>& triggered) {
    Detonation result;
    std::vector<int> pending = triggered;
    std::vector<bool> visited(bombs.size());
    while (!pending.empty()) {
        const int index = pending.back(); pending.pop_back();
        if (index < 0 || index >= static_cast<int>(bombs.size()) || visited[index]) continue;
        visited[index] = true; result.bombs.push_back(index);
        for (Cell cell : BlastCells(map,bombs,index)) {
            AppendUnique(result.flames,cell);
            if (map.At(cell) == Tile::Crate) AppendUnique(result.crates,cell);
            const int neighbor = BombAt(bombs,cell);
            if (neighbor >= 0 && !visited[neighbor]) pending.push_back(neighbor);
        }
    }
    std::sort(result.bombs.begin(),result.bombs.end());
    return result;
}
std::array<float,rules::cellCount> PredictDanger(const Arena& map, const std::vector<Bomb>& bombs,
                                               const std::vector<Flame>& flames) {
    std::array<float,rules::cellCount> danger;
    danger.fill(std::numeric_limits<float>::infinity());
    std::vector<float> due;
    std::vector<std::vector<Cell>> rays;
    for (int i = 0; i < static_cast<int>(bombs.size()); ++i) {
        const bool alreadyBurning = std::any_of(flames.begin(),flames.end(),[&bombs,i](const Flame& flame) {
            return flame.remaining > 0 && flame.cell == bombs[i].cell;
        });
        due.push_back(alreadyBurning ? 0.0f : std::max(0.0f,bombs[i].remaining));
        rays.push_back(BlastCells(map,bombs,i));
    }
    // Propagate earlier fuse times through the bomb graph before projecting danger.
    for (std::size_t pass = 0; pass < bombs.size(); ++pass) {
        bool changed = false;
        for (int i = 0; i < static_cast<int>(bombs.size()); ++i) for (Cell cell : rays[i]) {
            const int next = BombAt(bombs,cell);
            if (next >= 0 && due[next] > due[i]) { due[next] = due[i]; changed = true; }
        }
        if (!changed) break;
    }
    for (int i = 0; i < static_cast<int>(bombs.size()); ++i) for (Cell cell : rays[i])
        if (Arena::Inside(cell)) danger[Arena::Index(cell)] = std::min(danger[Arena::Index(cell)],due[i]);
    for (const auto& flame : flames)
        if (flame.remaining > 0 && Arena::Inside(flame.cell)) danger[Arena::Index(flame.cell)] = 0;
    return danger;
}
} // namespace games::bomber
