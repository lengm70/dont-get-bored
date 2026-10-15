#pragma once
#include "raylib.h"
#include "config/AppSettings.h"
#include "games/bomber/BomberGame.h"
namespace games::bomber {
struct ViewState {
    Options options;
    std::array<Direction,2> held{};
    bool setup = true;
};
void UpdateFromInput(BomberGame& game, ViewState& view, float elapsedSeconds);
void DrawScreen(BomberGame& game, ViewState& view, Font font, app::Language language, int bestScore);
} // namespace games::bomber
