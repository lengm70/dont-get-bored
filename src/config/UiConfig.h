#pragma once

#include "raylib.h"
#include "ui/UiPalette.h"

namespace ui::config {

inline constexpr int designWidth = 960;
inline constexpr int designHeight = 600;
inline constexpr const char* windowTitle = "don't get bored";
inline constexpr const char* fontPath = "assets/fonts/menu.ttf";
inline constexpr const char* backgroundPath = "assets/backgrounds/space.png";
inline constexpr int fontLoadSize = 48;
inline constexpr float textSpacing = 1.0f;

inline constexpr int titleY = 134;
inline constexpr int titleSize = 46;
inline constexpr int subtitleY = 202;
inline constexpr int subtitleSize = 22;
inline constexpr int pageTitleY = 126;
inline constexpr int pageTitleSize = 38;
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

inline constexpr Color textColor = ui::palette::text;
inline constexpr Color mutedColor = ui::palette::muted;
inline constexpr Color accentColor = ui::palette::accent;
inline constexpr Color gradientTop = ui::palette::gradientTop;
inline constexpr Color gradientBottom = ui::palette::gradientBottom;
inline constexpr Color panelColor = ui::palette::panel;
inline constexpr Color borderNormal = ui::palette::border;
inline constexpr Color baseNormal = ui::palette::control;
inline constexpr Color baseFocused = ui::palette::focused;
inline constexpr Color basePressed = ui::palette::pressed;
inline constexpr int guiTextSize = 22;
inline constexpr int guiBorderWidth = 2;
inline constexpr int guiTextPadding = 8;
inline constexpr int guiComboButtonWidth = 72;
inline constexpr int guiComboButtonSpacing = 4;

}  // namespace ui::config
