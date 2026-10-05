#include <array>
#include <iostream>
#include <stdexcept>

#include "raylib.h"
#include "games/breakout/BreakoutScreen.h"
#include "games/minesweeper/MinesweeperConfig.h"
#include "games/minesweeper/MinesweeperScreen.h"
#include "games/snake/SnakeScreen.h"
#include "games/tetris/TetrisScreen.h"
#include "ui/UiLayout.h"

namespace {
std::array<bool, 512> keys{};
std::array<bool, 8> buttons{};
Vector2 mouse{};
int width = 960, height = 600;
constexpr float seconds = 1.0f / 60.0f;
void Require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
void Clear() { keys.fill(false); buttons.fill(false); }
}

// Substitute input/window queries so real screen adapters are tested without a GUI.
extern "C" {
bool IsKeyPressed(int key) { return keys.at(key); }
bool IsKeyDown(int key) { return keys.at(key); }
bool IsKeyPressedRepeat(int) { return false; }
bool IsMouseButtonPressed(int button) { return buttons.at(button); }
Vector2 GetMousePosition() { return mouse; }
int GetScreenWidth() { return width; }
int GetScreenHeight() { return height; }
}

int main() {
    try {
        games::snake::SnakeGame snake(42);
        keys[KEY_SPACE] = true;
        games::snake::UpdateFromInput(snake, seconds);
        Require(snake.State() == games::snake::Status::Playing, "Snake starts with Space");
        Clear();
        games::tetris::TetrisGame tetris(42);
        keys[KEY_ENTER] = true;
        games::tetris::UpdateFromInput(tetris, seconds);
        Require(tetris.State() == games::tetris::Status::Ready, "Enter no longer starts Tetris");
        const auto first = tetris.Active();
        Clear();
        keys[KEY_SPACE] = true;
        games::tetris::UpdateFromInput(tetris, seconds);
        Require(tetris.State() == games::tetris::Status::Playing && tetris.Score() == 0 &&
                tetris.Active().y == first.y && tetris.Active().type == first.type,
                "Starting Tetris must not also hard-drop");
        games::tetris::UpdateFromInput(tetris, seconds);
        Require(tetris.Score() > 0, "Space still hard-drops during play");
        Clear();
        games::breakout::BreakoutGame breakout;
        const auto ball = breakout.Ball();
        keys[KEY_SPACE] = true;
        games::breakout::UpdateFromInput(breakout, seconds);
        Require(breakout.State() == games::breakout::Status::Playing &&
                breakout.Ball().y == ball.y, "Space starts Breakout without double action");
        Clear();
        games::breakout::UpdateFromInput(breakout, seconds);
        Require(breakout.Ball().y < ball.y, "Served ball moves on subsequent frames");
        keys[KEY_P] = true;
        games::breakout::UpdateFromInput(breakout, seconds);
        Require(breakout.State() == games::breakout::Status::Paused, "P pauses Breakout");

        namespace mines = games::minesweeper;
        for (const auto size : std::array<std::array<int, 2>, 4>{{
                 {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}}}) {
            Clear(); width = size[0]; height = size[1];
            mines::MinesweeperGame game(42);
            const auto layout = ui::CurrentLayout();
            mouse = layout.Point(mines::config::boardX + 4.5f * mines::config::cellSize,
                                  mines::config::boardY + 4.5f * mines::config::cellSize);
            buttons[MOUSE_BUTTON_LEFT] = true;
            mines::UpdateFromInput(game, seconds);
            Require(game.RevealedSafeCells() == 0, "Mouse cannot bypass Space start");
            keys[KEY_SPACE] = true;
            mines::UpdateFromInput(game, seconds);
            Require(game.State() == mines::Status::Playing && game.RevealedSafeCells() == 0,
                    "Space starts Minesweeper without revealing a cell");
            Clear(); buttons[MOUSE_BUTTON_LEFT] = true;
            mines::UpdateFromInput(game, seconds);
            Require(game.At(4, 4).revealed, "Mouse maps correctly at every supported resolution");
            const int revealed = game.RevealedSafeCells();
            mouse = layout.Point(mines::config::boardX + mines::rules::columns * mines::config::cellSize,
                                  mines::config::boardY + mines::rules::rows * mines::config::cellSize);
            mines::UpdateFromInput(game, seconds);
            Require(game.RevealedSafeCells() == revealed, "Right/bottom edges are outside input bounds");
            Clear(); keys[KEY_P] = true;
            mines::UpdateFromInput(game, seconds);
            Require(game.State() == mines::Status::Paused, "P pauses Minesweeper");
            Clear(); keys[KEY_R] = true;
            mines::UpdateFromInput(game, seconds);
            Require(game.State() == mines::Status::Ready && game.RevealedSafeCells() == 0,
                    "R returns Minesweeper to Space-ready state");
        }
        std::cout << "Space start and scaled mouse input tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
