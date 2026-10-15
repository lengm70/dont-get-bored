#pragma once
#include <cstdint>
namespace games::twentyone::matchRules {
using Money = std::int64_t;
inline constexpr int maxSeats = 4, minSeats = 2, defaultRounds = 10, maxRounds = 100;
inline constexpr int defaultFunds = 1000, minFunds = 50, maxFunds = 1000000;
inline constexpr Money ante = 50, defaultRaise = 50;
inline constexpr float aiDelay = 0.85f;
}  // namespace games::twentyone::matchRules
