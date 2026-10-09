#include "games/fruit/FruitVisual.h"
#include <cmath>
#include <algorithm>
#include "games/fruit/FruitConfig.h"
namespace games::fruit {
void DrawFusionOrb(Vector2 center, float radius, int level, float angle, float impact, float scale, unsigned char alpha) {
    const Color tone = config::colors[level];
    const float opacity = alpha / 255.0f;
    const int sides = config::minSides + level % 4;
    const float degrees = angle * RAD2DEG;
    const float stroke = std::max(1.0f, config::glowWidth * scale);
    DrawCircleV(center, radius, ColorAlpha(tone, opacity * (0.10f + impact * 0.18f)));
    DrawCircleLinesV(center, radius, ColorAlpha(tone, opacity * 0.60f));
    DrawPoly(center, sides, radius * config::shellRadius, degrees, ColorAlpha(tone, opacity * 0.18f));
    DrawPolyLinesEx(center, sides, radius * config::shellRadius, degrees, stroke,
        ColorAlpha(tone, opacity));
    DrawPolyLinesEx(center, sides, radius * config::innerRadius, -degrees, scale,
        ColorAlpha(tone, opacity * 0.50f));
    for (int index = 0; index < sides; ++index) {
        const float rotation = angle + index * 2 * PI / sides;
        const Vector2 inner{center.x + std::cos(rotation) * radius * 0.72f,
                            center.y + std::sin(rotation) * radius * 0.72f};
        const Vector2 outer{center.x + std::cos(rotation) * radius * config::shellRadius,
                            center.y + std::sin(rotation) * radius * config::shellRadius};
        DrawLineEx(inner, outer, scale, ColorAlpha(tone, opacity * 0.80f));
    }
    DrawPoly(center, 4, radius * config::coreRadius * (1 + impact * 0.7f), degrees + 45,
        ColorAlpha(WHITE, opacity * 0.90f));
    if (impact > 0) DrawCircleLinesV(center, radius * (0.75f + impact * 0.25f), ColorAlpha(WHITE, opacity * impact));
}
}  // namespace games::fruit
