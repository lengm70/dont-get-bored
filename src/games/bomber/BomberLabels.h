#pragma once
#include <array>
#include "config/AppSettings.h"
namespace games::bomber {
struct Labels {
    const char *name, *description, *choose, *solo, *duel, *soloDescription, *duelDescription;
    const char *targetWins, *start, *setupHint, *score, *best, *lives, *stage, *enemies, *round, *time;
    const char *capacity, *range, *speed, *soloControls, *duelControls, *footer;
    const char *paused, *resume, *lostLife, *nextRound, *cleared, *nextStage, *gameOver, *won, *matchOver;
    const char *playerOneWins, *playerTwoWins, *draw, *restart, *settling, *upgrades;
};
inline constexpr Labels chinese{
    "炸弹人", "能量爆破，闯关或双人对战", "选择作战模式", "单人闯关", "本地双人",
    "三关 / 三条生命：炸开障碍，消灭全部无人机", "同屏对战：先达到目标胜局的玩家获胜",
    "目标胜局", "空格开始", "道具可增加弹数、爆炸距离和移速；爆炸会伤到自己",
    "分数", "最高分", "生命", "关卡", "敌人", "回合", "剩余",
    "弹数", "范围", "速度", "WASD / 方向键移动   F / 空格放弹",
    "P1: WASD + F 放弹     P2: 方向键 + Enter 放弹", "P 暂停   R 重开设置   Esc 返回",
    "已暂停", "P 继续", "损失一条生命", "空格进入下一回合", "关卡完成", "空格进入下一关",
    "游戏结束", "全部关卡完成", "对战结束", "玩家一获胜", "玩家二获胜", "双方平局",
    "R 重新设置", "正在结算", "道具：蓝色加弹 / 橙色加范围 / 绿色加速度"};
inline constexpr Labels english{
    "Bomber", "Energy blasts: solo or local duel", "Choose your mission", "Solo campaign", "Local duel",
    "3 sectors / 3 lives: clear crates and eliminate every drone", "Shared keyboard: reach the target number of wins first",
    "Wins needed", "Space to start", "Collect upgrades; your own blasts can also destroy you",
    "Score", "Best", "Lives", "Sector", "Drones", "Round", "Time",
    "Bombs", "Range", "Speed", "WASD / Arrows move   F / Space plant",
    "P1: WASD + F plant     P2: Arrows + Enter plant", "P pause   R setup / restart   Esc return",
    "Paused", "P to resume", "Life lost", "Space: next round", "Sector cleared", "Space: next sector",
    "Game over", "Campaign complete", "Match complete", "Player one wins", "Player two wins", "Draw",
    "R: new setup", "Resolving", "Drops: Blue capacity / Orange range / Green speed"};
inline const Labels& ForLanguage(app::Language language) { return language == app::Language::English ? english : chinese; }
inline std::array<const char*,38> AllLabels(const Labels& t) {
    return {t.name,t.description,t.choose,t.solo,t.duel,t.soloDescription,t.duelDescription,t.targetWins,t.start,t.setupHint,
        t.score,t.best,t.lives,t.stage,t.enemies,t.round,t.time,t.capacity,t.range,t.speed,t.soloControls,t.duelControls,t.footer,
        t.paused,t.resume,t.lostLife,t.nextRound,t.cleared,t.nextStage,t.gameOver,t.won,t.matchOver,t.playerOneWins,t.playerTwoWins,
        t.draw,t.restart,t.settling,t.upgrades};
}
} // namespace games::bomber
