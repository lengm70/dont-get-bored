#include "games/minesweeper/MinesweeperGame.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <limits>
#include <vector>

namespace games::minesweeper {

MinesweeperGame::MinesweeperGame(std::uint32_t seed) : random_(seed) { Reset(); }

void MinesweeperGame::Reset() {
    board_.fill({});
    status_ = Status::Ready;
    placed_ = false;
    flags_ = 0;
    revealedSafe_ = 0;
    elapsed_ = 0.0;
}

void MinesweeperGame::Start() {
    if (status_ == Status::Ready) status_ = Status::Playing;
}

void MinesweeperGame::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}

bool MinesweeperGame::InBounds(int x, int y) {
    return x >= 0 && x < rules::columns && y >= 0 && y < rules::rows;
}

void MinesweeperGame::PlaceMines(int firstX, int firstY) {
    std::vector<int> candidates;
    for (int y = 0; y < rules::rows; ++y) {
        for (int x = 0; x < rules::columns; ++x) {
            if (std::abs(x - firstX) > rules::safeStartRadius ||
                std::abs(y - firstY) > rules::safeStartRadius) {
                candidates.push_back(y * rules::columns + x);
            }
        }
    }
    std::shuffle(candidates.begin(), candidates.end(), random_);
    for (int i = 0; i < rules::mineCount; ++i) board_[candidates[i]].mine = true;
    for (int y = 0; y < rules::rows; ++y) {
        for (int x = 0; x < rules::columns; ++x) {
            int count = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if ((dx != 0 || dy != 0) && InBounds(x + dx, y + dy) &&
                        At(x + dx, y + dy).mine) ++count;
                }
            }
            board_[y * rules::columns + x].adjacentMines = count;
        }
    }
    placed_ = true;
}

void MinesweeperGame::Reveal(int x, int y) {
    if (status_ != Status::Playing || !InBounds(x, y)) return;
    const Cell& cell = At(x, y);
    if (cell.flagged || cell.revealed) return;
    if (!placed_) PlaceMines(x, y);
    RevealArea(x, y);
}

void MinesweeperGame::RevealArea(int x, int y) {
    std::deque<int> pending{y * rules::columns + x};
    while (!pending.empty()) {
        const int index = pending.front();
        pending.pop_front();
        Cell& cell = board_[index];
        if (cell.revealed || cell.flagged) continue;
        cell.revealed = true;
        if (cell.mine) {
            status_ = Status::GameOver;
            return;
        }
        ++revealedSafe_;
        if (cell.adjacentMines == 0) {
            const int cellX = index % rules::columns;
            const int cellY = index / rules::columns;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (InBounds(cellX + dx, cellY + dy)) {
                        pending.push_back((cellY + dy) * rules::columns + cellX + dx);
                    }
                }
            }
        }
    }
    if (revealedSafe_ == rules::safeCellCount) status_ = Status::Won;
}

void MinesweeperGame::ToggleFlag(int x, int y) {
    if (status_ != Status::Playing || !InBounds(x, y)) return;
    Cell& cell = board_[y * rules::columns + x];
    if (cell.revealed || (!cell.flagged && flags_ == rules::mineCount)) return;
    cell.flagged = !cell.flagged;
    flags_ += cell.flagged ? 1 : -1;
}

void MinesweeperGame::RevealNeighbors(int x, int y) {
    if (status_ != Status::Playing || !InBounds(x, y) || !At(x, y).revealed) return;
    int flags = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (InBounds(x + dx, y + dy) && At(x + dx, y + dy).flagged) ++flags;
        }
    }
    if (flags != At(x, y).adjacentMines) return;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) Reveal(x + dx, y + dy);
    }
}

void MinesweeperGame::Update(float elapsedSeconds) {
    if (status_ == Status::Playing && placed_ && std::isfinite(elapsedSeconds)) {
        elapsed_ += std::max(0.0f, elapsedSeconds);
    }
}

int MinesweeperGame::ElapsedMilliseconds() const {
    const double milliseconds = std::ceil(elapsed_ * rules::millisecondsPerSecond);
    return static_cast<int>(std::min(milliseconds,
                                    static_cast<double>(std::numeric_limits<int>::max())));
}

}  // namespace games::minesweeper
