#include "games/sokoban/SokobanScreen.h"
#include <cmath>
namespace games::sokoban {
void UpdateFromInput(SokobanGame &g, ViewState &v, float dt, app::ProgressRecords &r) {
    v.animation = std::min(1.f, v.animation + std::max(0.f, dt) / .09f);
    if (IsKeyPressed(KEY_R)) {
        g.Reset(g.LevelIndex());
        v = {};
        return;
    }
    if (g.Phase() == Status::Ready) {
        int d = int(IsKeyPressed(KEY_RIGHT)) - int(IsKeyPressed(KEY_LEFT));
        if (d) {
            g.Reset(std::clamp(g.LevelIndex() + d, 0,
                               std::min(levelCount, std::max(1, r.Get("sokoban.unlock"))) - 1));
            return;
        }
    }
    if (IsKeyPressed(KEY_SPACE) &&
        (g.Phase() == Status::Ready ||
         (g.Phase() == Status::Won && g.LevelIndex() < levelCount - 1))) {
        if (g.Phase() == Status::Won)
            g.Reset(std::min(levelCount - 1, g.LevelIndex() + 1));
        g.Start();
        v = {};
        return;
    }
    if (IsKeyPressed(KEY_P)) {
        g.TogglePause();
        return;
    }
    if (IsKeyPressed(KEY_Z)) {
        if (g.Undo())
            v.animation = 0;
        return;
    }
    if (g.Phase() != Status::Playing)
        return;
    const int keys[]{KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT}, other[]{KEY_W, KEY_D, KEY_S, KEY_A},
        dx[]{0, 1, 0, -1}, dy[]{-1, 0, 1, 0};
    int direction = -1;
    for (int i = 0; i < 4; ++i)
        if (IsKeyDown(keys[i]) || IsKeyDown(other[i])) {
            direction = i;
            break;
        }
    v.repeat -= dt;
    if (direction != v.held) {
        v.held = direction;
        v.repeat = 0;
    }
    if (direction >= 0 && v.repeat <= 0) {
        if (g.Move(dx[direction], dy[direction]))
            v.animation = 0;
        v.repeat = .14f;
    }
    if (g.Phase() == Status::Won) {
        r.Best("sokoban.moves." + std::to_string(g.LevelIndex() + 1), g.Board().moves, true);
        r.Best("sokoban.unlock", std::min(levelCount, g.LevelIndex() + 2));
    }
}
void DrawScreen(const SokobanGame &g, const ViewState &v, Font font, app::Language lang,
                const app::ProgressRecords &r) {
    using namespace arcade;
    auto l = ui::CurrentLayout();
    bool zh = Chinese(lang);
    int best = r.Get("sokoban.moves." + std::to_string(g.LevelIndex() + 1));
    std::string stats = std::to_string(g.LevelIndex() + 1) + " / 20   " +
                        (zh ? "步数 " : "Moves ") + std::to_string(g.Board().moves) + "   " +
                        (zh ? "推动 " : "Pushes ") + std::to_string(g.Board().pushes) + "   " +
                        (zh ? "最佳 " : "Best ") + (best ? std::to_string(best) : "--");
    Header(font, zh ? "推箱子" : "Sokoban", stats, l);
    auto rect = l.Rect(board);
    DrawRectangleRec(rect, ui::palette::board);
    BeginScissorMode(int(rect.x), int(rect.y), int(rect.width), int(rect.height));
    constexpr float cell = 44, left = 102, top = 4;
    for (int i = 0; i < cellCount; ++i) {
        float x = left + (i % columns) * cell, y = top + (i / columns) * cell;
        auto b = l.Rect({board.x + x + 1, board.y + y + 1, cell - 2, cell - 2});
        DrawRectangleRec(b, g.Wall(i) ? Fade(ui::palette::accent, .25f)
                                      : Fade(ui::palette::grid, .12f));
        DrawRectangleLinesEx(b, l.scale, Fade(ui::palette::accent, .25f));
        if (g.Goal(i)) {
            DrawPolyLinesEx(Point(x + 22, y + 22, l), 4, 12 * l.scale, 0, 2 * l.scale, cyan);
            DrawCircleV(Point(x + 22, y + 22, l), 3 * l.scale, cyan);
        }
        if (g.Board().boxes[i]) {
            float bx = x, by = y;
            if (i == g.MovedBoxTo() && g.MovedBoxFrom() >= 0) {
                float fromx = left + (g.MovedBoxFrom() % columns) * cell,
                      fromy = top + (g.MovedBoxFrom() / columns) * cell;
                bx = fromx + (x - fromx) * v.animation;
                by = fromy + (y - fromy) * v.animation;
            }
            auto box = l.Rect({board.x + bx + 6, board.y + by + 6, 32, 32});
            Color c = g.Goal(i) ? cyan : gold;
            DrawRectangleRec(box, Fade(c, .25f));
            DrawRectangleLinesEx(box, 2 * l.scale, c);
            DrawPolyLinesEx(Point(bx + 22, by + 22, l), 4, 9 * l.scale, 45, l.scale, c);
        }
    }
    auto p = g.Board().player, prev = g.PreviousPlayer();
    float px = prev.x + (p.x - prev.x) * v.animation, py = prev.y + (p.y - prev.y) * v.animation;
    DrawPoly(Point(left + (px + .5f) * cell, top + (py + .5f) * cell, l), 6, 12 * l.scale, 30,
             cyan);
    Status phase = g.Phase();
    arcade::Status display = phase == Status::Ready    ? arcade::Status::Ready
                             : phase == Status::Paused ? arcade::Status::Paused
                             : phase == Status::Won    ? arcade::Status::Won
                                                       : arcade::Status::Playing;
    Overlay(display, font, lang, l,
            phase == Status::Won
                ? (g.LevelIndex() == levelCount - 1
                       ? (zh ? "全部20关完成 / Z 撤销" : "All 20 cleared / Z undo")
                       : (zh ? "空格下一关 / Z 撤销" : "Space next level / Z undo"))
            : phase == Status::Ready
                ? (zh ? "左右选关；箱子无法拉动" : "Left/Right select; boxes cannot be pulled")
                : stats);
    EndScissorMode();
    DrawRectangleLinesEx(rect, l.scale, ui::palette::accent);
    Footer(font, zh ? "方向键 / WASD 移动   Z 撤销" : "Arrows / WASD move   Z undo", lang, l);
}
} // namespace games::sokoban
