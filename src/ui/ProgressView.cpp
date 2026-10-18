#include "ui/ProgressView.h"
#include "games/common/ArcadeUi.h"
#include "ui/AnimatedControls.h"
#include "ui/GeometricPanel.h"
namespace ui {
MenuResult DrawProgress(Font font, app::Language lang, const app::HighScores &high,
                        const app::ProgressRecords &records,
                        const games::breakout::Progress &breakout) {
    using namespace games::arcade;
    auto layout = CurrentLayout();
    bool zh = Chinese(lang);
    const auto rows = app::CollectionProgress(high, records, breakout);
    DrawBackground(layout, false, {90, 30, 780, 545});
    Text(font, zh ? "成绩与目标" : "Records & Goals", 55, 30, palette::text, layout);
    Text(font,
         std::string(zh ? "已完成目标 " : "Goals completed ") +
             std::to_string(app::CompletedGoals(rows)) + " / 12",
         103, 19, cyan, layout);
    auto bar = layout.Rect({160, 136, 640, 5});
    DrawRectangleRec(bar, Fade(cyan, .1f));
    bar.width *= app::CompletedGoals(rows) / 12.f;
    DrawRectangleRec(bar, cyan);
    static int page = 0;
    constexpr MenuAction actions[]{
        MenuAction::StartSnake,       MenuAction::StartTetris, MenuAction::StartBreakout,
        MenuAction::StartMinesweeper, MenuAction::StartGomoku, MenuAction::StartFruit,
        MenuAction::StartTwentyOne,   MenuAction::StartBomber, MenuAction::StartTank,
        MenuAction::StartSokoban,     MenuAction::StartRhythm, MenuAction::StartRace};
    for (int i = 0; i < 6; ++i) {
        int index = page * 6 + i;
        const auto &e = rows[index];
        float y = 158 + i * 51;
        Rectangle design{130, y, 700, 46};
        auto bounds = layout.Rect(design);
        bool click = AnimatedButton(bounds, nullptr);
        DrawGeometricPanel(bounds, 5 * layout.scale, palette::card,
                           e.Complete() ? cyan : palette::cardBorder, layout.scale);
        DrawScaledText(font, zh ? e.chineseName : e.englishName, 148, y + 4, 17, palette::text,
                       layout);
        std::string record = std::string(zh ? "最佳 " : "Best ") + e.record;
        float size = 11;
        float measured = MeasureTextEx(font, record.c_str(), size * layout.scale, layout.scale).x;
        if (measured > 300 * layout.scale)
            size *= 300 * layout.scale / measured;
        DrawScaledText(font, record.c_str(), 148, y + 27, size, palette::muted, layout);
        DrawScaledText(font, zh ? e.chineseGoal : e.englishGoal, 460, y + 8, 13, palette::muted,
                       layout);
        std::string status = e.Complete() ? "OK"
                                          : std::to_string(std::min(e.value, e.target)) + "/" +
                                                std::to_string(e.target);
        DrawScaledText(font, status.c_str(), 755, y + 14, 15, e.Complete() ? cyan : gold, layout);
        if (click)
            return {actions[index]};
    }
    if (AnimatedButton(layout.Rect({300, 477, 55, 32}), "<"))
        page = 1 - page;
    if (AnimatedButton(layout.Rect({605, 477, 55, 32}), ">"))
        page = 1 - page;
    Text(font, std::to_string(page + 1) + " / 2", 485, 15, palette::text, layout);
    Text(font, zh ? "点击任一行进入游戏" : "Click a row to launch its game", 518, 12,
         palette::muted, layout);
    if (AnimatedButton(layout.Rect({380, 540, 200, 30}), zh ? "返回" : "Back"))
        return {MenuAction::Back};
    return {};
}
} // namespace ui
