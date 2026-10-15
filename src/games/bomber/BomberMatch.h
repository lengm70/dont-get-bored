#pragma once
#include "games/bomber/BomberTypes.h"
namespace games::bomber {
// Match flow and scoring are independent from movement and explosion simulation.
class Match {
public:
    explicit Match(Options options = {});
    void Reset(Options options);
    bool Advance();
    void Start();
    void TogglePause();
    void Finish(Outcome outcome);
    void AddScore(int points);
    const Options& Settings() const { return options_; }
    Status State() const { return status_; }
    int Stage() const { return stage_; }
    int Round() const { return round_; }
    int Lives() const { return lives_; }
    int Score() const { return score_; }
    int Winner() const { return winner_; }
    const std::array<int,2>& Wins() const { return wins_; }
private:
    Options options_;
    Status status_ = Status::Ready;
    int stage_ = 1, round_ = 1, lives_ = rules::initialLives, score_ = 0, winner_ = -1;
    std::array<int,2> wins_{};
};
} // namespace games::bomber
