#pragma once
#include "raylib.h"
#include "games/bomber/BomberRules.h"
namespace games::bomber::config {
inline constexpr Rectangle panel{120,30,720,555};
inline constexpr float tileSize = 34, boardX = 225, boardY = 143;
inline constexpr float boardWidth = rules::columns * tileSize, boardHeight = rules::rows * tileSize;
inline constexpr int titleY = 48, titleSize = 30, statsY = 91, statsSize = 20, upgradesY = 120, upgradesSize = 14;
inline constexpr int controlsY = 534, controlsSize = 14, footerY = 559, footerSize = 14;
inline constexpr float border = 2, cellInset = 2, bombRadius = 0.23f, pulseRate = 11;
inline constexpr int overlayTitleY = 255, overlayTitleSize = 30, overlayDetailY = 307, overlayDetailSize = 21;
inline constexpr int overlayHintY = 360, overlayHintSize = 18;
inline constexpr int setupTitleY = 172, setupTitleSize = 25, setupDescriptionY = 274, setupDescriptionSize = 17;
inline constexpr Rectangle soloButton{285,216,185,40}, duelButton{490,216,185,40};
inline constexpr Rectangle fewerButton{380,336,44,36}, moreButton{536,336,44,36};
inline constexpr Rectangle startButton{345,426,270,44};
inline constexpr int winsLabelY = 307, winsValueY = 343, setupHintY = 392;
inline constexpr Color playerOne{91,229,209,255}, playerTwo{255,147,177,255};
inline constexpr Color enemy{213,162,255,255}, crate{184,140,227,255};
inline constexpr Color wall{81,92,132,255}, flame{255,185,90,255};
inline constexpr Color capacity{102,188,255,255}, range{255,180,106,255}, speed{108,238,195,255};
inline constexpr float fragmentDistance = 0.7f;
} // namespace games::bomber::config
