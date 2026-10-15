#pragma once
#include <array>
#include "config/AppSettings.h"
namespace games::breakout {
struct CampaignLabels {
    const char *level, *levelScore, *combo, *levelBest, *cleared, *next, *select, *bonus, *maxCombo;
    const char *wide, *multi, *shield, *shakeOn, *shakeOff, *legend, *controls, *shortControls;
    std::array<const char*, 5> names;
};
inline constexpr CampaignLabels campaignChinese{
    "关卡", "本关分数", "连击", "本关纪录", "关卡完成", "空格进入下一关", "上下键或点击选择已解锁关卡",
    "生命奖励", "最大连击", "加长", "多球", "护盾", "震动开", "震动关",
    "装甲需多次命中 / 爆破连锁 / 移动砖块 / 障碍不会阻止通关",
    "鼠标 / 左右键 / A/D 移动   空格发球   P 暂停   R 重开",
    "S 开关震动   Esc 返回   道具：加长 12 秒 / 多球 / 一次护盾",
    {"初次接触", "装甲阵列", "连锁反应", "轨道迁移", "核心突破"}};
inline constexpr CampaignLabels campaignEnglish{
    "Level", "Sector score", "Combo", "Level best", "Sector cleared", "Space: next sector", "Up/Down or click an unlocked sector",
    "Life bonus", "Max combo", "Wide", "Multi", "Shield", "Shake on", "Shake off",
    "Armor: multiple hits / Blast: chain / Moving / Solid: indestructible",
    "Mouse / Arrows / A/D move   Space serve   P pause   R restart",
    "S toggle shake   Esc return   Drops: Wide 12s / Multi / Shield",
    {"First Contact", "Armor Array", "Chain Reaction", "Orbital Shift", "Core Breach"}};
inline const CampaignLabels& CampaignFor(app::Language language) {
    return language == app::Language::English ? campaignEnglish : campaignChinese;
}
inline std::array<const char*, 22> AllLabels(const CampaignLabels& labels) {
    return {labels.level, labels.levelScore, labels.combo, labels.levelBest, labels.cleared, labels.next, labels.select,
        labels.bonus, labels.maxCombo, labels.wide, labels.multi, labels.shield, labels.shakeOn,
        labels.shakeOff, labels.legend, labels.controls, labels.shortControls,
        labels.names[0], labels.names[1], labels.names[2], labels.names[3], labels.names[4]};
}
} // namespace games::breakout
