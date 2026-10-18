#include "games/race/RaceTrack.h"
#include <algorithm>
#include <cmath>
namespace games::race {
namespace {
Track Oval() {
    Track t{"Orbit Ring", 34, {}};
    for (int i = 0; i < 16; ++i) {
        float a = i * 6.2831853f / 16;
        t.gates.push_back({300 + 230 * std::cos(a), 180 + 118 * std::sin(a)});
    }
    return t;
}
} // namespace
const Track &GetTrack(int i) {
    static const Track tracks[]{Oval(),
                                {"Ion Chicane",
                                 26,
                                 {{510, 180},
                                  {500, 280},
                                  {370, 285},
                                  {340, 220},
                                  {220, 220},
                                  {200, 285},
                                  {90, 280},
                                  {80, 180},
                                  {90, 80},
                                  {210, 80},
                                  {230, 140},
                                  {350, 140},
                                  {370, 80},
                                  {500, 80}}},
                                {"Nova Circuit",
                                 29,
                                 {{530, 180},
                                  {460, 300},
                                  {300, 265},
                                  {130, 300},
                                  {70, 180},
                                  {140, 60},
                                  {300, 95},
                                  {470, 60}}}};
    return tracks[std::clamp(i, 0, 2)];
}
float SegmentDistance(Point p, Point a, Point b) {
    float dx = b.x - a.x, dy = b.y - a.y, length = dx * dx + dy * dy;
    float t = length ? std::clamp(((p.x - a.x) * dx + (p.y - a.y) * dy) / length, 0.f, 1.f) : 0;
    return std::hypot(p.x - a.x - t * dx, p.y - a.y - t * dy);
}
bool OnRoad(const Track &t, Point p) {
    for (std::size_t i = 0; i < t.gates.size(); ++i)
        if (SegmentDistance(p, t.gates[i], t.gates[(i + 1) % t.gates.size()]) <= t.halfWidth)
            return true;
    return false;
}
} // namespace games::race
