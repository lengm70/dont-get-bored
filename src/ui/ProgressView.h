#pragma once
#include "config/CollectionProgress.h"
#include "ui/MenuView.h"
namespace ui {
inline constexpr const char *progressGlyphs =
    "成绩与目标 已完成目标 点击任一行进入游戏 目标 最佳 记录 完成全部预设难度 赢下一盘对局 "
    "完成一次多轮对局 保护基地完成三波 完成全部曲目 完成全部赛道 获得分 完成单人三关 任一预设";
MenuResult DrawProgress(Font font, app::Language lang, const app::HighScores &high,
                        const app::ProgressRecords &records,
                        const games::breakout::Progress &breakout);
} // namespace ui
