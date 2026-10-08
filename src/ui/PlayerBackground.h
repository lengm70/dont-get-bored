#pragma once
#include "raylib.h"
namespace ui {
bool LoadPlayerBackground(const char* path);
void DrawPlayerBackground(Rectangle bounds);
void UnloadPlayerBackground();
}
