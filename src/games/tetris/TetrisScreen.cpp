#include "games/tetris/TetrisScreen.h"

#include <string>

#include "config/UiConfig.h"
#include "games/tetris/TetrisConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"

namespace games::tetris {
namespace {

Rectangle BoardBounds() {
    return {static_cast<float>(config::boardX), static_cast<float>(config::boardY),
            static_cast<float>(rules::columns * config::cellSize),
            static_cast<float>(rules::rows * config::cellSize)};
}

void DrawCell(int x, int y, int originX, int originY, int cellSize,
              Color color, const ui::UiLayout& layout) {
    const float inset = config::cellInset;
    DrawRectangleRec(layout.Rect(Rectangle{
        static_cast<float>(originX + x * cellSize) + inset,
        static_cast<float>(originY + y * cellSize) + inset,
        cellSize - 2.0f * inset, cellSize - 2.0f * inset}), color);
}

void DrawBoard(const TetrisGame& game, const ui::UiLayout& layout) {
    DrawRectangleRec(layout.Rect(BoardBounds()), config::boardColor);
    for (int column = 1; column < rules::columns; ++column) {
        const float x = config::boardX + column * config::cellSize;
        DrawLineV(layout.Point(x, config::boardY),
                  layout.Point(x, config::boardY + rules::rows * config::cellSize),
                  config::gridColor);
    }
    for (int row = 1; row < rules::rows; ++row) {
        const float y = config::boardY + row * config::cellSize;
        DrawLineV(layout.Point(config::boardX, y),
                  layout.Point(config::boardX + rules::columns * config::cellSize, y),
                  config::gridColor);
    }

    for (int y = 0; y < rules::rows; ++y) {
        for (int x = 0; x < rules::columns; ++x) {
            const int piece = game.LockedBoard()[y][x];
            if (piece != 0) {
                DrawCell(x, y, config::boardX, config::boardY, config::cellSize,
                         config::pieceColors[piece - 1], layout);
            }
        }
    }
    for (const Cell cell : game.ActiveCells()) {
        if (cell.y >= 0) {
            DrawCell(cell.x, cell.y, config::boardX, config::boardY,
                     config::cellSize,
                     config::pieceColors[static_cast<int>(game.Active().type)], layout);
        }
    }
    DrawRectangleLinesEx(layout.Rect(BoardBounds()), 2.0f * layout.scale,
                         ui::config::accentColor);
}

void DrawBoardCenteredText(Font font, const char* text, float y, float size,
                           Color color, const ui::UiLayout& layout) {
    const float fontSize = size * layout.scale;
    const float spacing = ui::config::textSpacing * layout.scale;
    const Vector2 measured = MeasureTextEx(font, text, fontSize, spacing);
    const float boardCenter = config::boardX + rules::columns * config::cellSize / 2.0f;
    const Vector2 center = layout.Point(boardCenter, y);
    DrawTextEx(font, text, Vector2{center.x - measured.x / 2.0f, center.y},
               fontSize, spacing, color);
}

void DrawOverlay(const TetrisGame& game, Font font,
                 const ui::text::TetrisLabels& labels, const ui::UiLayout& layout) {
    if (game.State() == Status::Playing) return;
    DrawRectangleRec(layout.Rect(BoardBounds()), config::overlayColor);
    const char* title = nullptr;
    const char* hint = nullptr;
    switch (game.State()) {
        case Status::Ready:
            title = labels.ready;
            hint = labels.startHint;
            break;
        case Status::Paused:
            title = labels.paused;
            hint = labels.resumeHint;
            break;
        case Status::GameOver:
            title = labels.gameOver;
            hint = labels.restartHint;
            break;
        case Status::Playing: break;
    }
    DrawBoardCenteredText(font, title, config::overlayTitleY,
                          config::overlayTitleSize, ui::config::textColor, layout);
    DrawBoardCenteredText(font, hint, config::overlayHintY,
                          config::overlayHintSize, ui::config::mutedColor, layout);
}

void DrawStat(Font font, const char* label, int value, int y,
              const ui::UiLayout& layout) {
    ui::DrawScaledText(font, label, config::sideX, y, config::statLabelSize,
                       ui::config::mutedColor, layout);
    const std::string number = std::to_string(value);
    ui::DrawScaledText(font, number.c_str(), config::sideX,
                       y + config::statValueOffsetY, config::statValueSize,
                       ui::config::textColor, layout);
}

void DrawNextPiece(PieceType piece, Font font,
                   const ui::text::TetrisLabels& labels, const ui::UiLayout& layout) {
    ui::DrawScaledText(font, labels.next, config::sideX, config::nextLabelY,
                       config::statLabelSize, ui::config::mutedColor, layout);
    for (const Cell cell : TetrisGame::ShapeCells(piece, 0)) {
        DrawCell(cell.x, cell.y, config::previewX, config::previewY,
                 config::previewCellSize,
                 config::pieceColors[static_cast<int>(piece)], layout);
    }
}

}  // namespace

void UpdateFromInput(TetrisGame& game, float elapsedSeconds) {
    if (IsKeyPressed(KEY_R)) game.Reset();
    if (IsKeyPressed(KEY_ENTER)) game.Start();
    if (IsKeyPressed(KEY_P)) game.TogglePause();

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT) ||
        IsKeyPressed(KEY_A) || IsKeyPressedRepeat(KEY_A)) {
        game.MoveHorizontal(HorizontalMove::Left);
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT) ||
               IsKeyPressed(KEY_D) || IsKeyPressedRepeat(KEY_D)) {
        game.MoveHorizontal(HorizontalMove::Right);
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_W)) {
        game.Rotate(Turn::Clockwise);
    } else if (IsKeyPressed(KEY_Z)) {
        game.Rotate(Turn::Counterclockwise);
    }

    if (IsKeyPressed(KEY_SPACE)) game.HardDrop();
    else game.Update(elapsedSeconds, IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S));
}

void DrawScreen(const TetrisGame& game, Font font, app::Language language,
                int bestScore) {
    const ui::UiLayout layout = ui::CurrentLayout();
    const ui::text::TetrisLabels& labels = ui::text::TetrisForLanguage(language);
    ui::DrawBackground(layout, false);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize,
                         ui::config::textColor, layout);
    DrawBoard(game, layout);
    DrawOverlay(game, font, labels, layout);
    DrawNextPiece(game.NextPiece(), font, labels, layout);
    DrawStat(font, labels.score, game.Score(), config::scoreLabelY, layout);
    DrawStat(font, labels.best, bestScore, config::bestLabelY, layout);
    DrawStat(font, labels.lines, game.Lines(), config::linesLabelY, layout);
    DrawStat(font, labels.level, game.Level(), config::levelLabelY, layout);
    ui::DrawCenteredText(font, labels.controlsFirst, config::footerFirstY,
                         config::footerSize, ui::config::mutedColor, layout);
    ui::DrawCenteredText(font, labels.controlsSecond, config::footerSecondY,
                         config::footerSize, ui::config::mutedColor, layout);
}

}  // namespace games::tetris
