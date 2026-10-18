#pragma once
#include "app/Screen.h"
namespace app {
inline bool IsGameScreen(Screen screen) {
    switch (screen) {
    case Screen::Snake:
    case Screen::Tetris:
    case Screen::Breakout:
    case Screen::Minesweeper:
    case Screen::Gomoku:
    case Screen::Fruit:
    case Screen::TwentyOne:
    case Screen::Bomber:
    case Screen::Tank:
    case Screen::Sokoban:
    case Screen::Rhythm:
    case Screen::Race:
        return true;
    default:
        return false;
    }
}
// Screen changes reset temporary overlays. Focus returning never resumes play by itself.
class SessionGuard {
  public:
    bool Update(Screen screen, bool focused, bool pausePressed, bool helpPressed) {
        bool changed = screen != screen_;
        if (changed) {
            screen_ = screen;
            paused_ = help_ = false;
        }
        if (!IsGameScreen(screen)) {
            paused_ = help_ = false;
            return false;
        }
        bool consumed = false;
        if (helpPressed) {
            help_ = !help_;
            consumed = true;
        }
        if (pausePressed && !help_) {
            paused_ = !paused_;
            consumed = true;
        }
        if (!focused)
            paused_ = true;
        return consumed;
    }
    void Restart() {
        paused_ = help_ = false;
    }
    bool Blocked() const {
        return paused_ || help_;
    }
    bool HelpOpen() const {
        return help_;
    }
    bool CloseHelp() {
        if (!help_)
            return false;
        help_ = false;
        return true;
    }

  private:
    Screen screen_ = Screen::MainMenu;
    bool paused_ = false, help_ = false;
};
} // namespace app
