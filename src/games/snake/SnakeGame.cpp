#include "games/snake/SnakeGame.h"

#include <algorithm>
#include <vector>

#include "games/snake/SnakeRules.h"

namespace games::snake {
namespace {

bool AreOpposite(Direction first, Direction second) {
    return (first == Direction::Up && second == Direction::Down) ||
           (first == Direction::Down && second == Direction::Up) ||
           (first == Direction::Left && second == Direction::Right) ||
           (first == Direction::Right && second == Direction::Left);
}

Cell NextCell(Cell cell, Direction direction) {
    switch (direction) {
        case Direction::Up: --cell.y; break;
        case Direction::Right: ++cell.x; break;
        case Direction::Down: ++cell.y; break;
        case Direction::Left: --cell.x; break;
    }
    return cell;
}

}  // namespace

SnakeGame::SnakeGame(std::uint32_t seed) : random_(seed) {
    Reset();
}

void SnakeGame::Reset() {
    body_.clear();
    const int headX = rules::columns / 2;
    const int headY = rules::rows / 2;
    for (int offset = 0; offset < rules::initialLength; ++offset) {
        body_.push_back({headX - offset, headY});
    }
    direction_ = Direction::Right;
    queuedDirection_ = direction_;
    status_ = Status::Ready;
    elapsed_ = 0.0f;
    score_ = 0;
    PlaceFood();
}

void SnakeGame::Start() {
    if (status_ == Status::Ready) status_ = Status::Playing;
}

void SnakeGame::QueueDirection(Direction next) {
    if (status_ != Status::Playing || queuedDirection_ != direction_) return;
    if (!AreOpposite(direction_, next)) queuedDirection_ = next;
}

void SnakeGame::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}

void SnakeGame::Update(float elapsedSeconds) {
    if (status_ != Status::Playing) return;
    elapsed_ += std::clamp(elapsedSeconds, 0.0f, rules::maxFrameSeconds);
    while (elapsed_ >= rules::secondsPerStep && status_ == Status::Playing) {
        elapsed_ -= rules::secondsPerStep;
        Step();
    }
}

void SnakeGame::Step() {
    direction_ = queuedDirection_;
    const Cell next = NextCell(body_.front(), direction_);
    if (next.x < 0 || next.x >= rules::columns ||
        next.y < 0 || next.y >= rules::rows) {
        status_ = Status::GameOver;
        return;
    }

    const bool eating = food_.has_value() && next == *food_;
    const std::size_t occupiedCount = body_.size() - (eating ? 0 : 1);
    for (std::size_t index = 0; index < occupiedCount; ++index) {
        if (body_[index] == next) {
            status_ = Status::GameOver;
            return;
        }
    }

    body_.push_front(next);
    if (eating) {
        score_ += rules::pointsPerFood;
        PlaceFood();
    } else {
        body_.pop_back();
    }
}

void SnakeGame::PlaceFood() {
    std::vector<Cell> freeCells;
    freeCells.reserve(rules::columns * rules::rows - body_.size());
    for (int y = 0; y < rules::rows; ++y) {
        for (int x = 0; x < rules::columns; ++x) {
            const Cell candidate{x, y};
            if (std::find(body_.begin(), body_.end(), candidate) == body_.end()) {
                freeCells.push_back(candidate);
            }
        }
    }

    if (freeCells.empty()) {
        food_.reset();
        status_ = Status::Won;
        return;
    }
    std::uniform_int_distribution<std::size_t> choose(0, freeCells.size() - 1);
    food_ = freeCells[choose(random_)];
}

}  // namespace games::snake
