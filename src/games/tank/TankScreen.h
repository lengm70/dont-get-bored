#pragma once
#include "config/ProgressRecords.h"
#include "games/common/ArcadeUi.h"
#include "games/tank/TankGame.h"
namespace games::tank {
void UpdateFromInput(TankGame &game, float dt, app::ProgressRecords &records);
void DrawScreen(const TankGame &game, Font font, app::Language language,
                const app::ProgressRecords &records);
} // namespace games::tank
