#pragma once

namespace ui {

// Call before InitWindow so Windows gives the game its own taskbar group.
bool ConfigureApplicationIdentity();

// Call after InitWindow. The image is released after raylib copies the icon.
bool ConfigureWindowIcon();

}  // namespace ui
