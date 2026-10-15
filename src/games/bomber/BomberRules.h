#pragma once
namespace games::bomber::rules {
inline constexpr int columns = 15, rows = 11, cellCount = columns * rows;
inline constexpr int stages = 3, initialLives = 3, defaultTargetWins = 3, maxTargetWins = 9;
inline constexpr float playerSpeed = 3.5f, speedUpgrade = 0.45f, maxPlayerSpeed = 5.3f;
inline constexpr float enemySpeed = 1.9f, enemyStageSpeed = 0.2f;
inline constexpr float actorRadius = 0.28f, cornerAssist = 0.3f;
inline constexpr int initialCapacity = 1, maxCapacity = 5, initialRange = 2, maxRange = 7;
inline constexpr float fuseSeconds = 2.3f, flameSeconds = 0.5f, pickupProtection = 0.56f;
inline constexpr float roundSeconds = 180, settleSeconds = 0.65f;
inline constexpr float physicsStep = 1.0f / 120, maxFrameSeconds = 0.25f;
inline constexpr int cratePoints = 10, enemyPoints = 100, stageBonus = 300;
inline constexpr int cratePercent = 52, crateStageIncrease = 4, dropPercent = 38;
inline constexpr float aiSafetyMargin = 0.18f, aiThreatHorizon = 2.8f, aiCenterTolerance = 0.04f;
inline constexpr int aiChasePercent = 75;
} // namespace games::bomber::rules
