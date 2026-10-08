#include "games/minesweeper/MinesweeperScreen.h"

#include <cstdio>
#include <string>

#include "config/UiConfig.h"
#include "games/minesweeper/MinesweeperConfig.h"
#include "games/minesweeper/MinesweeperLayout.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"

namespace games::minesweeper {
namespace {

std::string TimeText(int milliseconds) {
    char text[32];
    std::snprintf(text, sizeof(text), "%.1f", milliseconds /
                  static_cast<double>(rules::millisecondsPerSecond));
    return text;
}

void DrawFlag(Rectangle bounds, const ui::UiLayout& layout, float cellScale) {
    const float margin = config::symbolMargin * layout.scale * cellScale;
    const Vector2 top{bounds.x + margin, bounds.y + margin};
    const Vector2 bottom{top.x, bounds.y + bounds.height - margin};
    DrawLineEx(top, bottom, config::symbolStroke * layout.scale, ui::config::textColor);
    DrawTriangle(top, {top.x, top.y + margin}, {bounds.x + bounds.width - margin, top.y},
                 config::flagColor);
    DrawLineEx({top.x - margin / 2.0f, bottom.y}, {top.x + margin / 2.0f, bottom.y},
               config::symbolStroke * layout.scale, ui::config::textColor);
}

void DrawCell(const Cell& cell, int x, int y, const BoardLayout& boardLayout, bool showMines, bool active,
              Font font, const ui::UiLayout& layout) {
    const Rectangle bounds = layout.Rect(boardLayout.Cell(x, y));
    const float cellScale = boardLayout.cellSize / config::cellSize;
    const float inset = config::cellInset * layout.scale;
    const Color color = cell.revealed ? config::revealedColor :
        active && CheckCollisionPointRec(GetMousePosition(), bounds) ?
            config::hoveredColor : config::hiddenColor;
    DrawRectangleRec({bounds.x + inset, bounds.y + inset,
                      bounds.width - 2.0f * inset, bounds.height - 2.0f * inset}, color);
    if (cell.mine && (cell.revealed || showMines)) {
        DrawCircleV({bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f},
                    config::mineRadius * layout.scale * cellScale, config::mineColor);
    } else if (cell.flagged) {
        DrawFlag(bounds, layout, cellScale);
        if (showMines && !cell.mine) {
            const float margin = config::symbolMargin * layout.scale * cellScale;
            DrawLineEx({bounds.x + margin, bounds.y + margin},
                       {bounds.x + bounds.width - margin, bounds.y + bounds.height - margin},
                       config::symbolStroke * layout.scale, config::mineColor);
        }
    } else if (cell.revealed && cell.adjacentMines > 0) {
        const std::string number = std::to_string(cell.adjacentMines);
        const float size = config::numberSize * layout.scale * cellScale;
        const float spacing = ui::config::textSpacing * layout.scale;
        const Vector2 measured = MeasureTextEx(font, number.c_str(), size, spacing);
        DrawTextEx(font, number.c_str(),
                   {bounds.x + (bounds.width - measured.x) / 2.0f,
                    bounds.y + (bounds.height - measured.y) / 2.0f}, size, spacing,
                   config::numberColors[cell.adjacentMines - 1]);
    }
}

}  // namespace

void UpdateFromInput(MinesweeperGame& game, float elapsedSeconds, bool mouseCaptured) {
    if (IsKeyPressed(KEY_R)) { game.Reset(); return; }
    if (IsKeyPressed(KEY_SPACE) && game.State() == Status::Ready) {
        game.Start();
        return;
    }
    if (IsKeyPressed(KEY_P)) { game.TogglePause(); return; }
    game.Update(elapsedSeconds);
    if (game.State() != Status::Playing || mouseCaptured) return;
    const ui::UiLayout layout = ui::CurrentLayout();
    const auto boardLayout = LayoutFor(game);
    const Rectangle board = layout.Rect(boardLayout.bounds);
    const Vector2 mouse = GetMousePosition();
    // Reject points at the right/bottom edge before converting to an array index.
    if (mouse.x < board.x || mouse.y < board.y ||
        mouse.x >= board.x + board.width || mouse.y >= board.y + board.height) return;
    const int x = static_cast<int>((mouse.x - board.x) / (boardLayout.cellSize * layout.scale));
    const int y = static_cast<int>((mouse.y - board.y) / (boardLayout.cellSize * layout.scale));
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) game.ToggleFlag(x, y);
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (game.At(x, y).revealed) game.RevealNeighbors(x, y);
        else game.Reveal(x, y);
    }
}

void DrawScreen(const MinesweeperGame& game, Font font, app::Language language,
                int bestMilliseconds) {
    const ui::UiLayout layout = ui::CurrentLayout();
    const auto& labels = ui::text::MinesweeperForLanguage(language);
    const auto boardLayout = LayoutFor(game);
    ui::DrawBackground(layout, false, config::gamePanel);
    const std::string title = std::string(labels.name) + " - " +
        ui::text::MinesweeperSetupForLanguage(language).names[static_cast<int>(game.Level())];
    ui::DrawCenteredText(font, title.c_str(), config::titleY, config::titleSize,
                         ui::config::textColor, layout);
    const bool custom = game.Level() == Difficulty::Custom;
    const std::string stats = std::string(labels.mines) + ": " +
        std::to_string(game.RemainingMines()) + "    " + labels.time + ": " +
        TimeText(game.ElapsedMilliseconds()) + "    " + labels.best + ": " +
        (custom ? ui::text::MinesweeperSetupForLanguage(language).noCustomRecord :
         bestMilliseconds > 0 ? TimeText(bestMilliseconds) : labels.noRecord);
    ui::DrawCenteredText(font, stats.c_str(), config::statsY, config::statsSize,
                         ui::config::mutedColor, layout);
    for (int y = 0; y < game.Rows(); ++y) {
        for (int x = 0; x < game.Columns(); ++x) {
            DrawCell(game.At(x, y), x, y, boardLayout, game.State() == Status::GameOver,
                     game.State() == Status::Playing, font, layout);
        }
    }
    if (game.State() == Status::Ready || game.State() == Status::Paused) {
        DrawRectangleRec(layout.Rect(boardLayout.bounds),
            config::overlayColor);
        const bool ready = game.State() == Status::Ready;
        ui::DrawCenteredText(font, ready ? labels.ready : labels.paused,
                             boardLayout.bounds.y + boardLayout.bounds.height / 2 + config::overlayTitleOffset, config::overlayTitleSize,
                             ui::config::textColor, layout);
        ui::DrawCenteredText(font, ready ? labels.startHint : labels.resumeHint,
                             boardLayout.bounds.y + boardLayout.bounds.height / 2 + config::overlayHintOffset, config::overlayHintSize,
                             ui::config::mutedColor, layout);
    } else {
        const char* hint = game.State() == Status::Won ? labels.won :
            game.State() == Status::GameOver ? labels.gameOver : labels.safeHint;
        ui::DrawCenteredText(font, hint, config::outcomeY, config::outcomeSize,
                             ui::config::accentColor, layout);
    }
    ui::DrawCenteredText(font, labels.controlsFirst, config::footerFirstY,
                         config::footerSize, ui::config::mutedColor, layout);
    ui::DrawCenteredText(font, labels.controlsSecond, config::footerSecondY,
                         config::footerSize, ui::config::mutedColor, layout);
}

}  // namespace games::minesweeper
