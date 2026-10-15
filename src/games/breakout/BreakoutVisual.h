#pragma once
#include "raylib.h"
#include "ui/UiLayout.h"
#include "games/breakout/BreakoutGame.h"
namespace games::breakout {
ui::UiLayout ImpactLayout(const BreakoutGame& game, ui::UiLayout layout, bool shake);
void DrawPlayfield(const BreakoutGame& game, const ui::UiLayout& layout);
} // namespace games::breakout
