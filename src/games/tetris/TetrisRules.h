#pragma once

namespace games::tetris::rules {

inline constexpr int columns = 10;
inline constexpr int rows = 20;
inline constexpr int piecesPerBag = 7;
inline constexpr int rotationCount = 4;
inline constexpr int spawnX = 3;
inline constexpr int spawnY = -1;
inline constexpr int linesPerLevel = 10;
inline constexpr int hardDropPointsPerCell = 2;
inline constexpr int softDropPointsPerCell = 1;
inline constexpr float initialGravitySeconds = 0.7f;
inline constexpr float gravityMultiplier = 0.85f;
inline constexpr float minimumGravitySeconds = 0.08f;
inline constexpr float softDropSeconds = 0.04f;
inline constexpr float maxFrameSeconds = 0.25f;

}  // namespace games::tetris::rules
