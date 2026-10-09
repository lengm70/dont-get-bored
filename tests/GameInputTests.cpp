#include <array>
#include <iostream>
#include <stdexcept>

#include "raylib.h"
#include "games/fruit/FruitScreen.h"
#include "games/breakout/BreakoutScreen.h"
#include "games/minesweeper/MinesweeperConfig.h"
#include "games/minesweeper/MinesweeperLayout.h"
#include "games/minesweeper/MinesweeperScreen.h"
#include "games/snake/SnakeScreen.h"
#include "games/tetris/TetrisScreen.h"
#include "games/gomoku/GomokuScreen.h"
#include "games/gomoku/GomokuConfig.h"
#include "ui/UiLayout.h"
#include "ui/MusicPlayerInput.h"

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
Vector2 GetMouseDelta() { return {}; }
Vector2 GetMousePosition() { return mouse; }
int GetScreenWidth() { return width; }
int GetScreenHeight() { return height; }
}

int main() {
    try {
        games::fruit::FruitGame fruitGame(42);
        Clear(); keys[KEY_SPACE] = true;
        games::fruit::UpdateFromInput(fruitGame, seconds);
        Require(fruitGame.State() == games::fruit::Status::Playing && fruitGame.Fruits().empty(), "Fruit Space starts without dropping");
        Clear(); buttons[MOUSE_BUTTON_LEFT] = true; mouse = {480, 300};
        games::fruit::UpdateFromInput(fruitGame, seconds, true);
        Require(fruitGame.Fruits().empty(), "Player-captured click cannot drop fruit");
        Clear(); keys[KEY_RIGHT] = true;
        const float previousAim = fruitGame.Aim();
        games::fruit::UpdateFromInput(fruitGame, seconds);
        Require(fruitGame.Aim() > previousAim, "Fruit keyboard aiming works with stationary cursor");
        Clear(); keys[KEY_SPACE] = true;
        games::fruit::UpdateFromInput(fruitGame, seconds);
        Require(fruitGame.Fruits().size() == 1, "Fruit Space drops after starting");
        Clear(); keys[KEY_P] = true;
        games::fruit::UpdateFromInput(fruitGame, seconds);
        Require(fruitGame.State() == games::fruit::Status::Paused, "Fruit pause input");
        Clear(); keys[KEY_R] = true;
        games::fruit::UpdateFromInput(fruitGame, seconds);
        Require(fruitGame.State() == games::fruit::Status::Ready && fruitGame.Fruits().empty(), "Fruit restart returns Ready");
        namespace gomoku = games::gomoku;
        gomoku::GomokuGame gomokuGame;
        gomoku::GomokuAiTurn ai;
        Clear(); buttons[MOUSE_BUTTON_LEFT] = true;
        mouse = ui::CurrentLayout().Point(gomoku::config::boardX, gomoku::config::boardY);
        gomoku::UpdateFromInput(gomokuGame, ai, gomoku::Strength::Easy);
        Require(gomokuGame.MoveCount() == 0, "Mouse cannot bypass Gomoku Space start");
        keys[KEY_SPACE] = true;
        gomoku::UpdateFromInput(gomokuGame, ai, gomoku::Strength::Easy);
        Require(gomokuGame.State() == gomoku::Status::Playing && gomokuGame.MoveCount() == 0,
                "Gomoku Space start does not place a stone");
        for (const auto size : std::array<std::array<int, 2>, 4>{{
                 {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}}}) {
            width = size[0]; height = size[1];
            const auto layout = ui::CurrentLayout();
            for (gomoku::Move cell : {gomoku::Move{0, 0}, {7, 7}, {14, 14}}) {
                const auto point = layout.Point(gomoku::config::boardX + cell.x * gomoku::config::spacing,
                                               gomoku::config::boardY + cell.y * gomoku::config::spacing);
                const auto mapped = gomoku::MoveFromPoint(point, layout);
                Require(mapped && mapped->x == cell.x && mapped->y == cell.y, "Gomoku intersections map at every resolution");
            }
            Require(!gomoku::MoveFromPoint(layout.Point(gomoku::config::boardX - gomoku::config::spacing,
                                                       gomoku::config::boardY), layout), "Outside Gomoku board rejected");
        }
        width = 960; height = 600;
        Clear(); buttons[MOUSE_BUTTON_LEFT] = true;
        mouse = ui::CurrentLayout().Point(gomoku::config::boardX, gomoku::config::boardY);
        gomoku::UpdateFromInput(gomokuGame, ai, gomoku::Strength::Easy, true);
        Require(gomokuGame.MoveCount() == 0, "Captured player mouse cannot place a Gomoku stone");
        gomoku::UpdateFromInput(gomokuGame, ai, gomoku::Strength::Easy);
        Require(gomokuGame.At(0, 0) == gomoku::Stone::Black, "Mouse places on intersection");
        Clear(); keys[KEY_R] = true;
        gomoku::UpdateFromInput(gomokuGame, ai, gomoku::Strength::Easy);
        Require(gomokuGame.State() == gomoku::Status::Ready && gomokuGame.MoveCount() == 0, "R returns Gomoku to ready state");
        Clear();
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
        for (const auto size : std::array<std::array<int, 2>, 5>{{
                 {960, 600}, {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}}}) {
            Clear(); width = size[0]; height = size[1];
            ui::MusicPlayerInput player;
            const auto layout = ui::CurrentLayout();
            for (const auto edge : {ui::PlayerDock::Left, ui::PlayerDock::Right, ui::PlayerDock::Top, ui::PlayerDock::Bottom}) {
                ui::MusicPlayerInput docked;
                const Vector2 center = layout.Point(730, 454);
                docked.Update(layout, center, true, true);
                Vector2 destination{width / 2.0f, height / 2.0f};
                if (edge == ui::PlayerDock::Left) destination.x = 0;
                if (edge == ui::PlayerDock::Right) destination.x = static_cast<float>(width);
                if (edge == ui::PlayerDock::Top) destination.y = 0;
                if (edge == ui::PlayerDock::Bottom) destination.y = static_cast<float>(height);
                docked.Update(layout, destination, false, true);
                Require(docked.Update(layout, destination, false, false), "Dock release captures underlying input");
                Require(docked.Collapsed() && docked.Dock() == edge, "Player docks to nearest edge");
                const Rectangle tab = layout.Rect(docked.VisibleBounds());
                Require(tab.x >= -0.1f && tab.y >= -0.1f && tab.x + tab.width <= width + 0.1f && tab.y + tab.height <= height + 0.1f, "Collapsed tab stays in actual window bounds");
                const auto full = layout.Rect(docked.DesignBounds());
                Require(!docked.Update(layout, {full.x + full.width / 2, full.y + full.height / 2}, false, false), "Hidden player area does not capture game input");
                const Vector2 arrow{tab.x + tab.width / 2, tab.y + tab.height / 2};
                Require(docked.Update(layout, arrow, true, true) && !docked.Collapsed(), "Arrow click expands player without passing through");
                Require(docked.SuppressControls(), "Expansion press cannot activate player buttons");
                Require(docked.Update(layout, arrow, false, false), "Expansion release captured");
                Require(docked.SuppressControls(), "Expansion release cannot activate player buttons");
                docked.Update(layout, layout.Point(300, 250), false, false);
                Require(!docked.Collapsed(), "Expanded player does not immediately collapse again");
            }
            mines::MinesweeperGame game(42);
            game.Configure(mines::Difficulty::Hard);
            game.Start();
            mouse = layout.Point(750, 500);
            buttons[MOUSE_BUTTON_LEFT] = true;
            mines::UpdateFromInput(game, seconds, player.Update(layout, mouse, true, true));
            Require(game.RevealedSafeCells() == 0, "Player click must not reveal the board");
            Clear(); buttons[MOUSE_BUTTON_RIGHT] = true;
            mines::UpdateFromInput(game, seconds, player.Update(layout, mouse, false, false));
            Require(game.RemainingMines() == game.MineCount(), "Player right click must not flag the board");
            Require(player.Update(layout, layout.Point(730, 454), true, true), "Header captures drag");
            Require(player.Update(layout, layout.Point(300, 250), false, true), "Drag stays captured outside old panel");
            Require(player.Update(layout, layout.Point(300, 250), false, false), "Drag release stays captured");
            mouse = layout.Point(320, 296);
            Clear(); buttons[MOUSE_BUTTON_LEFT] = true;
            mines::UpdateFromInput(game, seconds, player.Update(layout, mouse, true, true));
            Require(game.RevealedSafeCells() == 0, "Moved player must use its new bounds");
            player.Update(layout, mouse, false, false);
            Require(!player.Update(layout, layout.Point(750, 500), false, false), "Old player position no longer captures input");
            mines::UpdateFromInput(game, seconds, false);
            Require(game.RevealedSafeCells() > 0, "Uncaptured board input still works");
            Clear(); keys[KEY_P] = true;
            mines::UpdateFromInput(game, seconds, true);
            Require(game.State() == mines::Status::Paused, "Captured mouse does not suppress keyboard input");
        }
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
        for (const auto dimensions : std::array<std::array<int, 2>, 4>{{
                 {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}}}) {
            width = dimensions[0]; height = dimensions[1];
            for (const auto level : {mines::Difficulty::Normal, mines::Difficulty::Hard, mines::Difficulty::Custom}) {
                Clear();
                mines::MinesweeperGame game(42);
                Require(game.Configure(level, {40, 30, 200}), "Configure scaled board");
                game.Start();
                const auto board = mines::LayoutFor(game);
                const auto layout = ui::CurrentLayout();
                const auto last = board.Cell(game.Columns() - 1, game.Rows() - 1);
                mouse = layout.Point(last.x + last.width / 2, last.y + last.height / 2);
                buttons[MOUSE_BUTTON_RIGHT] = true;
                mines::UpdateFromInput(game, seconds);
                Require(game.At(game.Columns() - 1, game.Rows() - 1).flagged,
                        "Dynamic boards map the last cell correctly at each resolution");
                mouse = layout.Point(board.bounds.x + board.bounds.width,
                                     board.bounds.y + board.bounds.height);
                mines::UpdateFromInput(game, seconds);
                Require(game.RemainingMines() == game.MineCount() - 1,
                        "Dynamic board right/bottom edge is rejected");
            }
        }
        std::cout << "Space start and scaled mouse input tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
