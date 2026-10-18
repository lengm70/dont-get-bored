#include "games/rhythm/RhythmCharts.h"
#include <algorithm>
namespace games::rhythm {
const Track &GetTrack(int index) {
    static const Track tracks[] = {{"Neon Orbit", "assets/rhythm/neon.wav", 96},
                                   {"Ion Drift", "assets/rhythm/ion.wav", 120},
                                   {"Nova Pulse", "assets/rhythm/nova.wav", 144}};
    return tracks[std::clamp(index, 0, 2)];
}
std::vector<Note> MakeChart(int track) {
    track = std::clamp(track, 0, 2);
    double beat = 60.0 / GetTrack(track).bpm;
    std::vector<Note> notes;
    for (int n = 4; n < beats; ++n) {
        int lane = (n * 3 + n / 8 + track) % 4;
        notes.push_back({2 + n * beat, lane});
        if (track >= 1 && n % 8 == 5)
            notes.push_back({2 + (n + .5) * beat, (lane + 1) % 4});
        if (track == 2 && n % 8 == 0)
            notes.push_back({2 + n * beat, (lane + 2) % 4});
    }
    std::sort(notes.begin(), notes.end(),
              [](const Note &a, const Note &b) { return a.time < b.time; });
    return notes;
}
} // namespace games::rhythm
