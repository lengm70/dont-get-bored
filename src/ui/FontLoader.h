#pragma once

#include "raylib.h"
#include <string>
#include <vector>

namespace ui {

// The caller owns the returned font and must call UnloadFont after a valid load.
Font LoadMenuFont(const std::vector<std::string>& additionalText = {});

}  // namespace ui
