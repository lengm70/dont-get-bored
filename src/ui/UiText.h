#pragma once

#include <array>

#include "config/AppSettings.h"

namespace ui::text {

inline constexpr const char* title = "don't get bored";
inline constexpr const char* resolutionChoices = "720p;1080p;2K;4K";
inline constexpr const char* fpsChoices = "60;120;165;240";
// raygui counters and generated score text need these glyphs.
inline constexpr const char* generatedTextGlyphs = "0123456789/:";

struct Labels {
    const char* subtitle;
    const char* selectGame;
    const char* settings;
    const char* quit;
    const char* back;
    const char* showFps;
    const char* language;
    const char* resolution;
    const char* fpsLimit;
    const char* languageChoices;
};

inline constexpr Labels chinese{
    "小游戏合集", "选择游戏", "设置", "退出", "返回", "显示 FPS",
    "语言", "屏幕分辨率", "帧率上限", "中文;English"};

inline constexpr Labels english{
    "Mini Game Collection", "Select Game", "Settings", "Quit", "Back",
    "Show FPS", "Language", "Resolution", "FPS limit", "Chinese;English"};

inline const Labels& ForLanguage(app::Language language) {
    return language == app::Language::English ? english : chinese;
}

inline constexpr std::array<const char*, 10> AllLabels(const Labels& labels) {
    return {labels.subtitle, labels.selectGame, labels.settings, labels.quit,
            labels.back, labels.showFps, labels.language, labels.resolution,
            labels.fpsLimit, labels.languageChoices};
}

struct SnakeLabels {
    const char* name;
    const char* score;
    const char* best;
    const char* ready;
    const char* paused;
    const char* gameOver;
    const char* won;
    const char* startHint;
    const char* resumeHint;
    const char* restartHint;
    const char* controlsHint;
    const char* pauseHint;
    const char* backHint;
};

inline constexpr SnakeLabels snakeChinese{
    "贪吃蛇", "分数", "最高分", "准备开始", "已暂停", "游戏结束", "胜利！",
    "按空格开始", "空格继续",
    "按 R 重新开始", "方向键或 WASD 移动", "空格暂停", "Esc 返回"};

inline constexpr SnakeLabels snakeEnglish{
    "Snake", "Score", "Best", "Ready", "Paused", "Game Over", "You Win!",
    "Press Space to start", "Space to resume",
    "Press R to restart", "Arrows or WASD to move", "Space to pause", "Esc to return"};

inline const SnakeLabels& SnakeForLanguage(app::Language language) {
    return language == app::Language::English ? snakeEnglish : snakeChinese;
}

inline constexpr std::array<const char*, 13> AllLabels(const SnakeLabels& labels) {
    return {labels.name, labels.score, labels.best, labels.ready, labels.paused, labels.gameOver,
            labels.won, labels.startHint, labels.resumeHint, labels.restartHint,
            labels.controlsHint, labels.pauseHint, labels.backHint};
}

struct TetrisLabels {
    const char* name;
    const char* next;
    const char* score;
    const char* best;
    const char* lines;
    const char* level;
    const char* ready;
    const char* paused;
    const char* gameOver;
    const char* startHint;
    const char* resumeHint;
    const char* restartHint;
    const char* controlsFirst;
    const char* controlsSecond;
};

inline constexpr TetrisLabels tetrisChinese{
    "俄罗斯方块", "下一个", "分数", "最高分", "消除行数", "等级",
    "准备开始", "已暂停", "游戏结束", "按空格开始", "按 P 继续",
    "按 R 重开", "左右移动  上键或 X 旋转  Z 反转  下键加速",
    "空格落下  P 暂停  R 重开  Esc 返回"};

inline constexpr TetrisLabels tetrisEnglish{
    "Tetris", "Next", "Score", "Best", "Lines", "Level", "Ready", "Paused",
    "Game Over", "Press Space to start", "Press P to resume",
    "Press R to restart", "Left/Right move  Up/X rotate  Z reverse  Down soft drop",
    "Space hard drop  P pause  R restart  Esc return"};

inline const TetrisLabels& TetrisForLanguage(app::Language language) {
    return language == app::Language::English ? tetrisEnglish : tetrisChinese;
}

inline constexpr std::array<const char*, 14> AllLabels(const TetrisLabels& labels) {
    return {labels.name, labels.next, labels.score, labels.best, labels.lines,
            labels.level, labels.ready, labels.paused, labels.gameOver,
            labels.startHint, labels.resumeHint, labels.restartHint,
            labels.controlsFirst, labels.controlsSecond};
}

struct BreakoutLabels {
    const char* name;
    const char* score;
    const char* best;
    const char* lives;
    const char* ready;
    const char* paused;
    const char* gameOver;
    const char* won;
    const char* startHint;
    const char* resumeHint;
    const char* restartHint;
    const char* controlsHint;
};

inline constexpr BreakoutLabels breakoutChinese{
    "打砖块", "分数", "最高分", "生命", "准备开始", "已暂停", "游戏结束", "胜利！",
    "按空格发球", "按 P 继续", "按 R 重开",
    "左右键或 A/D 移动  空格发球  P 暂停  R 重开  Esc 返回"};

inline constexpr BreakoutLabels breakoutEnglish{
    "Breakout", "Score", "Best", "Lives", "Ready", "Paused", "Game Over", "You Win!",
    "Press Space to serve", "Press P to resume", "Press R to restart",
    "Arrows or A/D move  Space serve  P pause  R restart  Esc return"};

inline const BreakoutLabels& BreakoutForLanguage(app::Language language) {
    return language == app::Language::English ? breakoutEnglish : breakoutChinese;
}

inline constexpr std::array<const char*, 12> AllLabels(const BreakoutLabels& labels) {
    return {labels.name, labels.score, labels.best, labels.lives, labels.ready,
            labels.paused, labels.gameOver, labels.won, labels.startHint,
            labels.resumeHint, labels.restartHint, labels.controlsHint};
}

struct MinesweeperLabels {
    const char* name;
    const char* mines;
    const char* time;
    const char* best;
    const char* noRecord;
    const char* ready;
    const char* paused;
    const char* gameOver;
    const char* won;
    const char* startHint;
    const char* resumeHint;
    const char* safeHint;
    const char* controlsFirst;
    const char* controlsSecond;
};

inline constexpr MinesweeperLabels minesweeperChinese{
    "扫雷", "剩余雷数", "用时（秒）", "最佳用时", "暂无", "准备开始", "已暂停",
    "游戏结束  按 R 重开", "胜利！按 R 重开", "按空格开始", "按 P 继续",
    "首次翻开安全", "左键翻开  右键插旗  点击数字展开周围",
    "P 暂停  R 重开  Esc 返回"};

inline constexpr MinesweeperLabels minesweeperEnglish{
    "Minesweeper", "Mines", "Time (s)", "Best (s)", "None", "Ready", "Paused",
    "Game Over - R to restart", "You Win! R to restart", "Press Space to start",
    "Press P to resume", "First reveal is safe",
    "Left reveal  Right flag  Click number to reveal neighbors",
    "P pause  R restart  Esc return"};

inline const MinesweeperLabels& MinesweeperForLanguage(app::Language language) {
    return language == app::Language::English ? minesweeperEnglish : minesweeperChinese;
}

inline constexpr std::array<const char*, 14> AllLabels(const MinesweeperLabels& labels) {
    return {labels.name, labels.mines, labels.time, labels.best, labels.noRecord,
            labels.ready, labels.paused, labels.gameOver, labels.won, labels.startHint,
            labels.resumeHint, labels.safeHint, labels.controlsFirst, labels.controlsSecond};
}

}  // namespace ui::text
