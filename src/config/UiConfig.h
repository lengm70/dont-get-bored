#pragma once

#include "raylib.h"

namespace ui::config {

inline constexpr int designWidth = 960;
inline constexpr int designHeight = 600;
inline constexpr const char* windowTitle = "don't get bored";
inline constexpr const char* fontPath = "assets/fonts/menu.ttf";
inline constexpr int fontLoadSize = 48;
inline constexpr float textSpacing = 1.0f;

inline constexpr int titleY = 134;
inline constexpr int titleSize = 46;
inline constexpr int subtitleY = 202;
inline constexpr int subtitleSize = 22;
inline constexpr int pageTitleY = 126;
inline constexpr int pageTitleSize = 38;
inline constexpr int gameButtonY = 270;
inline constexpr int mainButtonY = 260;
inline constexpr int mainButtonGap = 71;
inline constexpr int mainButtonWidth = 320;
inline constexpr int backButtonY = 466;
inline constexpr int backButtonWidth = 240;
inline constexpr int buttonHeight = 54;
inline constexpr int settingsLabelX = 278;
inline constexpr int settingsRowY = 208;
inline constexpr int settingsRowGap = 64;
inline constexpr int settingsControlX = 455;
inline constexpr int settingsControlWidth = 260;
inline constexpr int settingsControlHeight = 44;
inline constexpr int settingsLabelOffsetY = 9;
inline constexpr int settingsLabelSize = 22;
inline constexpr Rectangle fpsCheckBox{390, 407, 28, 28};
inline constexpr int fpsCounterX = 24;
inline constexpr int fpsCounterY = 24;

inline constexpr Rectangle panel{220, 60, 520, 480};
inline constexpr float panelRoundness = 0.08f;
inline constexpr int panelSegments = 12;
inline constexpr Rectangle accentBar{448, 90, 64, 5};
inline constexpr float accentRoundness = 1.0f;
inline constexpr int accentSegments = 8;
inline constexpr int leftGlowX = 72;
inline constexpr int leftGlowY = 95;
inline constexpr int leftGlowRadius = 135;
inline constexpr int rightGlowX = 890;
inline constexpr int rightGlowY = 525;
inline constexpr int rightGlowRadius = 180;

inline constexpr Color textColor{238, 246, 255, 255};
inline constexpr Color mutedColor{160, 183, 206, 255};
inline constexpr Color accentColor{91, 226, 204, 255};
inline constexpr Color gradientTop{13, 24, 43, 255};
inline constexpr Color gradientBottom{20, 44, 63, 255};
inline constexpr Color leftGlowColor{56, 124, 139, 35};
inline constexpr Color rightGlowColor{47, 91, 154, 35};
inline constexpr Color panelColor{25, 42, 62, 244};
inline constexpr Color borderNormal{75, 104, 131, 255};
inline constexpr Color baseNormal{38, 60, 84, 255};
inline constexpr Color baseFocused{50, 78, 103, 255};
inline constexpr Color basePressed{38, 117, 112, 255};
inline constexpr int guiTextSize = 22;
inline constexpr int guiBorderWidth = 2;
inline constexpr int guiTextPadding = 8;
inline constexpr int guiComboButtonWidth = 72;
inline constexpr int guiComboButtonSpacing = 4;

}  // namespace ui::config
