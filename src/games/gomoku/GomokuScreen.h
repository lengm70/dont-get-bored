#pragma once
#include <optional>
#include "games/gomoku/GomokuAiTurn.h"
#include "ui/MenuView.h"
#include "ui/UiLayout.h"
namespace games::gomoku {
struct SetupOptions { int mode = 0; int strength = 1; };
std::optional<Move> MoveFromPoint(Vector2 point, const ui::UiLayout& layout);
void UpdateFromInput(GomokuGame& game, GomokuAiTurn& ai, Strength strength, bool mouseCaptured = false);
void DrawScreen(const GomokuGame& game, Font font, app::Language language, bool thinking);
ui::MenuResult DrawSetup(Font font, app::Language language, SetupOptions& options);
}  // namespace games::gomoku
