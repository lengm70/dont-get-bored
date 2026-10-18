#pragma once
#include <vector>
namespace games::race {
struct Point {
    float x = 0, y = 0;
};
struct Track {
    const char *name;
    float halfWidth;
    std::vector<Point> gates;
};
const Track &GetTrack(int index);
float SegmentDistance(Point point, Point a, Point b);
bool OnRoad(const Track &track, Point point);
} // namespace games::race
