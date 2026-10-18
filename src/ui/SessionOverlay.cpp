#include "ui/SessionOverlay.h"
namespace ui {
HelpText GameHelp(app::Screen s, app::Language language) {
    bool z = language == app::Language::Chinese;
    switch (s) {
    case app::Screen::Snake:
        return {z ? "吃食物，让小蛇长大" : "Eat food and grow",
                z ? "方向键 / WASD 转向" : "Arrows / WASD turn",
                z ? "空格开始；R 重开" : "Space starts; R restarts"};
    case app::Screen::Tetris:
        return {z ? "旋转方块，消除整行" : "Rotate pieces and clear rows",
                (z ? "左右键或A/D移动；上键或X旋转；Z反向旋转"
                   : "Arrows / A,D move; Up / X rotate; Z reverse"),
                (z ? "下键或S软降；开始后空格硬降"
                   : "Down / S soft drop; Space hard drop after start")};
    case app::Screen::Breakout:
        return {z ? "接球击砖，完成五关" : "Break all five sectors",
                (z ? "鼠标或左右键移动；空格发球" : "Mouse / A,D / Arrows move; Space serve"),
                (z ? "收集道具；准备时上下键选择已解锁关卡"
                   : "Collect drops; Up/Down selects unlocked sector on Ready")};
    case app::Screen::Minesweeper:
        return {z ? "揭开安全格，避开地雷" : "Reveal all safe cells",
                (z ? "左键揭开；右键插旗" : "Left-click reveal; right-click flag"),
                (z ? "点击数字可快速揭开；自定义难度不记纪录"
                   : "Click a number to chord; custom maps have no records")};
    case app::Screen::Gomoku:
        return {
            z ? "连成五子，双人或挑战电脑" : "Connect five; local or versus AI",
            (z ? "左键在交点落子；黑方先行" : "Left-click an intersection; black goes first"),
            (z ? "R重新开始；开始前可选择难度" : "R resets; choose difficulty before starting")};
    case app::Screen::Fruit:
        return {
            z ? "合并相同物体，获得更高分" : "Merge matching orbs",
            (z ? "鼠标或A/D瞄准；点击或空格投放" : "Mouse / A,D aim; click or Space drops"),
            (z ? "空格开始；避免物体超过容器" : "Space starts; avoid overflowing the container")};
    case app::Screen::TwentyOne:
        return {z ? "多轮筹码池对局，资金最多者获胜" : "Finish with the most funds",
                (z ? "D抽牌；H停牌；B加注并抽牌" : "D draw; H hold; B raise and draw"),
                (z ? "按钮调整加注金额；分池处理不同下注额"
                   : "Adjust raise with buttons; side pots handle uneven stakes")};
    case app::Screen::Bomber:
        return {
            z ? "清除无人机或赢得本地对战" : "Clear drones or win local duels",
            (z ? "单人：WASD或方向键移动；F或空格放弹" : "Solo: WASD / Arrows + F / Space"),
            (z ? "双人：P1使用WASD加F；P2方向键加Enter" : "Duel: P1 WASD + F; P2 Arrows + Enter")};
    case app::Screen::Tank:
        return {
            z ? "保护基地，击退三波坦克" : "Defend the base for three waves",
            (z ? "P1使用WASD加F或空格；P2方向键加Enter" : "P1 WASD + F / Space; P2 Arrows + Enter"),
            (z ? "开始前Tab切换单人或合作；空格开始"
               : "Tab switches solo / co-op before Space starts")};
    case app::Screen::Sokoban:
        return {z ? "将全部箱子推上目标" : "Push every box onto a goal",
                (z ? "方向键或WASD移动；Z撤销一步" : "Arrows / WASD move; Z undoes a move"),
                (z ? "准备时左右选择已解锁关卡；空格开始"
                   : "Left/Right selects unlocked levels before Space")};
    case app::Screen::Rhythm:
        return {z ? "跟随音乐，准确点击四条音轨" : "Hit the four lanes to the music",
                (z ? "D F J K点击音轨；左右键选曲" : "D F J K hit; Left/Right selects a track"),
                (z ? "开始前上下键调整并保存判定延迟"
                   : "Up/Down adjusts saved timing offset before starting")};
    case app::Screen::Race:
        return {z ? "按顺序通过检查点，完成三圈" : "Pass checkpoints in order for three laps",
                (z ? "上键或W加速；下键或S刹车；左右键转向"
                   : "Up/W accelerate; Down/S brake; A,D/Arrows steer"),
                (z ? "开始后空格漂移；离开赛道会减速"
                   : "Space drifts after start; off-track travel slows you")};
    default:
        return {"", "", ""};
    }
}
void DrawSessionOverlay(const app::SessionGuard &guard, app::Screen screen, Font font,
                        app::Language lang) {
    if (!app::IsGameScreen(screen))
        return;
    auto l = CurrentLayout();
    using namespace games::arcade;
    bool z = Chinese(lang);
    if (!guard.Blocked()) {
        Text(font, z ? "F1 操作说明 / P 暂停" : "F1: Help / P: Pause", 580, 11, palette::muted, l);
        return;
    }
    DrawRectangleRec(l.Rect({110, 38, 740, 530}), Fade(palette::board, .97f));
    Text(font, guard.HelpOpen() ? (z ? "操作说明" : "Controls") : (z ? "已暂停" : "Paused"), 150,
         32, palette::text, l);
    auto help = GameHelp(screen, lang);
    Text(font, help.goal, 221, 21, cyan, l);
    Text(font, help.first, 278, 17, palette::text, l);
    Text(font, help.second, 321, 16, palette::muted, l);
    Text(font,
         z ? "空格开始   P 暂停   R 重开   Esc 返回"
           : "Space: Start   P: Pause   R: Restart   Esc: Return",
         385, 15, palette::muted, l);
    Text(font,
         guard.HelpOpen() ? (z ? "按 F1 或 Esc 关闭" : "F1 or Esc closes help")
                          : (z ? "按 P 继续；失去焦点后不会自动继续"
                               : "P resumes; returning focus does not auto-resume"),
         451, 18, cyan, l);
}
} // namespace ui
