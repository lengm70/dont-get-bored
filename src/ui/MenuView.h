#pragma once

#include "raylib.h"
#include "app/Screen.h"
#include "config/AppSettings.h"

namespace ui {

enum class MenuAction {
    None, OpenGameSelection, OpenSettings, StartSnake, StartTetris, Back, Quit
};

struct MenuResult {
    MenuAction action = MenuAction::None;
    bool settingsChanged = false;
};

MenuResult DrawMenu(app::Screen screen, Font font, app::Settings& settings);

}  // namespace ui
