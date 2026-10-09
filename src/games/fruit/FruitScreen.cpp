#include "games/fruit/FruitScreen.h"
#include <cmath>
#include <algorithm>
#include <string>
#include "games/fruit/FruitConfig.h"
#include "games/fruit/FruitVisual.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiText.h"
namespace games::fruit {
namespace {
void DrawFruit(Point position, int level, float radius, const ui::UiLayout& layout,
               unsigned char alpha = 255, float angle = 0, float impact = 0) {
    DrawFusionOrb(layout.Point(position.x, position.y), radius * layout.scale, level,
                  angle, impact, layout.scale, alpha);
}
}
void UpdateFromInput(FruitGame& game, float seconds, bool mouseCaptured) {
    if (IsKeyPressed(KEY_R)) { game.Reset(); return; }
    if (IsKeyPressed(KEY_SPACE) && game.State() == Status::Ready) { game.Start(); return; }
    if (IsKeyPressed(KEY_P)) { game.TogglePause(); return; }
    if (game.State() != Status::Playing) return;
    const auto layout = ui::CurrentLayout();
    const auto mouse = GetMousePosition();
    const Rectangle board = layout.Rect({config::boardX, config::boardY, rules::width, rules::height});
    const Vector2 mouseDelta = GetMouseDelta();
    if (!mouseCaptured && (mouseDelta.x != 0 || mouseDelta.y != 0) && CheckCollisionPointRec(mouse, board))
        game.SetAim((mouse.x - board.x) / layout.scale);
    const float move = static_cast<float>(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) -
        static_cast<float>(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    if (move != 0) game.SetAim(game.Aim() + move * rules::aimSpeed * std::min(seconds, rules::maxFrameTime));
    if (IsKeyPressed(KEY_SPACE) || (!mouseCaptured && CheckCollisionPointRec(mouse, board) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) game.Drop();
    game.Update(seconds);
}
void DrawScreen(const FruitGame& game, Font font, app::Language language, int bestScore) {
    const auto layout = ui::CurrentLayout();
    const auto& labels = ui::text::FruitForLanguage(language);
    ui::DrawBackground(layout, false, config::panel);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize, ui::palette::text, layout);
    const std::string stats = std::string(labels.score) + ": " + std::to_string(game.Score()) +
        "    " + labels.best + ": " + std::to_string(bestScore);
    ui::DrawCenteredText(font, stats.c_str(), config::statsY, config::statsSize, ui::palette::muted, layout);
    const Rectangle board = layout.Rect({config::boardX, config::boardY, rules::width, rules::height});
    DrawRectangleRec(board, ui::palette::board);
    BeginScissorMode(static_cast<int>(board.x), static_cast<int>(board.y), static_cast<int>(board.width), static_cast<int>(board.height));
    for (const auto& fruit : game.Fruits()) DrawFruit({config::boardX + fruit.position.x, config::boardY + fruit.position.y}, fruit.level, rules::radii[fruit.level], layout, 255, fruit.angle, fruit.impact);
    const float dangerY = config::boardY + rules::dangerY;
    const Color danger = game.DangerProgress() > 0 ? config::colors[0] : ui::palette::muted;
    for (float x = 0; x < rules::width; x += 16) DrawLineEx(layout.Point(config::boardX + x, dangerY),
        layout.Point(config::boardX + x + 8, dangerY), layout.scale, danger);
    EndScissorMode();
    DrawRectangleLinesEx(board, config::outlineWidth * layout.scale, ui::palette::accent);
    DrawFruit({config::boardX + game.Aim(), config::previewY}, game.CurrentLevel(), rules::radii[game.CurrentLevel()], layout, game.CanDrop() ? 255 : 120);
    ui::DrawScaledText(font, labels.next, config::nextX - 22, config::nextY - 50, config::statsSize, ui::palette::muted, layout);
    DrawFruit({config::nextX, config::nextY}, game.NextLevel(), rules::radii[game.NextLevel()], layout);
    if (game.DangerProgress() > 0) ui::DrawScaledText(font, labels.danger, config::nextX - 38, config::nextY + 52, config::hintSize, config::colors[0], layout);
    ui::DrawCenteredText(font, labels.controls, config::controlsY, config::hintSize, ui::palette::muted, layout);
    if (game.State() != Status::Playing) {
        DrawRectangleRec(board, ui::palette::overlay);
        const char* title = labels.ready; const char* hint = labels.start;
        if (game.State() == Status::Paused) { title = labels.paused; hint = labels.resume; }
        else if (game.State() == Status::GameOver) { title = labels.gameOver; hint = labels.restart; }
        else if (game.State() == Status::Won) { title = labels.won; hint = labels.restart; }
        ui::DrawCenteredText(font, title, config::overlayY, 26, ui::palette::text, layout);
        ui::DrawCenteredText(font, hint, config::overlayHintY, 18, ui::palette::muted, layout);
    }
}
}  // namespace games::fruit
