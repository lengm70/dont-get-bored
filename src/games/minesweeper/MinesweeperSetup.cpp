#include "games/minesweeper/MinesweeperSetup.h"

#include <array>
#include <cstdio>
#include <string>
#include "raygui.h"
#include "games/minesweeper/MinesweeperConfig.h"
#include "games/minesweeper/MinesweeperRules.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiText.h"

namespace games::minesweeper {

ui::MenuResult DrawSetup(Font font, app::Language language, SetupOptions& options,
                         const app::HighScores& records) {
    const auto layout = ui::CurrentLayout();
    const auto& text = ui::text::MinesweeperSetupForLanguage(language);
    ui::DrawBackground(layout, false);
    ui::DrawCenteredText(font, ui::text::MinesweeperForLanguage(language).name,
        config::setupTitleY, config::titleSize, ui::config::textColor, layout);
    ui::DrawCenteredText(font, text.choose, config::setupSubtitleY, config::setupTextSize,
        ui::config::mutedColor, layout);
    if (GuiComboBox(layout.Rect(config::setupSelector), text.choices, &options.selected)) {
        options.editColumns = options.editRows = options.editMines = false;
    }
    const auto level = static_cast<Difficulty>(options.selected);
    const bool custom = level == Difficulty::Custom;
    if (custom) {
        struct Row { const char* label; int* value; int minimum; int maximum; bool* editing; };
        const std::array<Row, 3> rows{{
            {text.columns, &options.custom.columns, difficulty::minDimension, difficulty::maxColumns, &options.editColumns},
            {text.rows, &options.custom.rows, difficulty::minDimension, difficulty::maxRows, &options.editRows},
            {text.mines, &options.custom.mines, 1,
             difficulty::MaximumMines(difficulty::ClampCustom(options.custom)), &options.editMines}
        }};
        for (std::size_t index = 0; index < rows.size(); ++index) {
            const float y = config::setupRowsY + index * config::setupRowGap;
            const auto& row = rows[index];
            ui::DrawScaledText(font, row.label, config::setupLabelX, y + config::setupLabelOffset,
                config::setupTextSize, ui::config::textColor, layout);
            if (GuiSpinner(layout.Rect({config::setupControlX, y, config::setupControlWidth, config::setupControlHeight}), nullptr,
                           row.value, row.minimum, row.maximum, *row.editing)) {
                *row.editing = !*row.editing;
            }
        }
        if (!options.editColumns && !options.editRows && !options.editMines) {
            options.custom = difficulty::ClampCustom(options.custom);
        }
    } else {
        const auto preset = difficulty::Preset(level);
        const std::string summary = std::to_string(preset.columns) + " x " +
            std::to_string(preset.rows) + "    " + text.mines + ": " + std::to_string(preset.mines);
        ui::DrawCenteredText(font, summary.c_str(), config::setupSummaryY, config::setupTextSize,
            ui::config::textColor, layout);
        const int record = app::MinesweeperBest(records, level);
        char seconds[32];
        std::snprintf(seconds, sizeof(seconds), "%.1f", record / static_cast<double>(rules::millisecondsPerSecond));
        const auto& gameLabels = ui::text::MinesweeperForLanguage(language);
        const std::string best = std::string(gameLabels.best) + ": " +
            (record > 0 ? seconds : gameLabels.noRecord);
        ui::DrawCenteredText(font, best.c_str(), config::setupRecordY, config::setupTextSize,
            ui::config::accentColor, layout);
    }
    ui::DrawCenteredText(font, custom ? text.customHint : text.startHint,
        config::setupHintY, config::setupHintSize, ui::config::mutedColor, layout);
    const bool editing = options.editColumns || options.editRows || options.editMines;
    const bool valid = !custom || difficulty::IsValid(options.custom);
    const bool allowStart = valid && !editing;
    const int previousState = GuiGetState();
    if (!allowStart) GuiDisable();
    const bool start = GuiButton(layout.Rect(config::setupStart), text.start);
    GuiSetState(previousState);
    if (allowStart && (start || IsKeyPressed(KEY_SPACE))) return {ui::MenuAction::LaunchMinesweeper};
    if (GuiButton(layout.Rect(config::setupBack), ui::text::ForLanguage(language).back)) {
        return {ui::MenuAction::Back};
    }
    return {};
}

}  // namespace games::minesweeper
