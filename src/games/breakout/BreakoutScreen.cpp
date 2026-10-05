#include "games/breakout/BreakoutScreen.h"

#include <string>

#include "config/UiConfig.h"
#include "games/breakout/BreakoutConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"

namespace games::breakout {
namespace {

Rectangle ToScreen(Bounds bounds, const ui::UiLayout& layout) {
    return layout.Rect({config::boardX + bounds.x, config::boardY + bounds.y,
                        bounds.width, bounds.height});
}

void DrawOverlay(const BreakoutGame& game, Font font,
                 const ui::text::BreakoutLabels& labels, const ui::UiLayout& layout) {
    if (game.State() == Status::Playing) return;
    const char* title = labels.ready;
    const char* hint = labels.startHint;
    switch (game.State()) {
        case Status::Paused: title = labels.paused; hint = labels.resumeHint; break;
        case Status::GameOver: title = labels.gameOver; hint = labels.restartHint; break;
        case Status::Won: title = labels.won; hint = labels.restartHint; break;
        case Status::Ready: break;
        case Status::Playing: break;
    }
    DrawRectangleRec(ToScreen({0, 0, rules::boardWidth, rules::boardHeight}, layout),
                     config::overlayColor);
    ui::DrawCenteredText(font, title, config::overlayTitleY, config::overlayTitleSize,
                         ui::config::textColor, layout);
    ui::DrawCenteredText(font, hint, config::overlayHintY, config::overlayHintSize,
                         ui::config::mutedColor, layout);
}

}  // namespace

void UpdateFromInput(BreakoutGame& game, float elapsedSeconds) {
    if (IsKeyPressed(KEY_R)) { game.Reset(); return; }
    if (IsKeyPressed(KEY_SPACE) && game.State() == Status::Ready) {
        game.Start();
        return;
    }
    if (IsKeyPressed(KEY_P)) { game.TogglePause(); return; }
    const float movement = static_cast<float>(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) -
                           static_cast<float>(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    game.Update(elapsedSeconds, movement);
}

void DrawScreen(const BreakoutGame& game, Font font, app::Language language,
                int bestScore) {
    const ui::UiLayout layout = ui::CurrentLayout();
    const auto& labels = ui::text::BreakoutForLanguage(language);
    ui::DrawBackground(layout, false);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize,
                         ui::config::textColor, layout);
    const std::string stats = std::string(labels.score) + ": " + std::to_string(game.Score()) +
        "    " + labels.best + ": " + std::to_string(bestScore) +
        "    " + labels.lives + ": " + std::to_string(game.Lives());
    ui::DrawCenteredText(font, stats.c_str(), config::statsY, config::statsSize,
                         ui::config::mutedColor, layout);
    const Rectangle board = ToScreen({0, 0, rules::boardWidth, rules::boardHeight}, layout);
    DrawRectangleRec(board, config::boardColor);
    for (int index = 0; index < rules::brickCount; ++index) {
        if (game.Bricks()[index]) {
            DrawRectangleRec(ToScreen(BreakoutGame::BrickBounds(index), layout),
                             config::brickColors[index / rules::brickColumns]);
        }
    }
    DrawRectangleRec(ToScreen(game.Paddle(), layout), config::paddleColor);
    const Point ball = game.Ball();
    DrawCircleV(layout.Point(config::boardX + ball.x, config::boardY + ball.y),
                rules::ballRadius * layout.scale, config::ballColor);
    DrawRectangleLinesEx(board, config::borderWidth * layout.scale, ui::config::accentColor);
    DrawOverlay(game, font, labels, layout);
    ui::DrawCenteredText(font, labels.controlsHint, config::footerY, config::footerSize,
                         ui::config::mutedColor, layout);
}

}  // namespace games::breakout
