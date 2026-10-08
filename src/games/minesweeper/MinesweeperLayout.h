#pragma once

#include <algorithm>
#include "games/minesweeper/MinesweeperConfig.h"
#include "games/minesweeper/MinesweeperGame.h"

namespace games::minesweeper {

struct BoardLayout {
    Rectangle bounds;
    float cellSize;
    Rectangle Cell(int x, int y) const {
        return {bounds.x + x * cellSize, bounds.y + y * cellSize, cellSize, cellSize};
    }
};

inline BoardLayout LayoutFor(const MinesweeperGame& game) {
    const float size = std::min({config::cellSize, config::boardArea.width / game.Columns(),
                                config::boardArea.height / game.Rows()});
    const float width = game.Columns() * size;
    return {{config::boardArea.x + (config::boardArea.width - width) / 2,
             config::boardArea.y, width, game.Rows() * size}, size};
}

}  // namespace games::minesweeper
