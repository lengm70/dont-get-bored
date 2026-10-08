#pragma once

#include "config/UiConfig.h"
#include <filesystem>

namespace ui {

// Load after InitWindow and unload before CloseWindow.
bool LoadBackgroundImage(const char* path = config::backgroundPath);
bool ImportBackgroundImage(const std::filesystem::path& source,
                           const std::filesystem::path& destination);
void UnloadBackgroundImage();
void DrawBackgroundImage(int width, int height);

}  // namespace ui
