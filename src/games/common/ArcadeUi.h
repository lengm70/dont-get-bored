#pragma once
#include "config/AppSettings.h"
#include "raylib.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiPalette.h"
#include <algorithm>
#include <string>
namespace games::arcade {
enum class Status { Ready, Playing, Paused, Won, GameOver };
inline constexpr Rectangle panel{100, 28, 760, 550}, board{180, 145, 600, 360};
inline constexpr Color cyan{91, 229, 209, 255}, pink{255, 147, 177, 255}, gold{255, 194, 107, 255};
inline constexpr const char *glyphs =
    "准备开始 空格开始 已暂停 按P继续 完成 游戏结束 按R重开 返回 暂停 重开 单人 双人合作 选择关卡 "
    "最佳 分数 生命 基地 波次 坦克大战 保护基地，击退敌军 WASD方向键移动 F空格开火 "
    "玩家方向键Enter开火 切换合作模式 消灭敌人 目标";
inline bool Chinese(app::Language l) {
    return l == app::Language::Chinese;
}
inline void Text(Font font, const std::string &text, float y, float size, Color color,
                 const ui::UiLayout &layout) {
    float measured = MeasureTextEx(font, text.c_str(), size * layout.scale, layout.scale).x;
    if (measured > 720 * layout.scale)
        size *= 720 * layout.scale / measured;
    ui::DrawCenteredText(font, text.c_str(), y, size, color, layout);
}
inline void Header(Font font, const char *title, const std::string &stats, const ui::UiLayout &l) {
    ui::DrawBackground(l, false, panel);
    Text(font, title, 49, 29, ui::palette::text, l);
    Text(font, stats, 94, 18, cyan, l);
}
inline void Footer(Font font, const char *controls, app::Language language, const ui::UiLayout &l) {
    Text(font, controls, 524, 14, ui::palette::muted, l);
    Text(font,
         Chinese(language) ? "P 暂停   R 重开   Esc 返回" : "P pause   R restart   Esc return", 551,
         14, ui::palette::muted, l);
}
inline void Overlay(Status state, Font font, app::Language lang, const ui::UiLayout &l,
                    const std::string &detail = {}) {
    if (state == Status::Playing)
        return;
    bool zh = Chinese(lang);
    DrawRectangleRec(l.Rect(board), ui::palette::overlay);
    const char *title = state == Status::Ready    ? (zh ? "准备开始" : "Ready")
                        : state == Status::Paused ? (zh ? "已暂停" : "Paused")
                        : state == Status::Won    ? (zh ? "完成" : "Complete")
                                                  : (zh ? "游戏结束" : "Game over");
    Text(font, title, 257, 30, ui::palette::text, l);
    Text(font, detail, 306, 19, cyan, l);
    Text(font,
         state == Status::Ready    ? (zh ? "空格开始" : "Space to start")
         : state == Status::Paused ? (zh ? "按 P 继续" : "P to resume")
                                   : (zh ? "按 R 重开" : "R to restart"),
         359, 18, ui::palette::muted, l);
}
inline Vector2 Point(float x, float y, const ui::UiLayout &l) {
    return l.Point(board.x + x, board.y + y);
}
} // namespace games::arcade
