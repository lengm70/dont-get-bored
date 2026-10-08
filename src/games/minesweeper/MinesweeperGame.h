#pragma once

#include <vector>
#include <cstdint>
#include <random>

#include "games/minesweeper/MinesweeperRules.h"
#include "games/minesweeper/MinesweeperDifficulty.h"

namespace games::minesweeper {

enum class Status { Ready, Playing, Paused, GameOver, Won };

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacentMines = 0;
};

using Board = std::vector<Cell>;

class MinesweeperGame {
public:
    explicit MinesweeperGame(std::uint32_t seed = std::random_device{}());

    void Reset();
    bool Configure(Difficulty level, BoardSettings custom = difficulty::easy);
    void Start();
    void TogglePause();
    void Reveal(int x, int y);
    void ToggleFlag(int x, int y);
    void RevealNeighbors(int x, int y);
    void Update(float elapsedSeconds);

    const Board& Cells() const { return board_; }
    const Cell& At(int x, int y) const;
    BoardSettings Settings() const { return settings_; }
    Difficulty Level() const { return difficulty_; }
    int Columns() const { return settings_.columns; }
    int Rows() const { return settings_.rows; }
    int MineCount() const { return settings_.mines; }
    Status State() const { return status_; }
    int RemainingMines() const { return settings_.mines - flags_; }
    int RevealedSafeCells() const { return revealedSafe_; }
    int ElapsedMilliseconds() const;

private:
    bool InBounds(int x, int y) const;
    void PlaceMines(int firstX, int firstY);
    void RevealArea(int x, int y);

    std::mt19937 random_;
    Board board_{};
    BoardSettings settings_ = difficulty::easy;
    Difficulty difficulty_ = Difficulty::Easy;
    Status status_ = Status::Ready;
    bool placed_ = false;
    int flags_ = 0;
    int revealedSafe_ = 0;
    double elapsed_ = 0.0;
};

}  // namespace games::minesweeper
