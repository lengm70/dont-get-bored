#pragma once

#include <array>

#include "config/AppSettings.h"

namespace ui::text {

inline constexpr const char* title = "don't get bored";
inline constexpr const char* resolutionChoices = "720p;1080p;2K;4K";
inline constexpr const char* fpsChoices = "60;120;165;240";
// raygui counters and generated score text need these glyphs.
inline constexpr const char* generatedTextGlyphs = "0123456789/:%<>|";

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
    const char* music;
    const char* importMusic;
    const char* musicVolume;
    const char* previousMusic;
    const char* nextMusic;
    const char* basic;
    const char* playPause;
    const char* background;
    const char* languageChoices;
};

inline constexpr Labels chinese{
    "小游戏合集", "选择游戏", "设置", "退出", "返回", "显示 FPS",
    "语言", "屏幕分辨率", "帧率上限", "Music", "Import", "BGM", "<<", ">>", "Basic", "||", "Background", "中文;English"};

inline constexpr Labels english{
    "Mini Game Collection", "Select Game", "Settings", "Quit", "Back",
    "Show FPS", "Language", "Resolution", "FPS limit", "Music", "Import local music", "Music volume", "Previous", "Next", "Basic", "Play/Pause", "Background", "Chinese;English"};

inline const Labels& ForLanguage(app::Language language) {
    return language == app::Language::English ? english : chinese;
}

inline constexpr std::array<const char*, 18> AllLabels(const Labels& labels) {
    return {labels.subtitle, labels.selectGame, labels.settings, labels.quit,
            labels.back, labels.showFps, labels.language, labels.resolution,
            labels.fpsLimit, labels.music, labels.importMusic, labels.musicVolume,
            labels.previousMusic, labels.nextMusic, labels.basic, labels.playPause,
            labels.background, labels.languageChoices};
}

struct SelectionLabels {
    const char* subtitle;
    const char* snakeHint;
    const char* tetrisHint;
    const char* breakoutHint;
    const char* minesweeperHint;
    const char* gomokuHint;
    const char* play;
};

inline constexpr SelectionLabels selectionChinese{
    "五款小游戏，随时开局", "吃食物，让小蛇长大", "旋转方块，消除整行",
    "接住小球，击碎砖块", "插旗排雷，寻找安全格", "双人对弈，挑战电脑", "进入游戏"};
inline constexpr SelectionLabels selectionEnglish{
    "Five games. Pick your next break.", "Grow your snake", "Stack & clear",
    "Keep bouncing", "Find safe tiles", "Connect five", "Play"};

inline const SelectionLabels& SelectionForLanguage(app::Language language) {
    return language == app::Language::English ? selectionEnglish : selectionChinese;
}

