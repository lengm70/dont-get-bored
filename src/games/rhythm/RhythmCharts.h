#pragma once
#include <vector>
namespace games::rhythm {
inline constexpr int trackCount=3,beats=64;
inline constexpr float perfectWindow=.065f,goodWindow=.15f,noteSpeed=140,hitLine=300;
struct Note{double time;int lane;int result=-1;};
struct Track{const char* name;const char* path;int bpm;};
const Track& GetTrack(int track);std::vector<Note> MakeChart(int track);
}
