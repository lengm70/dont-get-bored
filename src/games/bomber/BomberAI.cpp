#include "games/bomber/BomberAI.h"
#include <algorithm>
#include <cmath>
#include <queue>
namespace games::bomber {
namespace {
bool Walkable(const Arena& map, const std::vector<Bomb>& bombs, Cell cell) {
    return map.At(cell) == Tile::Floor && std::none_of(bombs.begin(),bombs.end(),
        [cell](const Bomb& bomb) { return bomb.cell == cell; });
}
}
Direction ChooseEnemyDirection(const Arena& map, const std::vector<Bomb>& bombs,
    const std::vector<Flame>& flames, const Actor& enemy, Point target, std::uint32_t& random) {
    const Cell start = OccupiedCell(enemy.position), goal = OccupiedCell(target);
    const auto danger = PredictDanger(map,bombs,flames);
    if (!Arena::Inside(start)) return Direction::None;
    const bool escape = danger[Arena::Index(start)] < rules::aiThreatHorizon;
    std::array<int,rules::cellCount> depth; depth.fill(-1);
    std::array<Direction,rules::cellCount> first{};
    std::queue<Cell> pending; pending.push(start); depth[Arena::Index(start)] = 0;
    const int rotation = static_cast<int>(NextRandom(random) % directions.size());
    Direction best = Direction::None;
    int bestDistance = rules::cellCount;
    while (!pending.empty()) {
        const Cell cell = pending.front(); pending.pop();
        const int index = Arena::Index(cell), steps = depth[index];
        if (steps > 0) {
            if (escape && !std::isfinite(danger[index])) return first[index];
            const int distance = std::abs(goal.x - cell.x) + std::abs(goal.y - cell.y);
            if (!escape && !std::isfinite(danger[index]) && distance < bestDistance) {
                bestDistance = distance; best = first[index];
            }
            if (!escape && cell == goal) break;
        }
        for (int n = 0; n < 4; ++n) {
            const Cell offset = directions[(n + rotation) % 4];
            const Cell next{cell.x + offset.x,cell.y + offset.y};
            if (!Walkable(map,bombs,next)) continue;
            const int nextIndex = Arena::Index(next);
            if (depth[nextIndex] >= 0) continue;
            const float arrival = (steps + 1) / enemy.speed + rules::aiSafetyMargin;
            if (danger[nextIndex] <= arrival) continue;
            depth[nextIndex] = steps + 1;
            first[nextIndex] = steps == 0 ? AsDirection(offset) : first[index];
            pending.push(next);
        }
    }
    if (!escape && NextRandom(random) % 100 >= rules::aiChasePercent) {
        for (int n = 0; n < 4; ++n) {
            const Cell offset = directions[(n + rotation) % 4], next{start.x + offset.x,start.y + offset.y};
            if (Walkable(map,bombs,next) && !std::isfinite(danger[Arena::Index(next)])) return AsDirection(offset);
        }
    }
    return best;
}
} // namespace games::bomber
