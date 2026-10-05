#pragma once

#include "raylib.h"
#include "ui/UiLayout.h"

namespace ui {

void ConfigureGuiStyle(Font font, float scale);
void DrawBackground(const UiLayout& layout, bool showAccent = true,
                    Rectangle panel = config::panel);

}  // namespace ui
