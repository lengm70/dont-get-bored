#include "games/gomoku/GomokuGame.h"

#include <stdexcept>

namespace games::gomoku {
namespace {
constexpr std::array<Move, 4> directions{{{1, 0}, {0, 1}, {1, 1}, {1, -1}}};
std::vector<Move> LineAt(const Board& board, Move move, Stone stone) {
    if (!InBounds(move.x, move.y) || stone == Stone::Empty ||
        board[move.y * rules::size + move.x] != stone) return {};
    for (auto direction : directions) {
        std::vector<Move> line{move};
        for (int sign : {-1, 1}) {
            Move next{move.x + sign * direction.x, move.y + sign * direction.y};
            while (InBounds(next.x, next.y) && board[next.y * rules::size + next.x] == stone) {
                if (sign < 0) line.insert(line.begin(), next);
                else line.push_back(next);
                next.x += sign * direction.x;
                next.y += sign * direction.y;
            }
        }
        if (line.size() >= rules::winningLength) return line;
    }
    return {};
}
}  // namespace

bool WinsAt(const Board& board, Move move, Stone stone) { return !LineAt(board, move, stone).empty(); }
Stone GomokuGame::At(int x, int y) const {
    if (!InBounds(x, y)) throw std::out_of_range("Gomoku cell outside board");
    return board_[y * rules::size + x];
}
void GomokuGame::Configure(Mode mode) { mode_ = mode; Reset(); }
void GomokuGame::Reset() {
    board_.fill(Stone::Empty);
    turn_ = Stone::Black;
    winner_ = Stone::Empty;
    state_ = Status::Ready;
    moves_ = 0;
    last_.reset();
    winningLine_.clear();
}
void GomokuGame::Start() { if (state_ == Status::Ready) state_ = Status::Playing; }
bool GomokuGame::Place(Move move) {
    if (state_ != Status::Playing || !InBounds(move.x, move.y) || At(move.x, move.y) != Stone::Empty) return false;
    board_[move.y * rules::size + move.x] = turn_;
    last_ = move;
    ++moves_;
    winningLine_ = LineAt(board_, move, turn_);
    if (!winningLine_.empty()) { winner_ = turn_; state_ = Status::Won; }
    else if (moves_ == rules::cellCount) state_ = Status::Draw;
    else turn_ = Opponent(turn_);
    return true;
}
}  // namespace games::gomoku
