#pragma once
#include "games/bomber/BomberExplosion.h"
namespace games::bomber {
Direction ChooseEnemyDirection(const Arena& map, const std::vector<Bomb>& bombs,
    const std::vector<Flame>& flames, const Actor& enemy, Point target, std::uint32_t& random);
} // namespace games::bomber
