#include "config/CollectionProgress.h"
#include <algorithm>
#include <cstdio>
namespace app {
namespace {
std::string Score(int n) {
    return n > 0 ? std::to_string(n) : "--";
}
std::string Time(int n) {
    if (n <= 0)
        return "--";
    char out[40];
    std::snprintf(out, sizeof(out), "%.3f s", n / 1000.0);
    return out;
}
} // namespace
std::array<CollectionEntry, 12> CollectionProgress(const HighScores &h, const ProgressRecords &r,
                                                   const games::breakout::Progress &b) {
    int boxes = 0, songs = 0, circuits = 0;
    for (int i = 1; i <= 20; ++i)
        boxes += r.Get("sokoban.moves." + std::to_string(i)) > 0;
    for (int i = 0; i < 3; ++i) {
        songs += r.Get("rhythm." + std::to_string(i) + ".clear") > 0;
        circuits += r.Get("race." + std::to_string(i) + ".clear") > 0;
    }
    int cleared = 0;
    for (int value : b.levelBest)
        cleared += value > 0;
    bool mines = h.minesweeperEasyMilliseconds > 0 || h.minesweeperNormalMilliseconds > 0 ||
                 h.minesweeperHardMilliseconds > 0;
    return {
        {{"贪吃蛇", "Snake", "获得100分", "Reach 100 points", h.snake, 100, Score(h.snake)},
         {"俄罗斯方块", "Tetris", "获得1000分", "Reach 1000 points", h.tetris, 1000,
          Score(h.tetris)},
         {"打砖块", "Breakout", "完成全部5关", "Clear all 5 sectors", cleared, 5,
          Score(std::max(h.breakout, b.bestScore))},
         {"扫雷", "Minesweeper", "完成任一预设难度", "Win a preset difficulty", mines ? 1 : 0, 1,
          "E " + Time(h.minesweeperEasyMilliseconds) + " / N " +
              Time(h.minesweeperNormalMilliseconds) + " / H " +
              Time(h.minesweeperHardMilliseconds)},
         {"五子棋", "Gomoku", "赢下一盘对局", "Win one game", r.Get("gomoku.clear"), 1,
          r.Get("gomoku.clear") ? "WIN" : "--"},
         {"合成大西瓜", "Fruit Merge", "获得1000分", "Reach 1000 points", h.fruit, 1000,
          Score(h.fruit)},
         {"二十一点", "Twenty One", "完成一次多轮对局", "Finish a complete match",
          r.Get("twentyone.clear"), 1, Score(r.Get("twentyone.funds"))},
         {"炸弹人", "Bomber", "完成单人三关", "Clear all 3 solo sectors", r.Get("bomber.clear"), 1,
          Score(h.bomber)},
         {"坦克大战", "Tank Battle", "保护基地完成三波", "Survive all 3 waves", r.Get("tank.clear"),
          1, Score(std::max(r.Get("tank.best"), r.Get("tank.coop.best")))},
         {"推箱子", "Sokoban", "完成全部20关", "Solve all 20 puzzles", boxes, 20,
          std::to_string(boxes) + " / 20"},
         {"节奏点击", "Rhythm", "完成全部三首曲目", "Finish all 3 tracks", songs, 3,
          std::to_string(songs) + " / 3"},
         {"太空竞速", "Space Race", "完成三条赛道", "Finish all 3 circuits", circuits, 3,
          "1 " + Time(r.Get("race.0.time")) + " / 2 " + Time(r.Get("race.1.time")) + " / 3 " +
              Time(r.Get("race.2.time"))}}};
}
int CompletedGoals(const std::array<CollectionEntry, 12> &entries) {
    return int(std::count_if(entries.begin(), entries.end(),
                             [](const CollectionEntry &e) { return e.Complete(); }));
}
} // namespace app
