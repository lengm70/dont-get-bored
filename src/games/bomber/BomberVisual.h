#pragma once
#include "raylib.h"
#include "ui/UiLayout.h"
#include "games/bomber/BomberGame.h"
namespace games::bomber {
void DrawArena(const BomberGame& game, const ui::UiLayout& layout);
void DrawBomberIcon(Rectangle bounds);
} // namespace games::bomber
