#include "ui/GameIllustration.h"

#include <array>

#include "ui/GameSelectionConfig.h"

namespace ui {
namespace {

struct Canvas {
    Rectangle bounds;
    float scale;

    Vector2 Point(float x, float y) const { return {bounds.x + x * scale, bounds.y + y * scale}; }
    void Block(Rectangle rectangle, Color color) const {
        DrawRectangleRec({bounds.x + rectangle.x * scale, bounds.y + rectangle.y * scale,
                          rectangle.width * scale, rectangle.height * scale}, color);
    }
    void Circle(Vector2 center, float radius, Color color) const {
        DrawCircleV(Point(center.x, center.y), radius * scale, color);
    }
    void Line(Vector2 start, Vector2 end, float width, Color color) const {
        DrawLineEx(Point(start.x, start.y), Point(end.x, end.y), width * scale, color);
    }
};

void DrawSnake(const Canvas& canvas) {
    constexpr int gridStart = 17, gridEnd = 143, gridStep = 18;
    for (int position = gridStart; position <= gridEnd; position += gridStep) {
        canvas.Line({static_cast<float>(position), gridStart},
                    {static_cast<float>(position), gridEnd}, 1, selection::artGridColor);
        canvas.Line({gridStart, static_cast<float>(position)},
                    {gridEnd, static_cast<float>(position)}, 1, selection::artGridColor);
    }
    constexpr std::array<Vector2, 10> body{{
        {35, 107}, {53, 107}, {71, 107}, {89, 107}, {89, 89},
        {89, 71}, {71, 71}, {53, 71}, {53, 53}, {71, 53}
    }};
    constexpr float segment = 15;
    for (std::size_t i = 0; i < body.size(); ++i) {
        canvas.Block({body[i].x, body[i].y, segment, segment},
                     Fade(selection::mint, 0.45f + 0.55f * static_cast<float>(i) / (body.size() - 1)));
    }
    constexpr std::array<Vector2, 2> eyes{{{82, 57}, {82, 64}}};
    for (Vector2 eye : eyes) canvas.Circle(eye, 1.5f, selection::darkInk);
    constexpr Vector2 food{123, 43};
    canvas.Circle(food, 11, Fade(selection::coral, 0.12f));
    canvas.Circle(food, 6, selection::coral);
    canvas.Line({125, 37}, {129, 32}, 2, selection::mint);
}

void DrawTetris(const Canvas& canvas) {
    constexpr int columns = 8, rows = 8;
    constexpr float origin = 16, step = 16, size = 14;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            canvas.Block({origin + x * step, origin + y * step, size, size},
                         Fade(selection::artGridColor, 0.45f));
        }
    }
    struct Tile { int x; int y; Color color; };
    const std::array<Tile, 20> tiles{{
        {0, 7, selection::blue}, {1, 7, selection::blue}, {2, 7, selection::blue},
        {2, 6, selection::blue}, {3, 7, selection::orange}, {4, 7, selection::orange},
        {3, 6, selection::orange}, {3, 5, selection::orange}, {5, 7, selection::yellow},
        {6, 7, selection::yellow}, {5, 6, selection::yellow}, {6, 6, selection::yellow},
        {7, 7, selection::mint}, {7, 6, selection::mint}, {7, 5, selection::mint},
        {7, 4, selection::mint}, {3, 1, selection::purple}, {2, 2, selection::purple},
        {3, 2, selection::purple}, {4, 2, selection::purple}
    }};
    for (const Tile& tile : tiles) {
        canvas.Block({origin + tile.x * step, origin + tile.y * step, size, size}, tile.color);
        canvas.Block({origin + tile.x * step + 2, origin + tile.y * step + 2, size - 4, 2},
                     Fade(WHITE, 0.25f));
    }
}

void DrawBreakout(const Canvas& canvas) {
    constexpr int columns = 5, rows = 3;
    constexpr float left = 13, top = 27, width = 24, height = 10, gap = 4;
    const std::array<Color, rows> colors{{selection::coral, selection::orange, selection::yellow}};
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            if (y == rows - 1 && x == 3) continue;
            canvas.Block({left + x * (width + gap), top + y * (height + gap), width, height}, colors[y]);
        }
    }
    constexpr std::array<Vector2, 4> trail{{{75, 121}, {83, 110}, {91, 99}, {99, 88}}};
    for (std::size_t i = 0; i < trail.size(); ++i) {
        canvas.Circle(trail[i], 2, Fade(selection::mint, 0.15f + 0.15f * i));
    }
    canvas.Circle({107, 77}, 10, Fade(WHITE, 0.08f));
    canvas.Circle({107, 77}, 5, WHITE);
    constexpr Rectangle paddle{46, 134, 58, 8};
    canvas.Block(paddle, selection::mint);
    canvas.Block({paddle.x, paddle.y + paddle.height, paddle.width, 4}, Fade(selection::mint, 0.12f));
}

void DrawMinesweeper(const Canvas& canvas, Font font) {
    constexpr int columns = 4, rows = 4;
    constexpr float left = 18, top = 18, step = 32, size = 29;
    constexpr std::array<int, rows * columns> cells{{
        0, 1, -1, -1, 0, 2, -2, -1, 0, 2, -1, -1, 0, 1, -1, -2
    }};
    for (int index = 0; index < static_cast<int>(cells.size()); ++index) {
        const float x = left + (index % columns) * step;
        const float y = top + (index / columns) * step;
        canvas.Block({x, y, size, size}, cells[index] >= 0 ?
                     selection::artGridColor : ui::palette::control);
        if (cells[index] > 0) {
            const char* number = cells[index] == 1 ? "1" : "2";
            constexpr float numberSize = 19;
            const Vector2 measured = MeasureTextEx(font, number, numberSize * canvas.scale, canvas.scale);
            DrawTextEx(font, number,
                       {canvas.bounds.x + (x + size / 2) * canvas.scale - measured.x / 2,
                        canvas.bounds.y + (y + size / 2) * canvas.scale - measured.y / 2},
                       numberSize * canvas.scale, canvas.scale,
                       cells[index] == 1 ? selection::blue : selection::mint);
        } else if (cells[index] == -2) {
            const Vector2 pole{ x + 10, y + 7 };
            canvas.Line(pole, {pole.x, y + 23}, 2, WHITE);
            DrawTriangle(canvas.Point(pole.x, pole.y), canvas.Point(pole.x, pole.y + 9),
                         canvas.Point(pole.x + 12, pole.y), selection::coral);
            canvas.Line({pole.x - 3, y + 23}, {pole.x + 4, y + 23}, 2, WHITE);
        }
    }
}

}  // namespace

void DrawGameIllustration(GameIllustration game, Rectangle bounds, Font font) {
    const Canvas canvas{bounds, bounds.width / selection::artSize};
    DrawRectangleRounded(bounds, selection::cornerRoundness, selection::cornerSegments,
                         selection::artBackground);
    switch (game) {
        case GameIllustration::Snake: DrawSnake(canvas); break;
        case GameIllustration::Tetris: DrawTetris(canvas); break;
        case GameIllustration::Breakout: DrawBreakout(canvas); break;
        case GameIllustration::Minesweeper: DrawMinesweeper(canvas, font); break;
    }
}

}  // namespace ui
