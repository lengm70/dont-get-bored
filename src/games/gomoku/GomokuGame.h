#pragma once

#include <array>
#include <optional>
#include <vector>
#include "games/gomoku/GomokuRules.h"

namespace games::gomoku {
enum class Stone { Empty, Black, White };
enum class Mode { TwoPlayers, Computer };
enum class Status { Ready, Playing, Won, Draw };
struct Move { int x; int y; };
using Board = std::array<Stone, rules::cellCount>;
inline Stone Opponent(Stone stone) { return stone == Stone::Black ? Stone::White : Stone::Black; }
inline bool InBounds(int x, int y) { return x >= 0 && y >= 0 && x < rules::size && y < rules::size; }
bool WinsAt(const Board& board, Move move, Stone stone);

class GomokuGame {
public:
    void Configure(Mode mode);
    void Reset();
    void Start();
    bool Place(Move move);
    const Board& Cells() const { return board_; }
    Stone At(int x, int y) const;
    Stone Turn() const { return turn_; }
    Stone Winner() const { return winner_; }
    Mode PlayMode() const { return mode_; }
    Status State() const { return state_; }
    bool HumanTurn() const { return state_ == Status::Playing && (mode_ == Mode::TwoPlayers || turn_ == Stone::Black); }
    int MoveCount() const { return moves_; }
    std::optional<Move> LastMove() const { return last_; }
    const std::vector<Move>& WinningLine() const { return winningLine_; }
private:
    Board board_{};
    Stone turn_ = Stone::Black;
    Stone winner_ = Stone::Empty;
    Mode mode_ = Mode::TwoPlayers;
    Status state_ = Status::Ready;
    int moves_ = 0;
    std::optional<Move> last_;
    std::vector<Move> winningLine_;
};
}  // namespace games::gomoku