inline constexpr std::array<const char*, 7> AllLabels(const SelectionLabels& labels) {
    return {labels.subtitle, labels.snakeHint, labels.tetrisHint,
            labels.breakoutHint, labels.minesweeperHint, labels.gomokuHint, labels.play};
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

struct MinesweeperSetupLabels {
    const char* choose;
    const char* choices;
    const char* columns;
    const char* rows;
    const char* mines;
    const char* start;
    const char* startHint;
    const char* customHint;
    const char* noCustomRecord;
    std::array<const char*, 4> names;
};

inline constexpr MinesweeperSetupLabels minesweeperSetupChinese{
    "选择难度", "简单;普通;困难;自定义", "地图宽度", "地图高度", "地雷数量",
    "开始游戏", "按空格开始；首次翻开及周围安全", "宽 5-40，高 5-30；自定义不保存纪录",
    "不记录", {"简单", "普通", "困难", "自定义"}};
inline constexpr MinesweeperSetupLabels minesweeperSetupEnglish{
    "Choose difficulty", "Easy;Normal;Hard;Custom", "Columns", "Rows", "Mines",
    "Start game", "Space to start. First reveal and neighbors are safe.",
    "Width 5-40, height 5-30. No custom records.", "Not recorded",
    {"Easy", "Normal", "Hard", "Custom"}};

inline const MinesweeperSetupLabels& MinesweeperSetupForLanguage(app::Language language) {
    return language == app::Language::English ? minesweeperSetupEnglish : minesweeperSetupChinese;
}

inline constexpr std::array<const char*, 13> AllLabels(const MinesweeperSetupLabels& labels) {
    return {labels.choose, labels.choices, labels.columns, labels.rows, labels.mines,
            labels.start, labels.startHint, labels.customHint, labels.noCustomRecord,
            labels.names[0], labels.names[1], labels.names[2], labels.names[3]};
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

struct GomokuLabels {
    const char* name;
    const char* mode;
    const char* modes;
    const char* difficulty;
    const char* difficulties;
    const char* humanBlack;
    const char* twoPlayers;
    const char* rules;
    const char* start;
    const char* ready;
    const char* blackTurn;
    const char* whiteTurn;
    const char* thinking;
    const char* blackWins;
    const char* whiteWins;
    const char* draw;
    const char* controls;
};
inline constexpr GomokuLabels gomokuChinese{
    "五子棋", "游戏模式", "双人对弈;人机对弈", "人机难度", "简单;普通;困难",
    "你执黑棋先行，电脑执白棋", "黑棋先行，双方轮流落子",
    "15×15 棋盘，无禁手；连续五颗或以上获胜", "开始游戏（空格）",
    "按空格开始", "轮到黑棋", "轮到白棋", "电脑思考中…",
    "黑棋获胜！", "白棋获胜！", "平局", "左键落子  R 重开  空格开始  Esc 返回"};
inline constexpr GomokuLabels gomokuEnglish{
    "Gomoku", "Game mode", "Two players;Vs computer", "AI difficulty", "Easy;Normal;Hard",
    "You play Black first. The computer plays White.", "Black starts. Take turns placing stones.",
    "15x15 board. No forbidden moves. Five or more in a row wins.", "Start game (Space)",
    "Press Space to start", "Black to move", "White to move", "Computer is thinking...",
    "Black wins!", "White wins!", "Draw", "Left click place  R restart  Space start  Esc return"};
inline const GomokuLabels& GomokuForLanguage(app::Language language) {
    return language == app::Language::English ? gomokuEnglish : gomokuChinese;
}
inline constexpr std::array<const char*, 17> AllLabels(const GomokuLabels& labels) {
    return {labels.name, labels.mode, labels.modes, labels.difficulty, labels.difficulties,
            labels.humanBlack, labels.twoPlayers, labels.rules, labels.start, labels.ready,
            labels.blackTurn, labels.whiteTurn, labels.thinking, labels.blackWins,
            labels.whiteWins, labels.draw, labels.controls};
}
struct MediaLabels {
    const char* title;
    const char* categories;
    const char* defaultItem;
    const char* import;
    const char* use;
    const char* restore;
    const char* current;
    const char* hint;
    const char* failure;
};
inline constexpr MediaLabels mediaChinese{"媒体库", "大背景;播放器背景;背景音乐", "默认",
    "添加文件", "使用选中项", "恢复默认", "正在使用此项目", "选择历史项目，或添加新文件", "无法使用此文件，已保留原设置"};
inline constexpr MediaLabels mediaEnglish{"Media Library", "Background;Player backdrop;Music", "Default",
    "Add file", "Use selected", "Restore default", "Currently in use", "Choose a saved item or add a file", "Could not load file. Previous selection kept."};
inline const MediaLabels& MediaForLanguage(app::Language language) {
    return language == app::Language::English ? mediaEnglish : mediaChinese;
}
inline constexpr std::array<const char*, 9> AllLabels(const MediaLabels& labels) {
    return {labels.title, labels.categories, labels.defaultItem, labels.import, labels.use,
            labels.restore, labels.current, labels.hint, labels.failure};
}
}  // namespace ui::text
