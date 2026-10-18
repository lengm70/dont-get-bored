#include "games/race/RaceScreen.h"
#include <cmath>
#include <cstdio>
namespace games::race {
namespace {
std::string Seconds(int ms) {
    if (ms <= 0)
        return "--";
    char out[32];
    std::snprintf(out, sizeof(out), "%.3fs", ms / 1000.0);
    return out;
}
} // namespace
void UpdateFromInput(RaceGame &g, float dt, app::ProgressRecords &r) {
    if (IsKeyPressed(KEY_R)) {
        g.Reset(g.TrackIndex());
        return;
    }
    if (g.State() == Status::Ready) {
        int d = int(IsKeyPressed(KEY_RIGHT)) - int(IsKeyPressed(KEY_LEFT));
        if (d) {
            g.Reset((g.TrackIndex() + d + 3) % 3);
            return;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            g.Start();
            return;
        }
    }
    if (IsKeyPressed(KEY_P)) {
        g.TogglePause();
        return;
    }
    Input input;
    input.throttle = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
    input.brake = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    input.steer = float(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) -
                  float(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    input.drift = IsKeyDown(KEY_SPACE);
    g.Update(dt, input);
    std::string key = "race." + std::to_string(g.TrackIndex());
    r.Best(key + ".lap", g.BestLapMs(), true);
    if (g.State() == Status::Won) {
        r.Best(key + ".time", int(std::round(g.Time() * 1000)), true);
        r.Best(key + ".clear", 1);
    }
}
void DrawScreen(const RaceGame &g, Font font, app::Language lang, const app::ProgressRecords &r) {
    using namespace arcade;
    auto l = ui::CurrentLayout();
    bool zh = Chinese(lang);
    const auto &t = GetTrack(g.TrackIndex());
    std::string stats = std::string(t.name) + "   " + (zh ? "圈数 " : "Lap ") +
                        std::to_string(std::min(3, g.CompletedLaps() + 1)) + "/3   " +
                        (zh ? "时间 " : "Time ") + Seconds(int(g.Time() * 1000));
    Header(font, zh ? "太空竞速" : "Space Race", stats, l);
    Text(font,
         std::string(zh ? "最快单圈 " : "Best lap ") +
             Seconds(r.Get("race." + std::to_string(g.TrackIndex()) + ".lap")) + "   " +
             (zh ? "最佳总用时 " : "Best total ") +
             Seconds(r.Get("race." + std::to_string(g.TrackIndex()) + ".time")) + "   " +
             std::to_string(int(g.Speed())) + " px/s",
         121, 14, cyan, l);
    auto rect = l.Rect(board);
    DrawRectangleRec(rect, ui::palette::board);
    BeginScissorMode(int(rect.x), int(rect.y), int(rect.width), int(rect.height));
    for (std::size_t i = 0; i < t.gates.size(); ++i) {
        auto a = t.gates[i], b = t.gates[(i + 1) % t.gates.size()];
        DrawLineEx(arcade::Point(a.x, a.y, l), arcade::Point(b.x, b.y, l),
                   (t.halfWidth * 2 + 5) * l.scale, Fade(ui::palette::accent, .65f));
        DrawCircleV(arcade::Point(a.x, a.y, l), (t.halfWidth + 2.5f) * l.scale,
                    Fade(ui::palette::accent, .65f));
    }
    for (std::size_t i = 0; i < t.gates.size(); ++i) {
        auto a = t.gates[i], b = t.gates[(i + 1) % t.gates.size()];
        DrawLineEx(arcade::Point(a.x, a.y, l), arcade::Point(b.x, b.y, l),
                   t.halfWidth * 2 * l.scale, {24, 25, 43, 255});
        DrawCircleV(arcade::Point(a.x, a.y, l), t.halfWidth * l.scale, {24, 25, 43, 255});
    }
    for (std::size_t i = 0; i < t.gates.size(); ++i) {
        auto a = t.gates[i], b = t.gates[(i + 1) % t.gates.size()];
        float length = std::hypot(b.x - a.x, b.y - a.y);
        for (float d = 0; d < length; d += 24) {
            float ratio = d / length;
            DrawCircleV(arcade::Point(a.x + (b.x - a.x) * ratio, a.y + (b.y - a.y) * ratio, l),
                        1.3f * l.scale, Fade(cyan, .3f));
        }
    }
    auto gate = t.gates[g.NextGate()],
         before = t.gates[(g.NextGate() + t.gates.size() - 1) % t.gates.size()];
    float dx = gate.x - before.x, dy = gate.y - before.y, len = std::hypot(dx, dy);
    dx /= len;
    dy /= len;
    DrawLineEx(arcade::Point(gate.x - dy * t.halfWidth, gate.y + dx * t.halfWidth, l),
               arcade::Point(gate.x + dy * t.halfWidth, gate.y - dx * t.halfWidth, l), 3 * l.scale,
               gold);
    for (int i = 11; i > 0; --i) {
        auto a = g.Trail()[i], b = g.Trail()[i - 1];
        DrawLineEx(arcade::Point(a.x, a.y, l), arcade::Point(b.x, b.y, l),
                   (1 - i / 12.f) * 4 * l.scale, Fade(cyan, (1 - i / 12.f) * .4f));
    }
    auto p = g.Position();
    float a = g.Heading(), c = std::cos(a), s = std::sin(a);
    Vector2 tip = arcade::Point(p.x + c * 13, p.y + s * 13, l),
            left = arcade::Point(p.x - c * 8 - s * 8, p.y - s * 8 + c * 8, l),
            right = arcade::Point(p.x - c * 8 + s * 8, p.y - s * 8 - c * 8, l);
    DrawTriangle(tip, right, left, g.Road() ? cyan : pink);
    DrawTriangleLines(tip, left, right, WHITE);
    Overlay(static_cast<arcade::Status>(g.State()), font, lang, l,
            g.State() == Status::Ready ? (zh ? "左右选赛道；按顺序通过检查点"
                                             : "Left/Right track; pass checkpoints in order")
                                       : stats);
    EndScissorMode();
    DrawRectangleLinesEx(rect, l.scale, ui::palette::accent);
    Footer(font,
           zh ? "上 / W 加速   下 / S 刹车   左右转向   空格漂移"
              : "Up/W accelerate   Down/S brake   Left/Right steer   Space drift",
           lang, l);
}
} // namespace games::race
