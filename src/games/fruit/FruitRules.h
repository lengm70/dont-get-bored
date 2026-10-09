#pragma once
#include <array>
namespace games::fruit::rules {
inline constexpr float width = 300, height = 340;
inline constexpr std::array<float, 11> radii{10, 14, 18, 23, 29, 36, 44, 53, 63, 74, 86};
inline constexpr int lastLevel = static_cast<int>(radii.size()) - 1;
inline constexpr int spawnLevels = 4;
inline constexpr float spawnY = 22, dangerY = 48;
inline constexpr float gravity = 620, timeStep = 1.0f / 120;
inline constexpr float maxFrameTime = 0.1f, restitution = 0.42f;
inline constexpr float damping = 0.998f, angularDamping = 0.999f;
inline constexpr float friction = 0.32f, bounceThreshold = 30, impactDecay = 4;
inline constexpr float maxSpin = 12;
inline constexpr int solverIterations = 6, maxFruits = 300;
inline constexpr float dropDelay = 0.45f, spawnGrace = 1.2f, overflowDelay = 1.5f;
inline constexpr float aimSpeed = 240;
}  // namespace games::fruit::rules
