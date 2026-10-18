#pragma once
#include "games/race/RaceRules.h"
#include "games/race/RaceTrack.h"
#include <array>
namespace games::race {
enum class Status { Ready, Playing, Paused, Won, GameOver };
struct Input {
    float throttle = 0, steer = 0;
    bool brake = false, drift = false;
};
class RaceGame {
  public:
    RaceGame() {
        Reset();
    }
    void Reset(int track = 0);
    void Start();
    void TogglePause();
    void Update(float seconds, Input input = {});
    Status State() const {
        return state_;
    }
    int TrackIndex() const {
        return track_;
    }
    Point Position() const {
        return position_;
    }
    Point Velocity() const {
        return velocity_;
    }
    float Heading() const {
        return heading_;
    }
    float Speed() const;
    double Time() const {
        return time_;
    }
    int CompletedLaps() const {
        return laps_;
    }
    int NextGate() const {
        return next_;
    }
    int BestLapMs() const {
        return bestLap_;
    }
    int LastLapMs() const {
        return lastLap_;
    }
    bool Road() const {
        return OnRoad(GetTrack(track_), position_);
    }
    const std::array<Point, 12> &Trail() const {
        return trail_;
    }

  private:
    void Step(float dt, Input input);
    Status state_ = Status::Ready;
    int track_ = 0, laps_ = 0, next_ = 1, bestLap_ = 0, lastLap_ = 0;
    Point position_, velocity_;
    float heading_ = 0, trailTimer_ = 0;
    double time_ = 0, lapStart_ = 0;
    std::array<Point, 12> trail_{};
};
} // namespace games::race
