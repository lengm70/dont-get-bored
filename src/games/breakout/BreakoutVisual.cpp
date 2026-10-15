#include "games/breakout/BreakoutVisual.h"
#include <cmath>
#include "games/breakout/BreakoutConfig.h"
#include "config/UiConfig.h"
namespace games::breakout {
namespace {
constexpr float pi = 3.14159265f;
Vector2 Position(Point point, const ui::UiLayout& layout) {
    return layout.Point(config::boardX + point.x, config::boardY + point.y);
}
Rectangle Rect(Bounds bounds, const ui::UiLayout& layout) {
    return layout.Rect({config::boardX + bounds.x, config::boardY + bounds.y, bounds.width, bounds.height});
}
Color BrickColor(BrickKind kind, int row) {
    switch (kind) {
        case BrickKind::Armored: return config::armorColor;
        case BrickKind::Explosive: return config::explosionColor;
        case BrickKind::Moving: return config::paddleColor;
        case BrickKind::Obstacle: return config::obstacleColor;
        case BrickKind::Normal: return config::brickColors[row];
    }
    return WHITE;
}
void Line(Point a, Point b, float width, Color color, const ui::UiLayout& layout) {
    DrawLineEx(Position(a, layout), Position(b, layout), width * layout.scale, color);
}
void DrawBrick(const BreakoutGame& game, int index, const ui::UiLayout& layout) {
    const Brick& brick = game.BrickStates()[index];
    if (!brick.Alive()) return;
    const Bounds bounds = game.CurrentBrick(index);
    const Color color = BrickColor(brick.kind, index / rules::brickColumns);
    const Rectangle rectangle = Rect(bounds, layout);
    float flash = 0;
    for (const auto& impact : game.Impacts())
        if ((impact.kind == ImpactKind::Brick || impact.kind == ImpactKind::Explosion) &&
            impact.age < config::flashSeconds && std::abs(impact.position.x - bounds.x - bounds.width / 2) < bounds.width / 2 &&
            std::abs(impact.position.y - bounds.y - bounds.height / 2) < bounds.height / 2)
            flash = 1 - impact.age / config::flashSeconds;
    DrawRectangleRec(rectangle, Fade(color, 0.25f));
    DrawRectangleLinesEx(rectangle, layout.scale, color);
    DrawRectangleRec(rectangle, Fade(WHITE, flash * 0.7f));
    Line({bounds.x + 4, bounds.y + 3}, {bounds.x + bounds.width - 4, bounds.y + 3}, 1, Fade(color, 0.6f), layout);
    const Point center{bounds.x + bounds.width / 2, bounds.y + bounds.height / 2};
    if (brick.kind == BrickKind::Armored) {
        for (int h = 0; h < brick.health; ++h)
            Line({center.x - (brick.health - 1) * 4 + h * 8, center.y - 3},
                 {center.x - (brick.health - 1) * 4 + h * 8, center.y + 3}, 2, color, layout);
    } else if (brick.kind == BrickKind::Explosive) {
        DrawPolyLinesEx(Position(center, layout), 4, 5 * layout.scale, 0, layout.scale, color);
        Line({center.x - 3, center.y}, {center.x + 3, center.y}, 1, color, layout);
    } else if (brick.kind == BrickKind::Moving) {
        Line({center.x - 7, center.y}, {center.x + 7, center.y}, 1, color, layout);
        Line({center.x + 3, center.y - 3}, {center.x + 7, center.y}, 1, color, layout);
    } else if (brick.kind == BrickKind::Obstacle) {
        Line({bounds.x + 5, bounds.y + 5}, {bounds.x + bounds.width - 5, bounds.y + bounds.height - 5}, 1, color, layout);
        Line({bounds.x + 5, bounds.y + bounds.height - 5}, {bounds.x + bounds.width - 5, bounds.y + 5}, 1, color, layout);
    }
}
void DrawImpact(const Impact& impact, const ui::UiLayout& layout) {
    const float t = impact.age / rules::effectDuration, opacity = 1 - t;
    const bool fragments = impact.kind == ImpactKind::Brick || impact.kind == ImpactKind::Explosion;
    const Color color = impact.kind == ImpactKind::Explosion ? config::explosionColor : config::paddleColor;
    DrawPolyLinesEx(Position(impact.position, layout), 6, (3 + t * (fragments ? 26 : 18)) * layout.scale,
                    t * 60, layout.scale, Fade(color, opacity * 0.7f));
    if (!fragments) return;
    for (int i = 0; i < config::fragmentCount; ++i) {
        const float angle = (i * 2 * pi / config::fragmentCount) + impact.serial * 0.7f;
        const float distance = t * (impact.kind == ImpactKind::Explosion ? 60 : 35);
        const Point point{impact.position.x + std::cos(angle) * distance,
                          impact.position.y + std::sin(angle) * distance + t * t * 12};
        DrawPoly(Position(point, layout), 3, (1 + opacity * 2) * layout.scale, t * 120 + i * 30, Fade(color, opacity));
    }
}
} // namespace
ui::UiLayout ImpactLayout(const BreakoutGame& game, ui::UiLayout layout, bool shake) {
    if (!shake || game.State() != Status::Playing) return layout;
    for (const auto& event : game.Impacts()) {
        if ((event.kind == ImpactKind::Explosion || event.kind == ImpactKind::Shield) && event.age < config::shakeSeconds) {
            const float strength = config::shakeAmplitude * (1 - event.age / config::shakeSeconds) * layout.scale;
            layout.offsetX += std::sin(event.age * 130 + event.serial) * strength;
            layout.offsetY += std::cos(event.age * 110 + event.serial) * strength;
            break;
        }
    }
    return layout;
}
void DrawPlayfield(const BreakoutGame& game, const ui::UiLayout& layout) {
    for (float x = 0; x <= rules::boardWidth; x += config::gridSpacing)
        Line({x, 0}, {x, rules::boardHeight}, 1, Fade(ui::config::accentColor, 0.05f), layout);
    for (float y = 0; y <= rules::boardHeight; y += config::gridSpacing)
        Line({0, y}, {rules::boardWidth, y}, 1, Fade(ui::config::accentColor, 0.05f), layout);
    for (int index = 0; index < rules::brickCount; ++index) DrawBrick(game, index, layout);
    const auto paddle = Rect(game.Paddle(), layout);
    DrawRectangleRec(paddle, Fade(config::paddleColor, 0.22f));
    DrawRectangleLinesEx(paddle, 2 * layout.scale, config::paddleColor);
    for (const auto& ball : game.Balls()) {
        for (int i = rules::trailLength - 1; i >= 0; --i) {
            const float strength = 1 - static_cast<float>(i) / rules::trailLength;
            DrawCircleV(Position(ball.trail[i], layout), rules::ballRadius * strength * layout.scale,
                        Fade(config::paddleColor, strength * 0.25f));
        }
        DrawCircleV(Position(ball.position, layout), rules::ballRadius * 1.6f * layout.scale, Fade(config::paddleColor, 0.12f));
        DrawCircleV(Position(ball.position, layout), rules::ballRadius * layout.scale, config::ballColor);
    }
    for (const auto& drop : game.Drops()) {
        const Color color = drop.kind == PowerUpKind::WidePaddle ? config::paddleColor :
                            drop.kind == PowerUpKind::Shield ? config::shieldColor : config::multiColor;
        DrawPoly(Position(drop.position, layout), 6, rules::dropRadius * layout.scale, 30, Fade(color, 0.22f));
        DrawPolyLinesEx(Position(drop.position, layout), 6, rules::dropRadius * layout.scale, 30, layout.scale, color);
        const Point pos = drop.position;
        if (drop.kind == PowerUpKind::WidePaddle) Line({pos.x - 5, pos.y}, {pos.x + 5, pos.y}, 3, color, layout);
        else if (drop.kind == PowerUpKind::Shield) DrawPolyLinesEx(Position(pos, layout), 4, 5 * layout.scale, 0, layout.scale, color);
        else for (float offset : {-4.0f, 0.0f, 4.0f}) DrawCircleV(Position({pos.x + offset, pos.y}, layout), 1.5f * layout.scale, color);
    }
    if (game.Powers().shield) Line({0,rules::shieldY}, {rules::boardWidth,rules::shieldY}, 2, config::shieldColor, layout);
    for (const auto& impact : game.Impacts()) DrawImpact(impact, layout);
}
} // namespace games::breakout
