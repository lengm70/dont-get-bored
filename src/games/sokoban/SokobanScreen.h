#pragma once
#include "config/ProgressRecords.h"
#include "games/common/ArcadeUi.h"
#include "games/sokoban/SokobanGame.h"
namespace games::sokoban {
inline constexpr const char *glyphs =
    "推箱子 将箱子推至全部目标 步数 推动 撤销 关卡 已解锁 空格下一关 方向键移动 空格开始 左右选关 "
    "Z撤销 箱子无法拉动 全部20关完成";
struct ViewState {
    float animation = 1, repeat = 0;
    int held = -1;
};
void UpdateFromInput(SokobanGame &game, ViewState &view, float dt, app::ProgressRecords &records);
void DrawScreen(const SokobanGame &game, const ViewState &view, Font font, app::Language lang,
                const app::ProgressRecords &records);
} // namespace games::sokoban
