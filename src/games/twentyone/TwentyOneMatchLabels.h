#pragma once
#include <array>
#include "config/AppSettings.h"
namespace games::twentyone {
struct MatchLabels {
    const char *setup,*funds,*rounds,*seats,*roles,*human,*ai,*start,*rules,*limits,
        *round,*pot,*stake,*payout,*turn,*raise,*raiseAmount,*next,*finished,*leaders,*out,*restart,*controls,*balance;
};
inline constexpr MatchLabels matchChinese{
    "21 点：对局设置","初始资金","对局轮数","席位数量","每席角色","玩家","AI","空格开始",
    "每轮底注 50；普通抽牌免费；加注并抽牌。最高未爆牌点数赢分池，平局分配。",
    "资金 50–1000000；轮数 1–100；2–4 席，至少一位玩家。",
    "轮次","筹码池","下注","获得","当前行动","加注并抽牌","加注金额","下一轮","对局结束",
    "资金最多者","已出局","回到设置","D 抽牌 | B 加注抽牌 | H 停牌 | 空格下一轮 | R 设置 | Esc 返回","资金"};
inline constexpr MatchLabels matchEnglish{
    "21: Match setup","Starting funds","Rounds","Seats","Seat roles","Human","AI","Space to start",
    "Ante 50. Draw free or raise + draw. Best non-bust hand wins each side pot; ties split.",
    "Funds 50-1000000; rounds 1-100; 2-4 seats, at least one human.",
    "Round","Pot","Bet","Payout","Acting","Raise + draw","Raise amount","Next round","Match finished",
    "Most funds","Eliminated","Back to setup","D draw | B raise + draw | H hold | Space next round | R setup | Esc back","Funds"};
inline const MatchLabels& MatchForLanguage(app::Language language) { return language==app::Language::English?matchEnglish:matchChinese; }
inline std::array<const char*,24> AllLabels(const MatchLabels& l) {
    return {l.setup,l.funds,l.rounds,l.seats,l.roles,l.human,l.ai,l.start,l.rules,l.limits,l.round,l.pot,
        l.stake,l.payout,l.turn,l.raise,l.raiseAmount,l.next,l.finished,l.leaders,l.out,l.restart,l.controls,l.balance};
}
}  // namespace games::twentyone
