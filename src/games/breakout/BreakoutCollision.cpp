#include "games/breakout/BreakoutCollision.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "games/breakout/BreakoutRules.h"

namespace games::breakout {
namespace {

struct Contact { Point normal; float penetration; };

std::optional<Contact> FindContact(Point ball, Bounds paddle) {
    const Point closest{std::clamp(ball.x, paddle.x, paddle.x + paddle.width),
                        std::clamp(ball.y, paddle.y, paddle.y + paddle.height)};
    const Point delta{ball.x - closest.x, ball.y - closest.y};
    const float squared = delta.x * delta.x + delta.y * delta.y;
    if (squared > rules::ballRadius * rules::ballRadius) return std::nullopt;
    if (squared > 0.0f) {
        const float distance = std::sqrt(squared);
        return Contact{{delta.x / distance, delta.y / distance}, rules::ballRadius - distance};
    }

    // If a moving paddle overlaps the center, eject through the nearest face.
    Contact contact{{0.0f, -1.0f}, ball.y - paddle.y};
    const auto consider = [&contact](Point normal, float distance) {
        if (distance < contact.penetration) contact = {normal, distance};
    };
    consider({0.0f, 1.0f}, paddle.y + paddle.height - ball.y);
    consider({-1.0f, 0.0f}, ball.x - paddle.x);
    consider({1.0f, 0.0f}, paddle.x + paddle.width - ball.x);
    contact.penetration += rules::ballRadius;
    return contact;
}

}  // namespace

bool BounceOffPaddle(Point& ball, Point& velocity, Bounds paddle, float paddleVelocityX) {
    const auto contact = FindContact(ball, paddle);
    if (!contact) return false;
    const Point normal = contact->normal;
    const float separation = contact->penetration + rules::separation;
    ball.x += normal.x * separation;
    ball.y += normal.y * separation;

    const float incoming = (velocity.x - paddleVelocityX) * normal.x + velocity.y * normal.y;
    // An already departing ball is only separated; do not bounce it back into the paddle.
    if (incoming >= 0.0f) return true;
    if (normal.x == 0.0f && normal.y == -1.0f) {
        const float offset = std::clamp(
            (ball.x - paddle.x - paddle.width / 2.0f) / (paddle.width / 2.0f), -1.0f, 1.0f);
        const float angle = offset * rules::maximumBounceAngle;
        velocity = {rules::ballSpeed * std::sin(angle), -rules::ballSpeed * std::cos(angle)};
    } else {
        // Reflect relative to the paddle, then return to world velocity.
        velocity.x -= 2.0f * incoming * normal.x;
        velocity.y -= 2.0f * incoming * normal.y;
        const float speed = std::hypot(velocity.x, velocity.y);
        if (speed > 0.0f) {
            velocity.x *= rules::ballSpeed / speed;
            velocity.y *= rules::ballSpeed / speed;
        } else {
            velocity = {normal.x * rules::ballSpeed, normal.y * rules::ballSpeed};
        }
    }
    return true;
}

}  // namespace games::breakout
