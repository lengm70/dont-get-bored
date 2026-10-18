#pragma once
#include "games/tank/TankGame.h"
#include "games/common/ArcadeUi.h"
#include "config/ProgressRecords.h"
namespace games::tank {
void UpdateFromInput(TankGame& game,float dt,app::ProgressRecords& records);
void DrawScreen(const TankGame& game,Font font,app::Language language,const app::ProgressRecords& records);
}
