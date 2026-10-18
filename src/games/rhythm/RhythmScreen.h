#pragma once
#include "config/ProgressRecords.h"
#include "games/common/ArcadeUi.h"
#include "games/rhythm/RhythmAudio.h"
#include "games/rhythm/RhythmGame.h"
#include <array>
namespace games::rhythm {
inline constexpr const char *glyphs =
    "节奏点击 三首原创电子曲 音准 连击 漏拍 错按 最高连击 评级 左右选曲 上下调整判定延迟毫秒 "
    "原创合成音乐 播放失败，请检查素材 空格开始 判定 音符以音乐播放时间为准";
struct ViewState {
    float feedback = 0;
    int judgment = -1;
    std::array<float, 4> laneGlow{};
    int offsetMs = 0;
    bool audioFailed = false;
};
void UpdateFromInput(RhythmGame &game, RhythmAudio &audio, ViewState &view, float dt, float volume,
                     app::ProgressRecords &records);
void DrawScreen(const RhythmGame &game, const ViewState &view, Font font, app::Language lang,
                const app::ProgressRecords &records);
} // namespace games::rhythm
