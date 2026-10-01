#pragma once

#include "raylib.h"

namespace ui {

// The caller owns the returned font and must call UnloadFont after a valid load.
Font LoadMenuFont();

}  // namespace ui
