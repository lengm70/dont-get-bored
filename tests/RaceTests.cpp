#include "games/race/RaceGame.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace games::race;
void Check(bool b, const char *m) {
    if (!b)
        throw std::runtime_error(m);
}
int main() {
    try {
        for (int track = 0; track < 3; ++track) {
            RaceGame g;
            g.Reset(track);
            auto initial = g.Position();
            g.Update(.2f, {1, 1});
            Check(g.Position().x == initial.x, "Ready");
            g.Start();
            for (int f = 0; f < 24000 && g.State() == Status::Playing; ++f) {
                auto target = GetTrack(track).gates[g.NextGate()], p = g.Position();
                float angle = std::atan2(target.y - p.y, target.x - p.x),
                      error = std::remainder(angle - g.Heading(), 6.2831853f);
                Input in;
                in.steer = std::clamp(error * 2, -1.f, 1.f);
                in.throttle = std::abs(error) > .8f ? .4f : 1.f;
                in.brake = std::abs(error) > 1.2f && g.Speed() > 80;
                g.Update(1.f / 120, in);
            }
            if (g.State() != Status::Won)
                std::cerr << "track " << track << " laps " << g.CompletedLaps() << " gate "
                          << g.NextGate() << " pos " << g.Position().x << "," << g.Position().y
                          << '\n';
            Check(g.State() == Status::Won && g.CompletedLaps() == 3 && g.BestLapMs() > 0,
                  "All three circuits must complete with ordinary controls");
        }
        RaceGame g;
        g.Start();
        for (int i = 0; i < 200; ++i)
            g.Update(1.f / 120, {1, 0});
        g.TogglePause();
        auto p = g.Position();
        double t = g.Time();
        g.Update(.2f, {1, 1});
        Check(g.Position().x == p.x && g.Time() == t, "Pause freezes time and physics");
        RaceGame a, b;
        a.Start();
        b.Start();
        for (int i = 0; i < 30; ++i)
            a.Update(1.f / 30, {1, .2f});
        for (int i = 0; i < 240; ++i)
            b.Update(1.f / 240, {1, .2f});
        Check(std::hypot(a.Position().x - b.Position().x, a.Position().y - b.Position().y) < .2f,
              "FPS invariant");
        Check(a.CompletedLaps() == 0, "No stationary/start-line lap exploit");
        std::cout << "3 tracks / 3 laps / control physics passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
