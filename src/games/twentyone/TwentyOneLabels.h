#pragma once
#include <array>
#include "config/AppSettings.h"
namespace games::twentyone {
struct Labels {
    const char *title, *hint, *description, *player, *ai, *score, *draw, *hold,
               *again, *active, *won, *lost, *tie, *waiting, *held, *bust, *twentyone, *controls;
};
inline constexpr Labels chinese{
    "21 点", "A 算 1 或 11 点 | J / Q / K 算 10 点 | AI 达到 17 点停牌",
    "抽牌来凑成21点", "玩家", "AI", "总分", "抽牌", "停牌", "再玩一局",
    "继续抽牌，还是停牌？", "你赢了！", "AI 赢了！", "平局！",
    "你已结束抽牌，等待 AI 停牌…", "已停牌", "爆牌", "21 点，已停牌",
    "D 抽牌 | H 停牌 | R 重开 | Esc 返回"};
inline constexpr Labels english{
    "21", "A = 1 or 11 | J / Q / K = 10 | AI holds at 17",
    "Draw cards to reach 21", "You", "AI", "Score", "Draw", "Hold", "Play again",
    "Draw another card or hold?", "You win!", "AI wins!", "Tie!",
    "Your hand is finished. Waiting for AI...", "Held", "Bust", "21 - held",
    "D draw | H hold | R restart | Esc back"};
inline const Labels& ForLanguage(app::Language language) {
    return language == app::Language::English ? english : chinese;
}
inline std::array<const char*,18> AllLabels(const Labels& l) {
    return {l.title,l.hint,l.description,l.player,l.ai,l.score,l.draw,l.hold,l.again,
        l.active,l.won,l.lost,l.tie,l.waiting,l.held,l.bust,l.twentyone,l.controls};
}
}
