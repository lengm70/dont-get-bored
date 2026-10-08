#pragma once
#include "raylib.h"
namespace ui::mediaConfig {
inline constexpr Rectangle panel{160, 40, 640, 540};
inline constexpr float titleY = 65, titleSize = 30;
inline constexpr Rectangle category{270, 116, 420, 42};
inline constexpr Rectangle list{220, 176, 520, 234};
inline constexpr float statusY = 425, statusSize = 15;
inline constexpr Rectangle importButton{220, 464, 154, 38}, useButton{403, 464, 154, 38};
inline constexpr Rectangle defaultButton{586, 464, 154, 38}, backButton{400, 524, 160, 38};
}
