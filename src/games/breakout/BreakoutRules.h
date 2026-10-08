#pragma once

namespace games::breakout::rules {

inline constexpr float boardWidth = 640.0f;
inline constexpr float boardHeight = 360.0f;
inline constexpr int brickColumns = 10;
inline constexpr int brickRows = 5;
inline constexpr int brickCount = brickColumns * brickRows;
inline constexpr float brickMargin = 20.0f;
inline constexpr float brickTop = 28.0f;
inline constexpr float brickGap = 6.0f;
inline constexpr float brickWidth =
    (boardWidth - 2.0f * brickMargin - (brickColumns - 1) * brickGap) / brickColumns;
inline constexpr float brickHeight = 18.0f;
inline constexpr float paddleWidth = 96.0f;
inline constexpr float paddleHeight = 14.0f;
inline constexpr float paddleY = 330.0f;
inline constexpr float paddleSpeed = 460.0f;
inline constexpr float ballRadius = 7.0f;
inline constexpr float ballSpeed = 320.0f;
inline constexpr float serveHorizontalRatio = 0.45f;
inline constexpr float maximumBounceAngle = 1.04719755f;  // 60 degrees, in radians.
inline constexpr float paddleDeflectionMin = 0.01745329f;  // 1 degree.
inline constexpr float paddleDeflectionMax = 0.13962634f;  // 8 degrees.
inline constexpr float separation = 0.01f;
inline constexpr float physicsStepSeconds = 1.0f / 240.0f;
inline constexpr float maxFrameSeconds = 0.25f;
inline constexpr int initialLives = 3;
inline constexpr int pointsPerBrick = 10;

}  // namespace games::breakout::rules
