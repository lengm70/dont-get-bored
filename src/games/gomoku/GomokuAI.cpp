#include "games/gomoku/GomokuAI.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <vector>

namespace games::gomoku {
namespace {
constexpr int infinity = 100000000;
constexpr int victory = 10000000;
constexpr std::array<Move, 4> directions{{{1, 0}, {0, 1}, {1, 1}, {1, -1}}};
int PatternValue(int count, int open) {
    if (count >= 5) return victory;
    if (open == 0) return 0;
    if (count == 4) return open == 2 ? 100000 : 12000;
    if (count == 3) return open == 2 ? 4000 : 300;
    if (count == 2) return open == 2 ? 160 : 25;
    return open == 2 ? 10 : 2;
}
int PointValue(Board& board, Move move, Stone side) {
    const int index = move.y * rules::size + move.x;
    const Stone previous = board[index];
    board[index] = side;
    int score = 0;
    for (Move direction : directions) {
        int length = 1, open = 0;
        for (int sign : {-1, 1}) {
            int x = move.x + sign * direction.x, y = move.y + sign * direction.y;
            while (InBounds(x, y) && board[y * rules::size + x] == side) {
                ++length; x += sign * direction.x; y += sign * direction.y;
            }
            if (InBounds(x, y) && board[y * rules::size + x] == Stone::Empty) ++open;
        }
        score += PatternValue(length, open);
        // Five-cell windows also recognize broken threes/fours with an internal gap.
        for (int start = -4; start <= 0; ++start) {
            int own = 0;
            bool valid = true;
            for (int offset = 0; offset < 5; ++offset) {
                const int x = move.x + (start + offset) * direction.x;
                const int y = move.y + (start + offset) * direction.y;
                if (!InBounds(x, y) || board[y * rules::size + x] == Opponent(side)) { valid = false; break; }
                if (board[y * rules::size + x] == side) ++own;
            }
            if (valid && own == 4 && length < 4) score += 2000;
            else if (valid && own == 3 && length < 3) score += 120;
        }
    }
    board[index] = previous;
    return score;
}
struct Candidate { Move move; int priority; };
std::vector<Candidate> Candidates(Board& board, Stone side, int limit) {
    std::vector<Candidate> moves, wins, blocks;
    bool occupied = false;
    for (Stone cell : board) occupied |= cell != Stone::Empty;
    if (!occupied) return {{{rules::size / 2, rules::size / 2}, 0}};
    for (int y = 0; y < rules::size; ++y) for (int x = 0; x < rules::size; ++x) {
        if (board[y * rules::size + x] != Stone::Empty) continue;
        bool near = false;
        for (int dy = -rules::candidateRadius; dy <= rules::candidateRadius && !near; ++dy)
            for (int dx = -rules::candidateRadius; dx <= rules::candidateRadius; ++dx)
                if (InBounds(x + dx, y + dy) && board[(y + dy) * rules::size + x + dx] != Stone::Empty) { near = true; break; }
        if (!near) continue;
        const Move move{x, y};
        const int attack = PointValue(board, move, side);
        const int defense = PointValue(board, move, Opponent(side));
        const int priority = attack + defense + defense / 10;
        moves.push_back({move, priority});
        board[y * rules::size + x] = side;
        if (WinsAt(board, move, side)) wins.push_back({move, priority});
        board[y * rules::size + x] = Opponent(side);
        if (WinsAt(board, move, Opponent(side))) blocks.push_back({move, priority});
        board[y * rules::size + x] = Stone::Empty;
    }
    if (!wins.empty()) moves = std::move(wins);
    else if (!blocks.empty()) moves = std::move(blocks);
    std::stable_sort(moves.begin(), moves.end(), [](const auto& a, const auto& b) { return a.priority > b.priority; });
    if (moves.size() > static_cast<std::size_t>(limit)) moves.resize(limit);
    return moves;
}
struct Search {
    Board board;
    SearchOptions options;
    const std::atomic<bool>& cancelled;
    std::chrono::steady_clock::time_point deadline;
    int nodes = 0;
    bool interrupted = false;
    bool Stop() {
        interrupted |= cancelled.load() || nodes >= options.nodeLimit || std::chrono::steady_clock::now() >= deadline;
        return interrupted;
    }
    int Evaluate(Stone side) {
        int own = 0, opponent = 0;
        for (int y = 0; y < rules::size; ++y) for (int x = 0; x < rules::size; ++x) {
            const Stone stone = board[y * rules::size + x];
            if (stone == Stone::Empty) continue;
            const int value = PointValue(board, {x, y}, stone);
            if (stone == side) own += value; else opponent += value;
        }
        return own - opponent;
    }
    int Negamax(Stone side, int depth, int alpha, int beta) {
        ++nodes;
        if (Stop()) return 0;
        if (depth == 0) return Evaluate(side);
        const auto candidates = Candidates(board, side, options.candidates);
        if (candidates.empty()) return 0;
        int best = -infinity;
        for (const auto& candidate : candidates) {
            const Move move = candidate.move;
            board[move.y * rules::size + move.x] = side;
            const int value = WinsAt(board, move, side) ? victory + depth :
                -Negamax(Opponent(side), depth - 1, -beta, -alpha);
            board[move.y * rules::size + move.x] = Stone::Empty;
            if (Stop()) return 0;
            best = std::max(best, value);
            alpha = std::max(alpha, value);
            if (alpha >= beta) break;
        }
        return best;
    }
};
}  // namespace

SearchResult FindMove(Board board, Stone side, SearchOptions options, const std::atomic<bool>& cancelled) {
    SearchResult result;
    if (cancelled.load() || side == Stone::Empty) return result;
    options.depth = std::clamp(options.depth, 1, 8);
    options.candidates = std::clamp(options.candidates, 1, rules::cellCount);
    options.milliseconds = std::max(1, options.milliseconds);
    options.nodeLimit = std::max(1, options.nodeLimit);
    Search search{board, options, cancelled, std::chrono::steady_clock::now() + std::chrono::milliseconds(options.milliseconds)};
    const auto candidates = Candidates(search.board, side, options.candidates);
    if (candidates.empty()) return result;
    result.move = candidates.front().move;  // Legal tactical fallback if the time budget expires.
    for (int depth = 1; depth <= options.depth; ++depth) {
        Move bestMove = result.move;
        int best = -infinity, alpha = -infinity;
        for (const auto& candidate : candidates) {
            if (search.Stop()) break;
            const Move move = candidate.move;
            search.board[move.y * rules::size + move.x] = side;
            const int value = WinsAt(search.board, move, side) ? victory + depth :
                -search.Negamax(Opponent(side), depth - 1, -infinity, -alpha);
            search.board[move.y * rules::size + move.x] = Stone::Empty;
            if (search.Stop()) break;
            if (value > best) { best = value; bestMove = move; }
            alpha = std::max(alpha, value);
        }
        if (search.interrupted) break;
        result.move = bestMove;
        result.depth = depth;
        if (best >= victory) break;
    }
    result.nodes = search.nodes;
    if (cancelled.load()) result.move = {-1, -1};
    return result;
}
}  // namespace games::gomoku
