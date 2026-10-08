#pragma once

#include <cstddef>
#include "ui/UiPalette.h"

namespace ui {
// Pure color analysis, independent of the graphics context.
palette::Colors ExtractTheme(const Color* pixels, std::size_t count);
// Called once after a background texture is successfully loaded.
void UpdateThemeFromBackground(Texture2D texture);
}  // namespace ui
