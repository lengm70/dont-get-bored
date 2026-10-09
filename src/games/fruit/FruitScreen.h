#pragma once
#include "raylib.h"
#include "config/AppSettings.h"
#include "games/fruit/FruitGame.h"
namespace games::fruit {
void UpdateFromInput(FruitGame& game, float elapsedSeconds, bool mouseCaptured = false);
void DrawScreen(const FruitGame& game, Font font, app::Language language, int bestScore);
}  // namespace games::fruit
