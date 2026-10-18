#pragma once
#include "config/ProgressRecords.h"
#include "games/common/ArcadeUi.h"
#include "games/race/RaceGame.h"
namespace games::race {
inline constexpr const char *glyphs =
    "太空竞速 三条赛道，漂移计时 圈数 时间 最快单圈 最佳总用时 速度 左右选赛道 上加速 下刹车 "
    "左右转向 空格漂移 检查点 按顺序通过检查点";
void UpdateFromInput(RaceGame &game, float dt, app::ProgressRecords &records);
void DrawScreen(const RaceGame &game, Font font, app::Language lang,
                const app::ProgressRecords &records);
} // namespace games::race
