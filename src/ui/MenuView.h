#pragma once

#include "raylib.h"
#include "app/Screen.h"
#include "config/AppSettings.h"

namespace ui {

enum class MenuAction {
    None, OpenGameSelection, OpenSettings, StartSnake, StartTetris,
    StartFruit, StartBreakout, StartMinesweeper, LaunchMinesweeper, StartGomoku, LaunchGomoku,
    OpenMusicSettings, ImportMusic, ImportBackground, OpenMediaLibrary, ImportMedia, UseMedia,
    PreviousMusic, ToggleMusic, NextMusic, Back, Quit
};

struct MenuResult {
    MenuAction action = MenuAction::None;
    bool settingsChanged = false;
    int mediaCategory = -1;
};

MenuResult DrawMenu(app::Screen screen, Font font, app::Settings& settings);
bool UpdateMusicPlayerInput();
MenuResult DrawMusicPlayer(Font font, app::Settings& settings);

}  // namespace ui
