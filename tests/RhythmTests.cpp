#include "games/rhythm/RhythmGame.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace games::rhythm;
void Check(bool b, const char *m) {
    if (!b)
        throw std::runtime_error(m);
}
int main() {
    try {
        for (int track = 0; track < trackCount; ++track) {
            RhythmGame g;
            g.Reset(track);
            Check(g.Hit(0) == -1, "Ready cannot score");
            g.Start();
            auto notes = g.Notes();
            for (auto &n : notes) {
                g.Advance(n.time);
                Check(g.Hit(n.lane) == 100, "Exact beat is perfect");
            }
            g.Advance(notes.back().time + 1);
            Check(g.State() == Status::Won && g.Accuracy() > 99.99 &&
                      g.MaxCombo() == int(notes.size()) && std::string(g.Grade()) == "S",
                  "Perfect full chart");
            g.Reset(track);
            g.Start();
            g.Advance(100);
            Check(g.Misses() == int(notes.size()) && g.Accuracy() == 0, "All misses counted");
        }
        RhythmGame g;
        g.Start();
        auto n = g.Notes().front();
        g.Advance(n.time + .1);
        Check(g.Hit(n.lane) == 60, "Good timing");
        Check(g.Hit(n.lane) == 0 && g.Ghosts() == 1 && g.Combo() == 0,
              "No repeat scoring and spam penalty");
        g.TogglePause();
        double before = g.Time();
        g.Advance(100);
        Check(g.Time() == before, "Pause clock");
        g.TogglePause();
        g.Advance(-1);
        Check(g.Time() == before, "Clock cannot go backwards");
        std::cout << "3 charts and timing judgments passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
