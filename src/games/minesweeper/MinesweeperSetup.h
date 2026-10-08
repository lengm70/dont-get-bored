#pragma once

#include "config/HighScores.h"
#include "games/minesweeper/MinesweeperDifficulty.h"
#include "ui/MenuView.h"

namespace games::minesweeper {

struct SetupOptions {
    int selected = 0;
    BoardSettings custom = difficulty::easy;
    bool editColumns = false;
    bool editRows = false;
    bool editMines = false;
};

ui::MenuResult DrawSetup(Font font, app::Language language, SetupOptions& options,
                         const app::HighScores& records);

}  // namespace games::minesweeper
