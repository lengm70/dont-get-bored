#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <random>

namespace games::snake {

struct Cell {
    int x;
    int y;
};

inline bool operator==(Cell left, Cell right) {
    return left.x == right.x && left.y == right.y;
}

enum class Direction { Up, Right, Down, Left };
enum class Status { Ready, Playing, Paused, GameOver, Won };

class SnakeGame {
public:
    explicit SnakeGame(std::uint32_t seed = std::random_device{}());

    void Reset();
    void Start();
    void QueueDirection(Direction next);
    void TogglePause();
    void Update(float elapsedSeconds);

    const std::deque<Cell>& Body() const { return body_; }
    std::optional<Cell> Food() const { return food_; }
    Status State() const { return status_; }
    Direction Heading() const { return direction_; }
    int Score() const { return score_; }

private:
    void Step();
    void PlaceFood();

    std::mt19937 random_;
    std::deque<Cell> body_;
    std::optional<Cell> food_;
    Direction direction_ = Direction::Right;
    Direction queuedDirection_ = Direction::Right;
    Status status_ = Status::Ready;
    float elapsed_ = 0.0f;
    int score_ = 0;
};

}  // namespace games::snake
