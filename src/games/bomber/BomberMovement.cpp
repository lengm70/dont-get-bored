#include "games/bomber/BomberGame.h"
#include <algorithm>
#include <cmath>
namespace games::bomber {
bool BomberGame::CanOccupy(Point point, int actor) const {
    const int minX = static_cast<int>(std::floor(point.x - rules::actorRadius));
    const int minY = static_cast<int>(std::floor(point.y - rules::actorRadius));
    const int maxX = static_cast<int>(std::floor(point.x + rules::actorRadius));
    const int maxY = static_cast<int>(std::floor(point.y + rules::actorRadius));
    for (int y = minY; y <= maxY; ++y) for (int x = minX; x <= maxX; ++x)
        if (map_.At({x,y}) != Tile::Floor && TouchesCell(point,{x,y})) return false;
    for (const auto& bomb : bombs_)
        if (!(bomb.passMask & (1u << actor)) && TouchesCell(point,bomb.cell)) return false;
    return true;
}
void BomberGame::MoveHuman(int index, Direction direction, float seconds) {
    if (direction == Direction::None) return;
    auto& actor = actors_[index]; actor.facing = direction;
    const Cell delta = Offset(direction);
    Point next{actor.position.x + delta.x * actor.speed * seconds,actor.position.y + delta.y * actor.speed * seconds};
    if (CanOccupy(next,index)) { actor.position = next; return; }
    // Gently center on a nearby corridor when the requested turn catches a corner.
    Point aligned = actor.position;
    float& perpendicular = delta.x ? aligned.y : aligned.x;
    const float center = std::floor(perpendicular) + 0.5f;
    const float difference = center - perpendicular;
    if (std::abs(difference) > rules::cornerAssist) return;
    perpendicular += std::clamp(difference,-actor.speed * seconds,actor.speed * seconds);
    if (CanOccupy(aligned,index)) actor.position = aligned;
    next = {actor.position.x + delta.x * actor.speed * seconds,actor.position.y + delta.y * actor.speed * seconds};
    if (CanOccupy(next,index)) actor.position = next;
}
void BomberGame::MoveEnemy(int index, float seconds) {
    auto& actor = actors_[index]; auto& route = navigation_[index];
    Point destination{route.target.x + 0.5f,route.target.y + 0.5f};
    float dx = destination.x - actor.position.x, dy = destination.y - actor.position.y;
    if (std::hypot(dx,dy) < rules::aiCenterTolerance) {
        actor.position = destination;
        const Direction direction = ChooseEnemyDirection(map_,bombs_,flames_,actor,actors_[0].position,random_);
        actor.facing = direction;
        const Cell offset = Offset(direction), cell = OccupiedCell(actor.position);
        route.target = {cell.x + offset.x,cell.y + offset.y};
        destination = {route.target.x + 0.5f,route.target.y + 0.5f};
        dx = destination.x - actor.position.x; dy = destination.y - actor.position.y;
    }
    const float distance = std::hypot(dx,dy);
    if (distance == 0) return;
    const float travel = std::min(distance,actor.speed * seconds);
    const Point next{actor.position.x + dx / distance * travel,actor.position.y + dy / distance * travel};
    if (CanOccupy(next,index)) actor.position = next;
    else route.target = OccupiedCell(actor.position);
}
void BomberGame::UpdatePassMasks() {
    for (auto& bomb : bombs_) for (int i = 0; i < static_cast<int>(actors_.size()); ++i)
        if (!actors_[i].alive || !TouchesCell(actors_[i].position,bomb.cell)) bomb.passMask &= ~(1u << i);
}
} // namespace games::bomber
