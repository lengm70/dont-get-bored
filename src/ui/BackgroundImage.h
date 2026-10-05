#pragma once

namespace ui {

// Load after InitWindow and unload before CloseWindow.
bool LoadBackgroundImage();
void UnloadBackgroundImage();
void DrawBackgroundImage(int width, int height);

}  // namespace ui
