#include "app/SessionGuard.h"
#include <iostream>
#include <stdexcept>
void Check(bool b, const char *m) {
    if (!b)
        throw std::runtime_error(m);
}
int main() {
    try {
        using namespace app;
        SessionGuard guard;
        guard.Update(Screen::MainMenu, false, true, true);
        Check(!guard.Blocked(), "Menus do not inherit gameplay pause");
        for (auto screen : {Screen::Snake, Screen::Tetris, Screen::Breakout, Screen::Minesweeper,
                            Screen::Gomoku, Screen::Fruit, Screen::TwentyOne, Screen::Bomber,
                            Screen::Tank, Screen::Sokoban, Screen::Rhythm, Screen::Race}) {
            guard.Update(screen, true, false, false);
            Check(!guard.Blocked(), "New game clears overlays");
            guard.Update(screen, false, false, false);
            Check(guard.Blocked(), "Focus loss pauses every game");
            guard.Update(screen, true, false, false);
            Check(guard.Blocked(), "Focus restoration never auto-resumes");
            Check(guard.Update(screen, true, true, false) && !guard.Blocked(),
                  "Resume consumes its key frame");
            guard.Update(screen, true, false, true);
            Check(guard.HelpOpen() && guard.Blocked(), "Help blocks gameplay");
            Check(guard.CloseHelp() && !guard.Blocked(), "Close help restores prior running state");
            guard.Update(screen, true, true, false);
            guard.Update(screen, true, false, true);
            guard.CloseHelp();
            Check(guard.Blocked(), "Closing help preserves manual pause");
        }
        guard.Restart();
        Check(!guard.Blocked(), "Restart clears help and pause");
        guard.Update(Screen::GameSelection, true, false, false);
        Check(!guard.Blocked(), "Leaving a game resets session pause");
        std::cout << "12-game pause, help and focus policy passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
