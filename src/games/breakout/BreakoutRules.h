#pragma once
namespace games::breakout::rules {
inline constexpr float boardWidth = 640, boardHeight = 360;
inline constexpr int brickColumns = 10, brickRows = 5;
inline constexpr int brickCount = brickColumns * brickRows, levelCount = 5;
inline constexpr float brickMargin = 20, brickTop = 28, brickGap = 6;
inline constexpr float brickWidth = (boardWidth - 2 * brickMargin - (brickColumns - 1) * brickGap) / brickColumns;
inline constexpr float brickHeight = 18;
inline constexpr float paddleWidth = 96, paddleHeight = 14, paddleY = 330;
inline constexpr float paddleSpeed = 460, mousePaddleSpeed = 600;
inline constexpr float paddleAcceleration = 14000, paddleDeceleration = 22000, mouseTracking = 20;
inline constexpr float ballRadius = 7, ballSpeed = 320, levelSpeedIncrease = 25;
inline constexpr float serveHorizontalRatio = 0.45f;
inline constexpr float maximumBounceAngle = 1.04719755f;
inline constexpr float paddleInfluence = 0.22f, minimumVerticalRatio = 0.25f, minimumHorizontalRatio = 0.08f;
inline constexpr float separation = 0.01f, physicsStepSeconds = 1.0f / 240;
inline constexpr float maxFrameSeconds = 0.25f;
inline constexpr int initialLives = 3, pointsPerBrick = 10, lifeBonus = 100;
inline constexpr int comboStep = 5, maximumMultiplier = 4;
inline constexpr float movingAmplitude = 3, movingFrequency = 1.4f;
inline constexpr float widePaddleRatio = 1.5f, wideDuration = 12;
inline constexpr float dropSpeed = 95, dropRadius = 10, shieldY = 352;
inline constexpr int bricksPerDrop = 6, maximumDrops = 3, maximumBalls = 3;
inline constexpr float multiballAngle = 0.45f;
inline constexpr float effectDuration = 0.5f, trailInterval = 0.012f;
inline constexpr int maximumEffects = 40, trailLength = 10;
} // namespace games::breakout::rules
