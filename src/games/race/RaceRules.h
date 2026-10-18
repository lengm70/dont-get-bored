#pragma once
namespace games::race::rules {
inline constexpr int tracks = 3, laps = 3;
inline constexpr float acceleration = 260, maxSpeed = 220, roadDrag = .7f, offroadDrag = 4.2f,
                       braking = 5, turnRate = 2.8f;
inline constexpr float grip = 10, driftGrip = 2.1f, radius = 7, step = 1.f / 240, maxFrame = .25f,
                       timeout = 180;
} // namespace games::race::rules
