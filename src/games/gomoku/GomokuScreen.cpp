#include "games/gomoku/GomokuScreen.h"
#include <cmath>
#include "raygui.h"
#include "games/gomoku/GomokuConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiText.h"

namespace games::gomoku {
namespace {
Vector2 Intersection(Move move, const ui::UiLayout& layout) {
    return layout.Point(config::boardX + move.x * config::spacing,
                        config::boardY + move.y * config::spacing);
}
void DrawStone(Move move, Stone stone, const ui::UiLayout& layout, float opacity = 1) {
    const Vector2 position = Intersection(move, layout);
    DrawCircleV(position, config::stoneRadius * layout.scale,
                Fade(stone == Stone::Black ? config::black : config::white, opacity));
    DrawCircleLinesV(position, config::stoneRadius * layout.scale,
                     Fade(ui::palette::accent, opacity));
}
}

std::optional<Move> MoveFromPoint(Vector2 point, const ui::UiLayout& layout) {
    const Vector2 origin = layout.Point(config::boardX, config::boardY);
    const float spacing = config::spacing * layout.scale;
    if (spacing <= 0) return std::nullopt;
    const Move move{static_cast<int>(std::floor((point.x - origin.x) / spacing + 0.5f)),
                    static_cast<int>(std::floor((point.y - origin.y) / spacing + 0.5f))};
    return InBounds(move.x, move.y) ? std::optional<Move>(move) : std::nullopt;
}

void UpdateFromInput(GomokuGame& game, GomokuAiTurn& ai, Strength strength, bool mouseCaptured) {
    if (IsKeyPressed(KEY_R)) { ai.Cancel(); game.Reset(); return; }
    if (IsKeyPressed(KEY_SPACE) && game.State() == Status::Ready) { game.Start(); return; }
    if (!mouseCaptured && game.HumanTurn() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (const auto move = MoveFromPoint(GetMousePosition(), ui::CurrentLayout())) game.Place(*move);
    }
    ai.Update(game, strength);
}

ui::MenuResult DrawSetup(Font font, app::Language language, SetupOptions& options) {
    const auto layout = ui::CurrentLayout();
    const auto& labels = ui::text::GomokuForLanguage(language);
    ui::DrawBackground(layout);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize, ui::palette::text, layout);
    ui::DrawScaledText(font, labels.mode, config::setupLabelX, config::modeY, config::statusSize, ui::palette::text, layout);
    GuiComboBox(layout.Rect({config::setupControlX, config::modeY, config::setupControlWidth, config::controlHeight}), labels.modes, &options.mode);
    if (options.mode == static_cast<int>(Mode::Computer)) {
        ui::DrawScaledText(font, labels.difficulty, config::setupLabelX, config::strengthY, config::statusSize, ui::palette::text, layout);
        GuiComboBox(layout.Rect({config::setupControlX, config::strengthY, config::setupControlWidth, config::controlHeight}), labels.difficulties, &options.strength);
    }
    ui::DrawCenteredText(font, options.mode == 1 ? labels.humanBlack : labels.twoPlayers,
                         config::setupHintY, config::hintSize, ui::palette::muted, layout);
    ui::DrawCenteredText(font, labels.rules, config::rulesHintY, config::hintSize, ui::palette::muted, layout);
    if (GuiButton(layout.Rect(config::startButton), labels.start) || IsKeyPressed(KEY_SPACE)) return {ui::MenuAction::LaunchGomoku};
    if (GuiButton(layout.Rect(config::backButton), ui::text::ForLanguage(language).back)) return {ui::MenuAction::Back};
    return {};
}

void DrawScreen(const GomokuGame& game, Font font, app::Language language, bool thinking) {
    const auto layout = ui::CurrentLayout();
    const auto& labels = ui::text::GomokuForLanguage(language);
    ui::DrawBackground(layout);
    ui::DrawCenteredText(font, labels.name, config::titleY, config::titleSize, ui::palette::text, layout);
    const char* status = game.Turn() == Stone::Black ? labels.blackTurn : labels.whiteTurn;
    if (game.State() == Status::Ready) status = labels.ready;
    else if (game.State() == Status::Draw) status = labels.draw;
    else if (game.State() == Status::Won) status = game.Winner() == Stone::Black ? labels.blackWins : labels.whiteWins;
    else if (thinking) status = labels.thinking;
    ui::DrawCenteredText(font, status, config::statusY, config::statusSize, ui::palette::accent, layout);
    DrawRectangleRounded(layout.Rect(config::boardPanel), 0.03f, config::circleSegments, ui::palette::board);
    for (int index = 0; index < rules::size; ++index) {
        DrawLineEx(Intersection({index, 0}, layout), Intersection({index, rules::size - 1}, layout), layout.scale, config::grid);
        DrawLineEx(Intersection({0, index}, layout), Intersection({rules::size - 1, index}, layout), layout.scale, config::grid);
    }
    constexpr Move stars[]{{3, 3}, {11, 3}, {7, 7}, {3, 11}, {11, 11}};
    for (Move move : stars) DrawCircleV(Intersection(move, layout), config::markerRadius * layout.scale, config::grid);
    for (int y = 0; y < rules::size; ++y) for (int x = 0; x < rules::size; ++x) {
        if (game.At(x, y) != Stone::Empty) DrawStone({x, y}, game.At(x, y), layout);
    }
    if (const auto last = game.LastMove()) DrawCircleV(Intersection(*last, layout), config::markerRadius * layout.scale, ui::palette::accent);
    if (!game.WinningLine().empty()) DrawLineEx(Intersection(game.WinningLine().front(), layout), Intersection(game.WinningLine().back(), layout), 2 * layout.scale, ui::palette::accent);
    if (game.HumanTurn()) {
        if (const auto move = MoveFromPoint(GetMousePosition(), layout); move && game.At(move->x, move->y) == Stone::Empty) DrawStone(*move, game.Turn(), layout, 0.35f);
    }
    ui::DrawCenteredText(font, labels.controls, config::controlsY, config::controlsSize, ui::palette::muted, layout);
}
}  // namespace games::gomoku
