#include "ui/MenuView.h"

#include <array>

#include "raygui.h"
#include "config/UiConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"

namespace ui {
namespace {

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

MenuResult DrawGameSelection(Font font, const text::Labels& labels,
                             app::Language language,
                             const UiLayout& layout) {
    DrawCenteredText(font, labels.selectGame, config::pageTitleY,
                     config::pageTitleSize, config::textColor, layout);
    struct Entry { const char* name; MenuAction action; };
    const std::array<Entry, 4> entries{{
        {text::SnakeForLanguage(language).name, MenuAction::StartSnake},
        {text::TetrisForLanguage(language).name, MenuAction::StartTetris},
        {text::BreakoutForLanguage(language).name, MenuAction::StartBreakout},
        {text::MinesweeperForLanguage(language).name, MenuAction::StartMinesweeper}
    }};
    constexpr int gridWidth = config::gameButtonColumns * config::gameButtonWidth +
        (config::gameButtonColumns - 1) * config::gameButtonGap;
    for (int index = 0; index < static_cast<int>(entries.size()); ++index) {
        const int x = (config::designWidth - gridWidth) / 2 +
            (index % config::gameButtonColumns) * (config::gameButtonWidth + config::gameButtonGap);
        const int y = config::gameButtonY + (index / config::gameButtonColumns) * config::gameButtonRowGap;
        if (GuiButton(layout.Rect({static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(config::gameButtonWidth),
                                  static_cast<float>(config::buttonHeight)}), entries[index].name)) {
            return {entries[index].action};
        }
    }
    if (GuiButton(CenteredButton(config::backButtonY, config::backButtonWidth, layout),
                  labels.back)) return {MenuAction::Back};
    return {};
}

MenuResult DrawSettings(Font font, app::Settings& settings,
                        const text::Labels& labels, const UiLayout& layout) {
    DrawCenteredText(font, labels.settings, config::pageTitleY,
                     config::pageTitleSize, config::textColor, layout);

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

    const bool previousShowFps = settings.showFps;
    GuiCheckBox(layout.Rect(config::fpsCheckBox), labels.showFps, &settings.showFps);
    changed |= settings.showFps != previousShowFps;

    const MenuAction action = GuiButton(
        CenteredButton(config::backButtonY, config::backButtonWidth, layout), labels.back)
        ? MenuAction::Back : MenuAction::None;
    return {action, changed};
}

}  // namespace

MenuResult DrawMenu(app::Screen screen, Font font, app::Settings& settings) {
    const UiLayout layout = CurrentLayout();
    const text::Labels& labels = text::ForLanguage(settings.language);
    DrawBackground(layout);
    switch (screen) {
        case app::Screen::MainMenu: return DrawMainMenu(font, labels, layout);
        case app::Screen::GameSelection:
            return DrawGameSelection(font, labels, settings.language, layout);
        case app::Screen::Settings: return DrawSettings(font, settings, labels, layout);
        case app::Screen::Snake: break;
        case app::Screen::Tetris: break;
        case app::Screen::Breakout: break;
        case app::Screen::Minesweeper: break;
    }
    return {};
}

}  // namespace ui
