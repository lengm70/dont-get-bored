#include "games/race/RaceGame.h"
#include <algorithm>
#include <cmath>
namespace games::race {
void RaceGame::Reset(int track) {
    track_ = std::clamp(track, 0, rules::tracks - 1);
    state_ = Status::Ready;
    laps_ = bestLap_ = lastLap_ = 0;
    next_ = 1;
    time_ = lapStart_ = 0;
    const auto &t = GetTrack(track_);
    position_ = t.gates[0];
    velocity_ = {};
    heading_ = std::atan2(t.gates[1].y - position_.y, t.gates[1].x - position_.x);
    trail_.fill(position_);
    trailTimer_ = 0;
}
void RaceGame::Start() {
    if (state_ == Status::Ready)
        state_ = Status::Playing;
}
void RaceGame::TogglePause() {
    if (state_ == Status::Playing)
        state_ = Status::Paused;
    else if (state_ == Status::Paused)
        state_ = Status::Playing;
}
float RaceGame::Speed() const {
    return std::hypot(velocity_.x, velocity_.y);
}
void RaceGame::Step(float dt, Input input) {
    time_ += dt;
    if (time_ >= rules::timeout) {
        state_ = Status::GameOver;
        return;
    }
    const auto &t = GetTrack(track_);
    bool road = OnRoad(t, position_);
    float speed = Speed();
    heading_ += input.steer * rules::turnRate *
                (speed > 1 ? std::clamp(speed / 90.f, .3f, 1.f) : 0.f) *
                (input.drift ? 1.15f : 1.f) * dt;
    heading_ = std::remainder(heading_, 6.2831853f);
    float fx = std::cos(heading_), fy = std::sin(heading_);
    velocity_.x += fx * input.throttle * rules::acceleration * (road ? 1.f : .32f) * dt;
    velocity_.y += fy * input.throttle * rules::acceleration * (road ? 1.f : .32f) * dt;
    float lateral = -fy * velocity_.x + fx * velocity_.y;
    float grip = 1 - std::exp(-(input.drift ? rules::driftGrip : rules::grip) * dt);
    velocity_.x += fy * lateral * grip;
    velocity_.y -= fx * lateral * grip;
    float drag = std::exp(
        -((road ? rules::roadDrag : rules::offroadDrag) + (input.brake ? rules::braking : 0)) * dt);
    velocity_.x *= drag;
    velocity_.y *= drag;
    speed = Speed();
    if (speed > rules::maxSpeed) {
        velocity_.x *= rules::maxSpeed / speed;
        velocity_.y *= rules::maxSpeed / speed;
    }
    Point previous = position_;
    position_.x = std::clamp(position_.x + velocity_.x * dt, rules::radius, 600 - rules::radius);
    position_.y = std::clamp(position_.y + velocity_.y * dt, rules::radius, 360 - rules::radius);
    Point gate = t.gates[next_], from = t.gates[(next_ + t.gates.size() - 1) % t.gates.size()];
    float direction = velocity_.x * (gate.x - from.x) + velocity_.y * (gate.y - from.y);
    if (direction > 1 && SegmentDistance(gate, previous, position_) < t.halfWidth + rules::radius) {
        if (next_ == 0) {
            ++laps_;
            lastLap_ = int(std::round((time_ - lapStart_) * 1000));
            if (!bestLap_ || lastLap_ < bestLap_)
                bestLap_ = lastLap_;
            lapStart_ = time_;
            if (laps_ == rules::laps)
                state_ = Status::Won;
        }
        next_ = (next_ + 1) % int(t.gates.size());
    }
    trailTimer_ += dt;
    if (trailTimer_ > .025f) {
        trailTimer_ = 0;
        for (int i = 11; i > 0; --i)
            trail_[i] = trail_[i - 1];
        trail_[0] = position_;
    }
}
void RaceGame::Update(float seconds, Input input) {
    if (state_ != Status::Playing || !std::isfinite(seconds) || !std::isfinite(input.throttle) ||
        !std::isfinite(input.steer))
        return;
    input.throttle = std::clamp(input.throttle, 0.f, 1.f);
    input.steer = std::clamp(input.steer, -1.f, 1.f);
    float left = std::clamp(seconds, 0.f, rules::maxFrame);
    while (left > 0 && state_ == Status::Playing) {
        float dt = std::min(left, rules::step);
        Step(dt, input);
        left -= dt;
    }
}
} // namespace games::race
