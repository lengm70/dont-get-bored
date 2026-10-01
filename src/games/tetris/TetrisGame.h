#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "games/tetris/TetrisRules.h"

namespace games::tetris {

struct Cell {
    int x;
    int y;
};

enum class PieceType { I, O, T, S, Z, J, L };
enum class Status { Ready, Playing, Paused, GameOver };
enum class HorizontalMove { Left = -1, Right = 1 };
enum class Turn { Counterclockwise = -1, Clockwise = 1 };

struct ActivePiece {
    PieceType type = PieceType::I;
    int rotation = 0;
    int x = rules::spawnX;
    int y = rules::spawnY;
};

using Board = std::array<std::array<int, rules::columns>, rules::rows>;

int ClearFullRows(Board& board);

class TetrisGame {
public:
    explicit TetrisGame(std::uint32_t seed = std::random_device{}());

    void Reset();
    void Start();
    void TogglePause();
    void MoveHorizontal(HorizontalMove direction);
    void Rotate(Turn direction);
    void HardDrop();
    void Update(float elapsedSeconds, bool softDrop);

    static std::array<Cell, 4> ShapeCells(PieceType type, int rotation);
    std::array<Cell, 4> ActiveCells() const;

    const Board& LockedBoard() const { return board_; }
    ActivePiece Active() const { return active_; }
    PieceType NextPiece() const { return next_; }
    Status State() const { return status_; }
    int Score() const { return score_; }
    int Lines() const { return lines_; }
    int Level() const { return 1 + lines_ / rules::linesPerLevel; }

private:
    PieceType DrawFromBag();
    bool CanPlace(const ActivePiece& piece) const;
    void StepDown(bool softDrop);
    void LockPiece();
    void SpawnNext();

    std::mt19937 random_;
    std::array<PieceType, rules::piecesPerBag> bag_{};
    int bagIndex_ = rules::piecesPerBag;
    Board board_{};
    ActivePiece active_{};
    PieceType next_ = PieceType::I;
    Status status_ = Status::Ready;
    float elapsed_ = 0.0f;
    bool wasSoftDrop_ = false;
    int score_ = 0;
    int lines_ = 0;
};

}  // namespace games::tetris
