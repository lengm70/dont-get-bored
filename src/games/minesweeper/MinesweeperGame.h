#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "games/minesweeper/MinesweeperRules.h"

namespace games::minesweeper {

enum class Status { Ready, Playing, Paused, GameOver, Won };

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacentMines = 0;
};

using Board = std::array<Cell, rules::cellCount>;

class MinesweeperGame {
public:
    explicit MinesweeperGame(std::uint32_t seed = std::random_device{}());

    void Reset();
    void Start();
    void TogglePause();
    void Reveal(int x, int y);
    void ToggleFlag(int x, int y);
    void RevealNeighbors(int x, int y);
    void Update(float elapsedSeconds);

    const Board& Cells() const { return board_; }
    const Cell& At(int x, int y) const { return board_.at(y * rules::columns + x); }
    Status State() const { return status_; }
    int RemainingMines() const { return rules::mineCount - flags_; }
    int RevealedSafeCells() const { return revealedSafe_; }
    int ElapsedMilliseconds() const;

private:
    static bool InBounds(int x, int y);
    void PlaceMines(int firstX, int firstY);
    void RevealArea(int x, int y);

    std::mt19937 random_;
    Board board_{};
    Status status_ = Status::Ready;
    bool placed_ = false;
    int flags_ = 0;
    int revealedSafe_ = 0;
    double elapsed_ = 0.0;
};

}  // namespace games::minesweeper
