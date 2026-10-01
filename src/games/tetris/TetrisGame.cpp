#include "games/tetris/TetrisGame.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace games::tetris {
namespace {

using Shape = std::array<Cell, 4>;

constexpr std::array<Shape, rules::piecesPerBag> baseShapes{{
    {{{0, 1}, {1, 1}, {2, 1}, {3, 1}}},  // I
    {{{1, 0}, {2, 0}, {1, 1}, {2, 1}}},  // O
    {{{1, 0}, {0, 1}, {1, 1}, {2, 1}}},  // T
    {{{1, 0}, {2, 0}, {0, 1}, {1, 1}}},  // S
    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},  // Z
    {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},  // J
    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}}   // L
}};

constexpr std::array<int, 5> lineScores{0, 100, 300, 500, 800};

}  // namespace

TetrisGame::TetrisGame(std::uint32_t seed) : random_(seed) {
    Reset();
}

std::array<Cell, 4> TetrisGame::ShapeCells(PieceType type, int rotation) {
    Shape cells = baseShapes[static_cast<int>(type)];
    if (type == PieceType::O) return cells;
    const int turns = (rotation % rules::rotationCount + rules::rotationCount) %
                      rules::rotationCount;
    for (int turn = 0; turn < turns; ++turn) {
        for (Cell& cell : cells) {
            const int x = cell.x;
            cell.x = 3 - cell.y;
            cell.y = x;
        }
    }
    return cells;
}

std::array<Cell, 4> TetrisGame::ActiveCells() const {
    Shape cells = ShapeCells(active_.type, active_.rotation);
    for (Cell& cell : cells) {
        cell.x += active_.x;
        cell.y += active_.y;
    }
    return cells;
}

void TetrisGame::Reset() {
    for (auto& row : board_) row.fill(0);
    bagIndex_ = rules::piecesPerBag;
    score_ = 0;
    lines_ = 0;
    elapsed_ = 0.0f;
    wasSoftDrop_ = false;
    status_ = Status::Ready;
    active_ = {DrawFromBag(), 0, rules::spawnX, rules::spawnY};
    next_ = DrawFromBag();
}

void TetrisGame::Start() {
    if (status_ == Status::Ready) status_ = Status::Playing;
}

void TetrisGame::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}

void TetrisGame::MoveHorizontal(HorizontalMove direction) {
    if (status_ != Status::Playing) return;
    ActivePiece candidate = active_;
    candidate.x += static_cast<int>(direction);
    if (CanPlace(candidate)) active_ = candidate;
}

void TetrisGame::Rotate(Turn direction) {
    if (status_ != Status::Playing) return;
    ActivePiece candidate = active_;
    candidate.rotation = (candidate.rotation + static_cast<int>(direction) +
                          rules::rotationCount) % rules::rotationCount;
    for (const Cell kick : std::array<Cell, 7>{{{0, 0}, {-1, 0}, {1, 0},
                                                {-2, 0}, {2, 0}, {0, -1}, {0, -2}}}) {
        candidate.x = active_.x + kick.x;
        candidate.y = active_.y + kick.y;
        if (CanPlace(candidate)) {
            active_ = candidate;
            return;
        }
    }
}

void TetrisGame::HardDrop() {
    if (status_ != Status::Playing) return;
    int distance = 0;
    ActivePiece candidate = active_;
    while (CanPlace(ActivePiece{candidate.type, candidate.rotation,
                                candidate.x, candidate.y + 1})) {
        ++candidate.y;
        ++distance;
    }
    active_ = candidate;
    score_ += distance * rules::hardDropPointsPerCell;
    LockPiece();
}

void TetrisGame::Update(float elapsedSeconds, bool softDrop) {
    if (status_ != Status::Playing) return;
    if (softDrop != wasSoftDrop_) {
        elapsed_ = 0.0f;
        wasSoftDrop_ = softDrop;
    }
    elapsed_ += std::clamp(elapsedSeconds, 0.0f, rules::maxFrameSeconds);
    const float gravity = std::max(
        rules::minimumGravitySeconds,
        rules::initialGravitySeconds *
            std::pow(rules::gravityMultiplier, static_cast<float>(Level() - 1)));
    const float interval = softDrop ? std::min(gravity, rules::softDropSeconds) : gravity;
    while (elapsed_ >= interval && status_ == Status::Playing) {
        elapsed_ -= interval;
        StepDown(softDrop);
    }
}

PieceType TetrisGame::DrawFromBag() {
    if (bagIndex_ == rules::piecesPerBag) {
        bag_ = {PieceType::I, PieceType::O, PieceType::T, PieceType::S,
                PieceType::Z, PieceType::J, PieceType::L};
        std::shuffle(bag_.begin(), bag_.end(), random_);
        bagIndex_ = 0;
    }
    return bag_[bagIndex_++];
}

bool TetrisGame::CanPlace(const ActivePiece& piece) const {
    for (Cell cell : ShapeCells(piece.type, piece.rotation)) {
        cell.x += piece.x;
        cell.y += piece.y;
        if (cell.x < 0 || cell.x >= rules::columns || cell.y >= rules::rows) {
            return false;
        }
        if (cell.y >= 0 && board_[cell.y][cell.x] != 0) return false;
    }
    return true;
}

void TetrisGame::StepDown(bool softDrop) {
    ActivePiece candidate = active_;
    ++candidate.y;
    if (CanPlace(candidate)) {
        active_ = candidate;
        if (softDrop) score_ += rules::softDropPointsPerCell;
    } else {
        LockPiece();
    }
}

void TetrisGame::LockPiece() {
    const auto cells = ActiveCells();
    for (const Cell cell : cells) {
        if (cell.y < 0) {
            status_ = Status::GameOver;
            return;
        }
    }
    for (const Cell cell : cells) {
        board_[cell.y][cell.x] = static_cast<int>(active_.type) + 1;
    }
    const int previousLevel = Level();
    const int cleared = ClearFullRows(board_);
    lines_ += cleared;
    score_ += lineScores[cleared] * previousLevel;
    SpawnNext();
}

int ClearFullRows(Board& board) {
    int writeRow = rules::rows - 1;
    int cleared = 0;
    for (int readRow = rules::rows - 1; readRow >= 0; --readRow) {
        const bool full = std::all_of(board[readRow].begin(), board[readRow].end(),
                                      [](int cell) { return cell != 0; });
        if (full) {
            ++cleared;
        } else {
            board[writeRow--] = board[readRow];
        }
    }
    while (writeRow >= 0) board[writeRow--].fill(0);
    return cleared;
}

void TetrisGame::SpawnNext() {
    active_ = {next_, 0, rules::spawnX, rules::spawnY};
    next_ = DrawFromBag();
    elapsed_ = 0.0f;
    if (!CanPlace(active_)) status_ = Status::GameOver;
}

}  // namespace games::tetris
