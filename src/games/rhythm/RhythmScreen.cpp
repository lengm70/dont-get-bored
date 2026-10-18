#include "games/rhythm/RhythmScreen.h"
#include <cmath>
#include <cstdio>
namespace games::rhythm {
void UpdateFromInput(RhythmGame &g, RhythmAudio &audio, ViewState &v, float dt, float volume,
                     app::ProgressRecords &r) {
    v.feedback = std::max(0.f, v.feedback - dt);
    for (auto &f : v.laneGlow)
        f = std::max(0.f, f - dt);
    if (IsKeyPressed(KEY_R)) {
        audio.Stop();
        g.Reset(g.TrackIndex());
        v.audioFailed = false;
        return;
    }
    if (g.State() == Status::Ready) {
        int direction = int(IsKeyPressed(KEY_RIGHT)) - int(IsKeyPressed(KEY_LEFT));
        if (direction) {
            g.Reset((g.TrackIndex() + direction + trackCount) % trackCount);
            return;
        }
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
            v.offsetMs = std::clamp(v.offsetMs + (IsKeyPressed(KEY_UP) ? 10 : -10), -150, 150);
            r.Set("rhythm.offset", v.offsetMs + 200);
            return;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            v.audioFailed = !audio.Start(g.TrackIndex(), volume);
            if (!v.audioFailed)
                g.Start();
            return;
        }
    }
    if (IsKeyPressed(KEY_P)) {
        g.TogglePause();
        audio.Pause(g.State() == Status::Paused);
        return;
    }
    audio.Update();
    if (g.State() != Status::Playing)
        return;
    g.Advance(std::max(0.0, audio.Time() + v.offsetMs / 1000.0));
    const int keys[]{KEY_D, KEY_F, KEY_J, KEY_K};
    for (int i = 0; i < 4; ++i)
        if (IsKeyPressed(keys[i])) {
            v.judgment = g.Hit(i);
            v.feedback = .3f;
            v.laneGlow[i] = .15f;
        }
    if (g.State() == Status::Won) {
        audio.Pause(true);
        std::string prefix = "rhythm." + std::to_string(g.TrackIndex());
        r.Best(prefix + ".score", g.Score());
        r.Best(prefix + ".accuracy", int(std::round(g.Accuracy() * 100)));
        r.Best(prefix + ".clear", 1);
    }
}
void DrawScreen(const RhythmGame &g, const ViewState &v, Font font, app::Language lang,
                const app::ProgressRecords &r) {
    using namespace arcade;
    auto l = ui::CurrentLayout();
    bool zh = Chinese(lang);
    char accuracy[24];
    std::snprintf(accuracy, sizeof(accuracy), "%.1f%%", g.Accuracy());
    std::string stats = std::string(GetTrack(g.TrackIndex()).name) + "   BPM " +
                        std::to_string(GetTrack(g.TrackIndex()).bpm) + "   " +
                        (zh ? "分数 " : "Score ") + std::to_string(g.Score()) + "   " +
                        (zh ? "连击 " : "Combo ") + std::to_string(g.Combo());
    Header(font, zh ? "节奏点击" : "Rhythm", stats, l);
    Text(font,
         std::string(zh ? "音准 " : "Accuracy ") + accuracy + "   " +
             (zh ? "最高连击 " : "Max combo ") + std::to_string(g.MaxCombo()) + "   " +
             (zh ? "最佳 " : "Best ") +
             std::to_string(r.Get("rhythm." + std::to_string(g.TrackIndex()) + ".score")),
         121, 14, cyan, l);
    auto rect = l.Rect(board);
    DrawRectangleRec(rect, ui::palette::board);
    BeginScissorMode(int(rect.x), int(rect.y), int(rect.width), int(rect.height));
    constexpr float left = 100, laneWidth = 100;
    const char *names[]{"D", "F", "J", "K"};
    for (int i = 0; i < 4; ++i) {
        auto lane = l.Rect({board.x + left + i * laneWidth, board.y, 98, 360});
        Color c = i % 2 ? pink : cyan;
        DrawRectangleRec(lane, Fade(c, .04f + v.laneGlow[i]));
        DrawRectangleLinesEx(lane, l.scale, Fade(c, .25f));
        ui::DrawScaledText(font, names[i], board.x + left + i * laneWidth + 40, board.y + 318, 18,
                           c, l);
    }
    for (const auto &n : g.Notes())
        if (n.result < 0) {
            float y = hitLine - float(n.time - g.Time()) * noteSpeed;
            if (y < -15 || y > 320)
                continue;
            auto note = l.Rect({board.x + left + n.lane * laneWidth + 8, board.y + y - 6, 82, 12});
            Color c = n.lane % 2 ? pink : cyan;
            DrawRectangleRec(note, Fade(c, .7f));
            DrawRectangleLinesEx(note, l.scale, WHITE);
        }
    DrawLineEx(Point(left, hitLine, l), Point(left + 400, hitLine, l), 3 * l.scale, gold);
    if (v.feedback > 0)
        Text(font,
             v.judgment == 100  ? "PERFECT"
             : v.judgment == 60 ? "GOOD"
                                : "EMPTY",
             210, 24, Fade(gold, v.feedback / .3f), l);
    if (g.State() == Status::Won) {
        DrawRectangleRec(rect, ui::palette::overlay);
        Text(font, std::string("GRADE ") + g.Grade(), 238, 34, cyan, l);
        Text(font,
             std::string(accuracy) + "   PERFECT " + std::to_string(g.Perfects()) + " / GOOD " +
                 std::to_string(g.Goods()) + " / MISS " + std::to_string(g.Misses()),
             296, 17, ui::palette::text, l);
        Text(font, zh ? "按 R 重新选曲" : "R to choose again", 365, 18, ui::palette::muted, l);
    } else if (g.State() != Status::Playing)
        Overlay(g.State() == Status::Ready ? arcade::Status::Ready : arcade::Status::Paused, font,
                lang, l,
                v.audioFailed ? (zh ? "播放失败，请检查素材" : "Audio unavailable; check assets")
                              : (zh ? "左右选曲 / 上下调整判定延迟"
                                    : "Left/Right track / Up/Down timing offset"));
    EndScissorMode();
    DrawRectangleLinesEx(rect, l.scale, ui::palette::accent);
    Footer(font,
           (std::string("D F J K    ") + (zh ? "判定延迟 " : "Timing offset ") +
            std::to_string(v.offsetMs) + " ms")
               .c_str(),
           lang, l);
}
} // namespace games::rhythm
