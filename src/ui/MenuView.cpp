#include "ui/MenuView.h"

#include <algorithm>
#include <array>
#include <cstdio>

#include "raygui.h"
#include "config/UiConfig.h"
#include "ui/GameSelectionView.h"
#include "ui/GameSelectionConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"
#include "ui/MusicPlayerInput.h"
#include "ui/PlayerBackground.h"

namespace ui {
namespace {
MusicPlayerInput musicPlayerInput;

void DrawRowLabel(Font font, const char* label, int row, const UiLayout& layout) {
    const int y = config::settingsRowY + row * config::settingsRowGap +
                  config::settingsLabelOffsetY;
    DrawScaledText(font, label, config::settingsLabelX, y,
                   config::settingsLabelSize, config::textColor, layout);
}

Rectangle CenteredButton(int y, int width, const UiLayout& layout) {
    return layout.Rect(Rectangle{
        static_cast<float>((config::designWidth - width) / 2),
        static_cast<float>(y), static_cast<float>(width),
        static_cast<float>(config::buttonHeight)});
}

Rectangle SettingsControl(int row, const UiLayout& layout) {
    return layout.Rect(Rectangle{
        static_cast<float>(config::settingsControlX),
        static_cast<float>(config::settingsRowY + row * config::settingsRowGap),
        static_cast<float>(config::settingsControlWidth),
        static_cast<float>(config::settingsControlHeight)});
}

MenuResult DrawMainMenu(Font font, const text::Labels& labels, const UiLayout& layout) {
    DrawCenteredText(font, text::title, config::titleY, config::titleSize,
                     config::textColor, layout);
    DrawCenteredText(font, labels.subtitle, config::subtitleY, config::subtitleSize,
                     config::mutedColor, layout);
    if (GuiButton(CenteredButton(config::mainButtonY, config::mainButtonWidth, layout),
                  labels.selectGame)) return {MenuAction::OpenGameSelection};
    if (GuiButton(CenteredButton(config::mainButtonY + config::mainButtonGap,
                                 config::mainButtonWidth, layout), labels.settings)) {
        return {MenuAction::OpenSettings};
    }
    if (GuiButton(CenteredButton(config::mainButtonY + 2 * config::mainButtonGap,
                                 config::mainButtonWidth, layout), labels.quit)) {
        return {MenuAction::Quit};
    }
    return {};
}

MenuResult DrawSettings(Font font, app::Settings& settings,
                        const text::Labels& labels, const UiLayout& layout) {
    DrawCenteredText(font, labels.settings, config::pageTitleY,
                     config::pageTitleSize, config::textColor, layout);
    const MenuAction tabAction = GuiButton(layout.Rect({330, 160, 140, 40}), labels.basic)
        ? MenuAction::OpenSettings : GuiButton(layout.Rect({490, 160, 140, 40}), labels.music)
        ? MenuAction::OpenMusicSettings : MenuAction::None;

    bool changed = false;
    DrawRowLabel(font, labels.language, 0, layout);
    int language = static_cast<int>(settings.language);
    if (GuiComboBox(SettingsControl(0, layout), labels.languageChoices, &language)) {
        settings.language = static_cast<app::Language>(language);
        changed = true;
    }

    DrawRowLabel(font, labels.resolution, 1, layout);
    int resolution = static_cast<int>(settings.resolution);
    if (GuiComboBox(SettingsControl(1, layout), text::resolutionChoices, &resolution)) {
        settings.resolution = static_cast<app::Resolution>(resolution);
        changed = true;
    }

    DrawRowLabel(font, labels.fpsLimit, 2, layout);
    constexpr std::array<int, 4> fpsLimits{60, 120, 165, 240};
    int fpsIndex = 0;
    for (int index = 0; index < static_cast<int>(fpsLimits.size()); ++index) {
        if (fpsLimits[index] == settings.fpsLimit) fpsIndex = index;
    }
    if (GuiComboBox(SettingsControl(2, layout), text::fpsChoices, &fpsIndex)) {
        settings.fpsLimit = fpsLimits[fpsIndex];
        changed = true;
    }

    DrawRowLabel(font, labels.background, 3, layout);
    const MenuAction backgroundAction = GuiButton(SettingsControl(3, layout), text::MediaForLanguage(settings.language).title)
        ? MenuAction::OpenMediaLibrary : MenuAction::None;

    const bool previousShowFps = settings.showFps;
    GuiCheckBox(layout.Rect(config::fpsCheckBox), labels.showFps, &settings.showFps);
    changed |= settings.showFps != previousShowFps;

    const MenuAction action = GuiButton(
        CenteredButton(config::backButtonY, config::backButtonWidth, layout), labels.back)
        ? MenuAction::Back : MenuAction::None;
    return {tabAction != MenuAction::None ? tabAction :
            backgroundAction != MenuAction::None ? backgroundAction : action, changed, 0};
}

MenuResult DrawMusicSettings(Font font, app::Settings& settings,
                             const text::Labels& labels, const UiLayout& layout) {
    DrawCenteredText(font, labels.music, config::pageTitleY, config::pageTitleSize,
                     config::textColor, layout);
    const MenuAction tabAction = GuiButton(layout.Rect({330, 160, 140, 40}), labels.basic)
        ? MenuAction::OpenSettings : GuiButton(layout.Rect({490, 160, 140, 40}), labels.music)
        ? MenuAction::OpenMusicSettings : MenuAction::None;
    DrawRowLabel(font, labels.music, 0, layout);
    const MenuAction importAction = GuiButton(SettingsControl(0, layout), text::MediaForLanguage(settings.language).title)
        ? MenuAction::OpenMediaLibrary : MenuAction::None;
    DrawRowLabel(font, labels.musicVolume, 1, layout);
    const float previousVolume = settings.musicVolume;
    const bool playerBackdrop = GuiButton(SettingsControl(2, layout),
        settings.language == app::Language::Chinese ? "播放器背景" : "Player backdrop");
    char volumeText[8]{};
    std::snprintf(volumeText, sizeof(volumeText), "%d%%",
                  static_cast<int>(settings.musicVolume * 100.0f + 0.5f));
    GuiSlider(SettingsControl(1, layout), nullptr, nullptr, &settings.musicVolume, 0.0f, 1.0f);
    DrawScaledText(font, volumeText, config::settingsControlX + 108,
                   config::settingsRowY + config::settingsRowGap + 10, 18, config::textColor, layout);
    const MenuAction action = GuiButton(
        CenteredButton(config::backButtonY, config::backButtonWidth, layout), labels.back)
        ? MenuAction::Back : MenuAction::None;
    return {tabAction != MenuAction::None ? tabAction : playerBackdrop ? MenuAction::OpenMediaLibrary : importAction != MenuAction::None ? importAction : action,
            settings.musicVolume != previousVolume, playerBackdrop ? 1 : 2};
}

}  // namespace

MenuResult DrawMenu(app::Screen screen, Font font, app::Settings& settings) {
    const UiLayout layout = CurrentLayout();
    const text::Labels& labels = text::ForLanguage(settings.language);
    if (screen == app::Screen::GameSelection) DrawBackground(layout, false, selection::panel);
    else DrawBackground(layout);
    switch (screen) {
        case app::Screen::MainMenu: return DrawMainMenu(font, labels, layout);
        case app::Screen::GameSelection:
            return DrawGameSelection(font, settings.language, layout);
        case app::Screen::Settings: return DrawSettings(font, settings, labels, layout);
        case app::Screen::MediaLibrary: break;
        case app::Screen::MusicSettings: return DrawMusicSettings(font, settings, labels, layout);
        case app::Screen::Fruit: break;
        case app::Screen::Snake: break;
        case app::Screen::Tetris: break;
        case app::Screen::Breakout: break;
        case app::Screen::Minesweeper: break;
        case app::Screen::MinesweeperSetup: break;
        case app::Screen::Gomoku: break;
        case app::Screen::GomokuSetup: break;
    }
    return {};
}

bool UpdateMusicPlayerInput() {
    return musicPlayerInput.Update(CurrentLayout(), GetMousePosition(),
                                   IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
                                   IsMouseButtonDown(MOUSE_BUTTON_LEFT));
}

MenuResult DrawMusicPlayer(Font font, app::Settings& settings) {
    const UiLayout layout = CurrentLayout();
    const auto& labels = text::ForLanguage(settings.language);
    if (musicPlayerInput.Collapsed()) {
        const Rectangle tab = layout.Rect(musicPlayerInput.VisibleBounds());
        DrawRectangleRounded(tab, playerConfig::cornerRoundness, playerConfig::cornerSegments, config::baseNormal);
        DrawRectangleRoundedLinesEx(tab, playerConfig::cornerRoundness, playerConfig::cornerSegments, layout.scale, config::accentColor);
        const Vector2 center{tab.x + tab.width / 2, tab.y + tab.height / 2};
        Vector2 direction{0, -1};
        if (musicPlayerInput.Dock() == PlayerDock::Top) direction = {0, 1};
        else if (musicPlayerInput.Dock() == PlayerDock::Left) direction = {1, 0};
        else if (musicPlayerInput.Dock() == PlayerDock::Right) direction = {-1, 0};
        const Vector2 tangent{-direction.y, direction.x};
        const float size = playerConfig::arrowSize * layout.scale;
        const Vector2 tip{center.x + direction.x * size, center.y + direction.y * size};
        DrawLineEx({center.x - direction.x * size + tangent.x * size, center.y - direction.y * size + tangent.y * size}, tip, 2 * layout.scale, config::textColor);
        DrawLineEx(tip, {center.x - direction.x * size - tangent.x * size, center.y - direction.y * size - tangent.y * size}, 2 * layout.scale, config::textColor);
        return {};
    }
    const Rectangle design = musicPlayerInput.DesignBounds();
    const Rectangle panel = layout.Rect(design);
    DrawPlayerBackground(panel);
    DrawRectangleRounded(panel, 0.08f, 8, ColorAlpha(BLACK, 0.30f));
    DrawRectangleRoundedLinesEx(panel, 0.08f, 8, 1.0f * layout.scale, config::borderNormal);
    DrawScaledText(font, labels.music, design.x + 12, design.y + 8,
                   15, config::textColor, layout);
    const bool playerWasLocked = GuiIsLocked();
    if (musicPlayerInput.SuppressControls()) GuiLock();
    const float x = design.x + 8;
    const float width = (design.width - 32) / 3.0f;
    MenuAction action = MenuAction::None;
    if (GuiButton(layout.Rect({x, design.y + 42, width, 30}), labels.previousMusic)) {
        action = MenuAction::PreviousMusic;
    }
    if (GuiButton(layout.Rect({x + width + 8, design.y + 42, width, 30}), labels.playPause)) {
        action = MenuAction::ToggleMusic;
    }
    if (GuiButton(layout.Rect({x + 2 * (width + 8), design.y + 42, width, 30}), labels.nextMusic)) {
        action = MenuAction::NextMusic;
    }
    char volumeText[8]{};
    std::snprintf(volumeText, sizeof(volumeText), "%d%%",
                  static_cast<int>(settings.musicVolume * 100.0f + 0.5f));
    const float previousVolume = settings.musicVolume;
    GuiSlider(layout.Rect({x, design.y + 88, design.width - 16, 24}), nullptr, nullptr,
              &settings.musicVolume, 0.0f, 1.0f);
    DrawScaledText(font, volumeText, design.x + design.width / 2 - 14, design.y + 90,
                   15, config::textColor, layout);
    if (!playerWasLocked) GuiUnlock();
    return {action, settings.musicVolume != previousVolume};
}

}  // namespace ui
