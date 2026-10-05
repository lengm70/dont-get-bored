#include "games/snake/SnakeScreen.h"

#include <string>

#include "config/UiConfig.h"
#include "games/snake/SnakeConfig.h"
#include "games/snake/SnakeRules.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"

namespace games::snake {
namespace {

Rectangle BoardBounds() {
    return {static_cast<float>(config::boardX), static_cast<float>(config::boardY),
            static_cast<float>(rules::columns * config::cellSize),
            static_cast<float>(rules::rows * config::cellSize)};
}

void DrawHeadEyes(Cell head, Direction heading, const ui::UiLayout& layout) {
    Vector2 forward{};
    switch (heading) {
        case Direction::Up: forward = {0, -1}; break;
        case Direction::Right: forward = {1, 0}; break;
        case Direction::Down: forward = {0, 1}; break;
        case Direction::Left: forward = {-1, 0}; break;
    }
    const Vector2 side{-forward.y, forward.x};
    const Vector2 center{
        config::boardX + (head.x + 0.5f) * config::cellSize,
        config::boardY + (head.y + 0.5f) * config::cellSize
    };
    for (const float sign : {-1.0f, 1.0f}) {
        const Vector2 eye = layout.Point(
            center.x + forward.x * config::eyeForwardOffset + sign * side.x * config::eyeSideOffset,
            center.y + forward.y * config::eyeForwardOffset + sign * side.y * config::eyeSideOffset);
        DrawCircleV(eye, config::eyeRadius * layout.scale, config::eyeColor);
    }
}

void DrawBoard(const SnakeGame& game, const ui::UiLayout& layout) {
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

    if (const auto food = game.Food()) {
        const float x = config::boardX + (food->x + 0.5f) * config::cellSize;
        const float y = config::boardY + (food->y + 0.5f) * config::cellSize;
        DrawCircleV(layout.Point(x, y), config::foodRadius * layout.scale,
                    config::foodColor);
    }

    const auto& body = game.Body();
    for (std::size_t index = 0; index < body.size(); ++index) {
        const Cell cell = body[index];
        const float x = config::boardX + cell.x * config::cellSize + config::segmentInset;
        const float y = config::boardY + cell.y * config::cellSize + config::segmentInset;
        const float side = config::cellSize - 2.0f * config::segmentInset;
        // Normalize by length so both short and long snakes have a distinct dark tail.
        const float progress = body.size() > 1
            ? static_cast<float>(index) / static_cast<float>(body.size() - 1) : 0.0f;
        DrawRectangleRounded(layout.Rect(Rectangle{x, y, side, side}),
                             config::segmentRoundness, config::segmentSegments,
                             ColorLerp(config::headColor, config::tailColor, progress));
    }
    if (!body.empty()) DrawHeadEyes(body.front(), game.Heading(), layout);

    DrawRectangleLinesEx(layout.Rect(BoardBounds()), 2.0f * layout.scale,
                         ui::config::accentColor);
}

void DrawOverlay(const SnakeGame& game, Font font,
                 const ui::text::SnakeLabels& labels, const ui::UiLayout& layout) {
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
        case Status::Won:
            title = labels.won;
            hint = labels.restartHint;
            break;
        case Status::Playing: break;
    }
    ui::DrawCenteredText(font, title, config::overlayTitleY, config::overlayTitleSize,
                         ui::config::textColor, layout);
    ui::DrawCenteredText(font, hint, config::overlayHintY, config::overlayHintSize,
                         ui::config::mutedColor, layout);
}

}  // namespace

void UpdateFromInput(SnakeGame& game, float elapsedSeconds) {
    if (IsKeyPressed(KEY_R)) game.Reset();
    if (IsKeyPressed(KEY_SPACE)) {
        if (game.State() == Status::Ready) game.Start();
        else game.TogglePause();
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) game.QueueDirection(Direction::Up);
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        game.QueueDirection(Direction::Right);
    } else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game.QueueDirection(Direction::Down);
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        game.QueueDirection(Direction::Left);
    }
    game.Update(elapsedSeconds);
}

void DrawScreen(const SnakeGame& game, Font font, app::Language language,
                int bestScore) {
    const ui::UiLayout layout = ui::CurrentLayout();
    const ui::text::SnakeLabels& labels = ui::text::SnakeForLanguage(language);
    ui::DrawBackground(layout);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize,
                         ui::config::textColor, layout);
    const std::string score = std::string(labels.score) + ": " +
                              std::to_string(game.Score()) + "     " +
                              labels.best + ": " + std::to_string(bestScore);
    ui::DrawCenteredText(font, score.c_str(), config::scoreY, config::scoreSize,
                         ui::config::mutedColor, layout);
    DrawBoard(game, layout);
    DrawOverlay(game, font, labels, layout);

    const char* pauseHint = game.State() == Status::Ready ? labels.startHint
        : game.State() == Status::Paused ? labels.resumeHint : labels.pauseHint;
    const std::string footer = std::string(labels.controlsHint) + "     " +
                               pauseHint + "     " + labels.backHint;
    ui::DrawCenteredText(font, footer.c_str(), config::footerY, config::footerSize,
                         ui::config::mutedColor, layout);
}

}  // namespace games::snake
