#pragma once

#include "raylib.h"
#include "ui/UiPalette.h"

namespace ui::config {

inline constexpr int designWidth = 960;
inline constexpr int designHeight = 600;
inline constexpr const char* windowTitle = "don't get bored";
inline constexpr const char* fontPath = "assets/fonts/menu.ttf";
inline constexpr const char* backgroundPath = "assets/backgrounds/space.png";
inline constexpr const char* defaultMusicPath = "assets/music/default.mp3";
inline constexpr const char* musicPlayerBackgroundPath = "assets/music/player.jpg";
inline constexpr const char* iconPath = "assets/icons/app.png";
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
inline constexpr int backButtonY = 465;
inline constexpr int backButtonWidth = 180;
inline constexpr int buttonHeight = 44;
inline constexpr int settingsLabelX = 278;
inline constexpr int settingsRowY = 215;
inline constexpr int settingsRowGap = 50;
inline constexpr int settingsControlX = 455;
inline constexpr int settingsControlWidth = 260;
inline constexpr int settingsControlHeight = 44;
inline constexpr int settingsLabelOffsetY = 9;
inline constexpr int settingsLabelSize = 22;
inline constexpr Rectangle fpsCheckBox{390, 425, 28, 28};
inline constexpr Rectangle musicPlayer{720, 444, 224, 126};
inline constexpr int fpsCounterX = 24;
inline constexpr int fpsCounterY = 24;

inline constexpr Rectangle panel{220, 60, 520, 480};
inline constexpr float panelRoundness = 0.08f;
inline constexpr int panelSegments = 12;
inline constexpr Rectangle accentBar{448, 90, 64, 5};
inline constexpr float accentRoundness = 1.0f;
inline constexpr int accentSegments = 8;

inline const Color& textColor = ui::palette::text;
inline const Color& mutedColor = ui::palette::muted;
inline const Color& accentColor = ui::palette::accent;
inline const Color& gradientTop = ui::palette::gradientTop;
inline const Color& gradientBottom = ui::palette::gradientBottom;
inline const Color& panelColor = ui::palette::panel;
inline const Color& borderNormal = ui::palette::border;
inline const Color& baseNormal = ui::palette::control;
inline const Color& baseFocused = ui::palette::focused;
inline const Color& basePressed = ui::palette::pressed;
inline constexpr int guiTextSize = 22;
inline constexpr int guiBorderWidth = 2;
inline constexpr int guiTextPadding = 8;
inline constexpr int guiListItemHeight = 36;
inline constexpr int guiListItemSpacing = 4;
inline constexpr int guiComboButtonWidth = 72;
inline constexpr int guiComboButtonSpacing = 4;

}  // namespace ui::config
