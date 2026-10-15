#include "games/bomber/BomberPowerUps.h"
#include <algorithm>
namespace games::bomber {
void ApplyUpgrade(Actor& actor, Upgrade upgrade) {
    if (!actor.alive || !actor.human) return;
    switch (upgrade) {
        case Upgrade::Capacity: actor.capacity = std::min(rules::maxCapacity,actor.capacity + 1); break;
        case Upgrade::Range: actor.range = std::min(rules::maxRange,actor.range + 1); break;
        case Upgrade::Speed: actor.speed = std::min(rules::maxPlayerSpeed,actor.speed + rules::speedUpgrade); break;
        case Upgrade::None: break;
    }
}
} // namespace games::bomber
