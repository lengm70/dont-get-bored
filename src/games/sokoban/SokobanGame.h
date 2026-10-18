#pragma once
#include "games/sokoban/SokobanLevels.h"
#include <vector>
namespace games::sokoban {
enum class Status { Ready, Playing, Paused, Won };
struct Position {
    int x = 0, y = 0;
    bool operator==(Position p) const {
        return x == p.x && y == p.y;
    }
};
struct State {
    Position player;
    std::array<bool, cellCount> boxes{};
    int moves = 0, pushes = 0;
};
class SokobanGame {
  public:
    SokobanGame() {
        Reset();
    }
    void Reset(int level = 0);
    void Start();
    void TogglePause();
    bool Move(int dx, int dy);
    bool Undo();
    Status Phase() const {
        return phase_;
    }
    int LevelIndex() const {
        return level_;
    }
    const State &Board() const {
        return board_;
    }
    bool Wall(int index) const {
        return walls_[index];
    }
    bool Goal(int index) const {
        return goals_[index];
    }
    int GoalsFilled() const;
    Position PreviousPlayer() const {
        return previousPlayer_;
    }
    int MovedBoxFrom() const {
        return boxFrom_;
    }
    int MovedBoxTo() const {
        return boxTo_;
    }

  private:
    bool Free(Position p) const;
    int level_ = 0;
    Status phase_ = Status::Ready;
    State board_{};
    std::array<bool, cellCount> walls_{}, goals_{};
    std::vector<State> history_;
    Position previousPlayer_;
    int boxFrom_ = -1, boxTo_ = -1;
};
} // namespace games::sokoban
