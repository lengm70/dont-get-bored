#pragma once

#include "raylib.h"
#include "ui/UiPalette.h"

namespace ui::selection {

inline constexpr Rectangle panel{60, 32, 840, 536};
inline constexpr int titleY = 68;
inline constexpr int titleSize = 34;
inline constexpr int subtitleY = 115;
inline constexpr int subtitleSize = 17;
inline constexpr int cardCount = 5;
inline constexpr float cardWidth = 148;
inline constexpr float cardHeight = 276;
inline constexpr float cardGap = 14;
inline constexpr float cardsY = 164;
inline constexpr float contentPadding = 12;
inline constexpr float illustrationHeight = 138;
inline constexpr float titleOffsetY = 160;
inline constexpr float descriptionOffsetY = 198;
inline constexpr float playOffsetY = 242;
inline constexpr float nameSize = 18;
inline constexpr float descriptionSize = 11;
inline constexpr float playSize = 14;
inline constexpr float cornerRoundness = 0.10f;
inline constexpr int cornerSegments = 10;
inline constexpr float borderWidth = 1;
inline constexpr float focusBorderWidth = 2;
inline constexpr float shadowOffset = 5;
inline constexpr float arrowInset = 18;
inline constexpr float arrowSize = 5;
inline constexpr Rectangle backButton{400, 509, 160, 38};
inline constexpr float artSize = 160;
inline const Color& cardColor = ui::palette::card;
inline const Color& focusedColor = ui::palette::cardFocused;
inline const Color& pressedColor = ui::palette::cardPressed;
inline const Color& shadowColor = ui::palette::shadow;
inline const Color& borderColor = ui::palette::cardBorder;
inline const Color& artBackground = ui::palette::board;
inline const Color& artGridColor = ui::palette::grid;
inline constexpr Color mint{91, 226, 204, 255};
inline constexpr Color purple{181, 151, 246, 255};
inline constexpr Color orange{248, 178, 109, 255};
inline constexpr Color blue{113, 188, 252, 255};
inline constexpr Color coral{244, 116, 137, 255};
inline constexpr Color yellow{244, 211, 114, 255};
inline const Color& darkInk = ui::palette::board;

}  // namespace ui::selection
