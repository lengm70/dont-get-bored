#pragma once
#include "raylib.h"
namespace games::fruit {
// Circular energy envelope matches the collider; rotating polygons show orientation.
void DrawFusionOrb(Vector2 center, float radius, int level, float angle = 0,
                   float impact = 0, float scale = 1, unsigned char alpha = 255);
}  // namespace games::fruit
