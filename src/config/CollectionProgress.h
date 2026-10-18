#pragma once
#include "config/HighScores.h"
#include "config/ProgressRecords.h"
#include "games/breakout/BreakoutProgress.h"
#include <array>
#include <string>
namespace app {
struct CollectionEntry {
    const char *chineseName;
    const char *englishName;
    const char *chineseGoal;
    const char *englishGoal;
    int value, target;
    std::string record;
    bool Complete() const {
        return value >= target;
    }
};
std::array<CollectionEntry, 12> CollectionProgress(const HighScores &high,
                                                   const ProgressRecords &records,
                                                   const games::breakout::Progress &breakout);
int CompletedGoals(const std::array<CollectionEntry, 12> &entries);
} // namespace app
