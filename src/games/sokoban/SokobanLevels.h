#pragma once
#include <array>
#include <string>
namespace games::sokoban {
inline constexpr int columns = 9, rows = 8, cellCount = columns * rows, levelCount = 20;
struct Level {
    std::array<const char *, rows> map;
    const char *solution;
};
const Level &GetLevel(int index);
} // namespace games::sokoban
