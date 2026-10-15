#pragma once
#include "games/breakout/BreakoutRules.h"

namespace games::breakout {

struct Point { float x; float y; };
struct Bounds { float x; float y; float width; float height; };

// Resolve circle/rectangle contact using the paddle's actual horizontal velocity.
// Top-face hits retain paddle aiming; side and corner hits reflect along the normal.
bool BounceOffPaddle(Point& ball, Point& velocity, Bounds paddle, float paddleVelocityX, float speed = rules::ballSpeed);
bool BounceOffRectangle(Point& ball, Point& velocity, Bounds bounds, float velocityX = 0);
void EnsureBallAngle(Point& velocity);

}  // namespace games::breakout
