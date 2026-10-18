#include "config/CollectionProgress.h"
#include <iostream>
#include <stdexcept>
void Check(bool b, const char *m) {
    if (!b)
        throw std::runtime_error(m);
}
int main() {
    try {
        app::HighScores h;
        app::ProgressRecords r;
        games::breakout::Progress b;
        auto fresh = app::CollectionProgress(h, r, b);
        Check(fresh.size() == 12 && app::CompletedGoals(fresh) == 0,
              "Fresh profile has no fake achievements");
        h.snake = 100;
        h.tetris = 1000;
        h.fruit = 1000;
        h.minesweeperNormalMilliseconds = 45000;
        for (auto &score : b.levelBest)
            score = 10;
        for (const char *key : {"gomoku.clear", "twentyone.clear", "bomber.clear", "tank.clear"})
            r.Best(key, 1);
        for (int i = 1; i <= 20; ++i)
            r.Best("sokoban.moves." + std::to_string(i), i + 2, true);
        for (int i = 0; i < 3; ++i) {
            r.Best("rhythm." + std::to_string(i) + ".clear", 1);
            r.Best("race." + std::to_string(i) + ".clear", 1);
        }
        auto complete = app::CollectionProgress(h, r, b);
        Check(app::CompletedGoals(complete) == 12, "All twelve goals independently recognized");
        app::ProgressRecords empty;
        auto migrated = app::CollectionProgress(h, empty, b);
        Check(app::CompletedGoals(migrated) == 5,
              "Legacy scores migrate read-only without inventing new-game completions");
        std::cout << "12 collection goals and migration passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
