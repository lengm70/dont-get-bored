#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "config/HighScores.h"
#include "config/AppSettings.h"
#include "games/breakout/BreakoutGame.h"
#include "games/minesweeper/MinesweeperGame.h"
#include "games/snake/SnakeGame.h"
#include "games/tetris/TetrisGame.h"

namespace {

void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void TestMinesweeper() {
    namespace mines = games::minesweeper;
    for (unsigned seed = 0; seed < 30; ++seed) {
        mines::MinesweeperGame game(seed);
        game.Reveal(4, 4);
        Require(game.RevealedSafeCells() == 0, "Minesweeper needs Space/start before input");
        game.Start();
        game.Update(1.0f);
        Require(game.ElapsedMilliseconds() == 0, "Timer must wait for first reveal");
        const int firstX = seed % mines::rules::columns;
        const int firstY = (seed / mines::rules::columns) % mines::rules::rows;
        game.Reveal(firstX, firstY);
        Require(game.At(firstX, firstY).revealed && game.At(firstX, firstY).adjacentMines == 0,
                "First reveal must be safe and open a blank area");
        int mineCount = 0;
        for (int y = 0; y < mines::rules::rows; ++y) {
            for (int x = 0; x < mines::rules::columns; ++x) {
                const auto& cell = game.At(x, y);
                if (cell.mine) ++mineCount;
                int adjacent = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        const int nx = x + dx, ny = y + dy;
                        if ((dx != 0 || dy != 0) && nx >= 0 && ny >= 0 &&
                            nx < mines::rules::columns && ny < mines::rules::rows &&
                            game.At(nx, ny).mine) ++adjacent;
                    }
                }
                Require(cell.adjacentMines == adjacent, "Neighbor counts must include board edges");
                if (std::abs(x - firstX) <= 1 && std::abs(y - firstY) <= 1) {
                    Require(!cell.mine && cell.revealed, "First-click neighborhood must flood open");
                }
            }
        }
        Require(mineCount == mines::rules::mineCount, "Mine count must be exact");
        game.Update(1.0f);
        Require(game.ElapsedMilliseconds() == 1000, "Timer uses milliseconds");
        const int revealed = game.RevealedSafeCells();
        game.TogglePause();
        game.Update(5.0f);
        game.Reveal(0, 0);
        game.ToggleFlag(0, 0);
        Require(game.RevealedSafeCells() == revealed && game.RemainingMines() == 10 &&
                game.ElapsedMilliseconds() == 1000, "Pause must freeze timer and input");
        game.TogglePause();
        game.Reveal(-1, 0);
        game.ToggleFlag(9, 0);
        Require(game.RemainingMines() == 10, "Out-of-bounds input is ignored");
        for (int y = 0; y < mines::rules::rows; ++y) {
            for (int x = 0; x < mines::rules::columns; ++x) {
                if (!game.At(x, y).mine && !game.At(x, y).revealed) {
                    game.ToggleFlag(x, y);
                    game.Reveal(x, y);
                    Require(!game.At(x, y).revealed, "Flagged cells cannot be revealed");
                    game.ToggleFlag(x, y);
                    game.Reveal(x, y);
                }
            }
        }
        Require(game.State() == mines::Status::Won, "All safe cells wins without flagging mines");
        game.Update(10.0f);
        Require(game.ElapsedMilliseconds() == 1000, "Win freezes the timer");
        game.Reset();
        Require(game.State() == mines::Status::Ready && game.ElapsedMilliseconds() == 0 &&
                game.RevealedSafeCells() == 0 && game.RemainingMines() == 10,
                "Reset clears the previous game");
        game.Start();
        game.Reveal(4, 4);
        for (int y = 0; y < mines::rules::rows; ++y) {
            for (int x = 0; x < mines::rules::columns; ++x) {
                if (game.At(x, y).mine) game.Reveal(x, y);
            }
        }
        Require(game.State() == mines::Status::GameOver, "Revealing a mine loses");
    }
    // Clicking a number opens neighbors only when the adjacent flag count matches.
    mines::MinesweeperGame chord(42);
    chord.Start();
    chord.Reveal(4, 4);
    bool exercisedChord = false;
    for (int y = 0; y < mines::rules::rows && !exercisedChord; ++y) {
        for (int x = 0; x < mines::rules::columns && !exercisedChord; ++x) {
            if (!chord.At(x, y).revealed || chord.At(x, y).adjacentMines == 0) continue;
            int coveredSafe = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    const int nx = x + dx, ny = y + dy;
                    if (nx < 0 || ny < 0 || nx >= 9 || ny >= 9) continue;
                    if (!chord.At(nx, ny).mine && !chord.At(nx, ny).revealed) ++coveredSafe;
                }
            }
            if (coveredSafe == 0) continue;
            const int before = chord.RevealedSafeCells();
            chord.RevealNeighbors(x, y);
            Require(chord.RevealedSafeCells() == before, "Unmatched flags must not chord");
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    const int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < 9 && ny < 9 && chord.At(nx, ny).mine) {
                        chord.ToggleFlag(nx, ny);
                    }
                }
            }
            chord.RevealNeighbors(x, y);
            Require(chord.RevealedSafeCells() > before && chord.State() != mines::Status::GameOver,
                    "Correct adjacent flags must safely open neighbors");
            exercisedChord = true;
        }
    }
    Require(exercisedChord, "Chord scenario was exercised");
}

void TestPaddleCollisions() {
    namespace breakout = games::breakout;
    const breakout::Bounds paddle{200.0f, breakout::rules::paddleY,
                                  breakout::rules::paddleWidth, breakout::rules::paddleHeight};
    const float right = paddle.x + paddle.width;
    const float bottom = paddle.y + paddle.height;
    struct Case {
        breakout::Point ball;
        breakout::Point velocity;
        breakout::Point outward;
    };
    const Case cases[]{
        {{paddle.x - 6.0f, paddle.y + 7.0f}, {300.0f, 100.0f}, {-1.0f, 0.0f}},
        {{right + 6.0f, paddle.y + 7.0f}, {-300.0f, 100.0f}, {1.0f, 0.0f}},
        {{paddle.x + paddle.width / 2.0f, paddle.y - 6.0f}, {50.0f, 300.0f}, {0.0f, -1.0f}},
        {{paddle.x + paddle.width / 2.0f, bottom + 6.0f}, {50.0f, -300.0f}, {0.0f, 1.0f}},
        {{paddle.x - 4.0f, paddle.y - 4.0f}, {100.0f, 200.0f}, {-1.0f, -1.0f}},
        {{right + 4.0f, paddle.y - 4.0f}, {-100.0f, 200.0f}, {1.0f, -1.0f}},
        {{paddle.x - 4.0f, bottom + 4.0f}, {100.0f, -200.0f}, {-1.0f, 1.0f}},
        {{right + 4.0f, bottom + 4.0f}, {-100.0f, -200.0f}, {1.0f, 1.0f}}
    };
    for (auto test : cases) {
        Require(breakout::BounceOffPaddle(test.ball, test.velocity, paddle, 0.0f),
                "Paddle faces and all four corners must collide");
        Require(test.velocity.x * test.outward.x + test.velocity.y * test.outward.y > 0.0f,
                "Paddle contact reflects the ball outward");
        Require(std::abs(std::hypot(test.velocity.x, test.velocity.y) -
                         breakout::rules::ballSpeed) < 0.01f, "Contact preserves configured ball speed");
        Require(!breakout::BounceOffPaddle(test.ball, test.velocity, paddle, 0.0f),
                "Resolved contact must not bounce again or remain inside the paddle");
    }
    breakout::Point nearby{paddle.x - 6.0f, paddle.y - 6.0f};
    breakout::Point incoming{100.0f, 200.0f};
    Require(!breakout::BounceOffPaddle(nearby, incoming, paddle, 0.0f),
            "Near-corner misses must not hit an artificially enlarged rectangular region");
    breakout::Point departing{paddle.x - 6.0f, paddle.y + 7.0f};
    breakout::Point away{-200.0f, 100.0f};
    Require(breakout::BounceOffPaddle(departing, away, paddle, 0.0f) &&
            away.x == -200.0f && away.y == 100.0f,
            "An overlapping but departing ball is separated without reversing again");
    breakout::Point inside{paddle.x + 3.0f, paddle.y + 6.0f};
    breakout::Point insideVelocity{200.0f, 100.0f};
    Require(breakout::BounceOffPaddle(inside, insideVelocity, paddle, 0.0f) &&
            inside.x < paddle.x - breakout::rules::ballRadius && insideVelocity.x < 0.0f,
            "A ball center inside the paddle is ejected through the nearest face");
    for (const float direction : {-1.0f, 1.0f}) {
        breakout::Point ball{direction < 0 ? paddle.x - 6.0f : right + 6.0f, paddle.y + 7.0f};
        breakout::Point velocity{direction * 100.0f, 0.0f};
        Require(breakout::BounceOffPaddle(ball, velocity, paddle,
                                        direction * breakout::rules::paddleSpeed) &&
                velocity.x * direction > 0.0f,
                "A moving paddle can catch a slower departing ball at either side");
    }
    breakout::Point top{paddle.x + paddle.width * 0.75f, paddle.y - 6.0f};
    breakout::Point topVelocity{0.0f, breakout::rules::ballSpeed};
    breakout::BounceOffPaddle(top, topVelocity, paddle, 0.0f);
    const float expectedAngle = 0.5f * breakout::rules::maximumBounceAngle;
    Require(std::abs(topVelocity.x - breakout::rules::ballSpeed * std::sin(expectedAngle)) < 0.01f &&
            std::abs(topVelocity.y + breakout::rules::ballSpeed * std::cos(expectedAngle)) < 0.01f,
            "Top-face hits keep the existing paddle aiming behavior");
}

void TestBreakout() {
    namespace breakout = games::breakout;
    breakout::BreakoutGame game;
    const auto initial = game.Ball();
    game.Update(0.1f, 1.0f);
    Require(game.State() == breakout::Status::Ready && game.Ball().x > initial.x &&
            game.Ball().y == initial.y, "Ready ball follows paddle without launching");
    game.Start();
    game.TogglePause();
    const auto pausedBall = game.Ball();
    game.Update(1.0f, -1.0f);
    Require(game.Ball().x == pausedBall.x && game.Ball().y == pausedBall.y,
            "Pause freezes ball and paddle");
    game.TogglePause();
    for (int frame = 0; frame < 180; ++frame) game.Update(1.0f / 60.0f, -1.0f);
    Require(game.Score() > 0 && game.Score() % breakout::rules::pointsPerBrick == 0,
            "Ball destroys bricks and earns points");
    Require(game.Paddle().x >= 0 &&
            game.Paddle().x + game.Paddle().width <= breakout::rules::boardWidth,
            "Paddle stays within walls");
    game.Reset();
    Require(game.Score() == 0 && game.Lives() == breakout::rules::initialLives &&
            std::all_of(game.Bricks().begin(), game.Bricks().end(), [](bool b) { return b; }),
            "Reset restores bricks, score, and lives");

    // Deliberately miss the ball to exercise all lives and the ready/serve transition.
    for (int frame = 0; frame < 20000 && game.State() != breakout::Status::GameOver; ++frame) {
        if (game.State() == breakout::Status::Ready) game.Start();
        const float away = game.Ball().x > breakout::rules::boardWidth / 2.0f ? -1.0f : 1.0f;
        game.Update(1.0f / 120.0f, away);
    }
    Require(game.State() == breakout::Status::GameOver && game.Lives() == 0,
            "Missing three balls ends the game");

    // Follow the ball with varying hit offsets to clear the complete board.
    game.Reset();
    for (int frame = 0; frame < 180000 && game.State() != breakout::Status::Won; ++frame) {
        if (game.State() == breakout::Status::Ready) game.Start();
        Require(game.State() != breakout::Status::GameOver, "Autoplay exhausted lives");
        const float aimOffset = 24.0f * std::sin(frame / 300.0f);
        const float desired = game.Ball().x - breakout::rules::paddleWidth / 2.0f + aimOffset;
        const float input = std::clamp((desired - game.Paddle().x) /
                                       (breakout::rules::paddleSpeed / 120.0f), -1.0f, 1.0f);
        game.Update(1.0f / 120.0f, input);
        Require(std::isfinite(game.Ball().x) && std::isfinite(game.Ball().y),
                "Physics remains finite");
    }
    Require(game.State() == breakout::Status::Won &&
            game.Score() == breakout::rules::brickCount * breakout::rules::pointsPerBrick,
            "Clearing all bricks wins with the correct score");

    breakout::BreakoutGame lowFps, highFps;
    lowFps.Start();
    highFps.Start();
    for (int frame = 0; frame < 30; ++frame) lowFps.Update(1.0f / 30.0f, 0.0f);
    for (int frame = 0; frame < 240; ++frame) highFps.Update(1.0f / 240.0f, 0.0f);
    Require(std::abs(lowFps.Ball().x - highFps.Ball().x) < 0.2f &&
            std::abs(lowFps.Ball().y - highFps.Ball().y) < 0.2f &&
            lowFps.Score() == highFps.Score(), "Physics stays consistent across FPS caps");
}

void TestRecords(const std::filesystem::path& directory) {
    const auto path = directory / "records-test.ini";
    {
        std::ofstream old(path);
        old << "snake=120\ntetris=456\n";
    }
    app::HighScores records;
    Require(app::LoadHighScores(records, path) && records.snake == 120 &&
            records.tetris == 456 && records.breakout == 0 &&
            records.minesweeperEasyMilliseconds == 0, "Old records remain compatible");
    records.breakout = 300;
    records.minesweeperEasyMilliseconds = 12345;
    Require(app::SaveHighScores(records, path), "Records save succeeds");
    app::HighScores loaded;
    Require(app::LoadHighScores(loaded, path) && loaded.snake == 120 && loaded.tetris == 456 &&
            loaded.breakout == 300 && loaded.minesweeperEasyMilliseconds == 12345,
            "All four records persist without overwriting older scores");
    loaded.minesweeperEasyMilliseconds = -1;
    Require(!app::SaveHighScores(loaded, path), "Negative record values are rejected");
    std::filesystem::remove(path);
}

void TestSettings(const std::filesystem::path& directory) {
    const auto path = directory / "settings-test.ini";
    app::Settings settings;
    settings.language = app::Language::English;
    settings.musicVolume = 0.7f;
    settings.musicPath = "C:/Music/星空.ogg";
    Require(app::SaveSettings(settings, path), "Settings save succeeds");
    app::Settings loaded;
    Require(app::LoadSettings(loaded, path) && loaded.language == app::Language::English &&
            loaded.musicPath == settings.musicPath && std::abs(loaded.musicVolume - 0.7f) < 0.001f,
            "Selected music path and volume persist across restart");
    std::filesystem::remove(path);
}

void TestMinesweeperDifficulties(const std::filesystem::path& directory) {
    namespace mines = games::minesweeper;
    const std::array<mines::BoardSettings, 5> boards{{
        mines::difficulty::easy, mines::difficulty::normal, mines::difficulty::hard,
        {5, 5, 16}, {40, 30, 1191}
    }};
    for (std::size_t index = 0; index < boards.size(); ++index) {
        for (unsigned seed = 0; seed < 12; ++seed) {
            mines::MinesweeperGame game(seed);
            const auto level = index < 3 ? static_cast<mines::Difficulty>(index) : mines::Difficulty::Custom;
            Require(game.Configure(level, boards[index]), "Difficulty configuration succeeds");
            Require(game.Columns() == boards[index].columns && game.Rows() == boards[index].rows &&
                    game.MineCount() == boards[index].mines, "Difficulty chooses the correct board");
            game.Start();
            const int firstX = seed % 2 ? 0 : game.Columns() / 2;
            const int firstY = seed % 2 ? 0 : game.Rows() / 2;
            game.Reveal(firstX, firstY);
            Require(std::count_if(game.Cells().begin(), game.Cells().end(),
                [](const mines::Cell& cell) { return cell.mine; }) == game.MineCount(),
                "Every board has its exact requested mine count");
            Require(game.At(firstX, firstY).adjacentMines == 0,
                "Dense custom boards still preserve first reveal safety");
            for (int y = 0; y < game.Rows(); ++y) {
                for (int x = 0; x < game.Columns(); ++x) {
                    if (!game.At(x, y).mine) game.Reveal(x, y);
                }
            }
            Require(game.State() == mines::Status::Won &&
                    game.RevealedSafeCells() == game.Columns() * game.Rows() - game.MineCount(),
                    "Dynamic boards win after all safe cells are revealed");
            game.Reset();
            Require(game.Columns() == boards[index].columns && game.MineCount() == boards[index].mines &&
                    game.State() == mines::Status::Ready, "Restart preserves selected difficulty");
        }
    }
    mines::MinesweeperGame game(42);
    Require(!game.Configure(mines::Difficulty::Custom, {4, 9, 10}) &&
            !game.Configure(mines::Difficulty::Custom, {40, 31, 10}) &&
            !game.Configure(mines::Difficulty::Custom, {5, 5, 17}) &&
            !game.Configure(mines::Difficulty::Custom, {5, 5, 0}) && game.Columns() == 9,
            "Invalid custom parameters are rejected without changing the game");
    app::HighScores records;
    Require(app::RecordMinesweeperWin(records, mines::Difficulty::Easy, 12000) &&
            app::RecordMinesweeperWin(records, mines::Difficulty::Normal, 24000) &&
            app::RecordMinesweeperWin(records, mines::Difficulty::Hard, 36000), "Three independent records update");
    Require(!app::RecordMinesweeperWin(records, mines::Difficulty::Custom, 1) &&
            !app::RecordMinesweeperWin(records, mines::Difficulty::Easy, 15000) &&
            app::MinesweeperBest(records, mines::Difficulty::Custom) == 0, "Custom wins and slower wins are not recorded");
    const auto path = directory / "difficulty-records.ini";
    Require(app::SaveHighScores(records, path), "Difficulty records save");
    app::HighScores loaded;
    Require(app::LoadHighScores(loaded, path) && loaded.minesweeperEasyMilliseconds == 12000 &&
            loaded.minesweeperNormalMilliseconds == 24000 && loaded.minesweeperHardMilliseconds == 36000,
            "All difficulty records persist across reload");
    {
        std::ofstream legacy(path);
        legacy << "snake=42\nminesweeper_best_ms=12345\n";
    }
    Require(app::LoadHighScores(loaded, path) && loaded.minesweeperEasyMilliseconds == 12345 &&
            loaded.minesweeperNormalMilliseconds == 0 && loaded.minesweeperHardMilliseconds == 0 &&
            loaded.snake == 42, "Legacy record migrates to Easy without touching other records");
    {
        std::ofstream mixed(path);
        mixed << "minesweeper_easy_ms=20000\nminesweeper_best_ms=12345\n";
    }
    Require(app::LoadHighScores(loaded, path) && loaded.minesweeperEasyMilliseconds == 20000,
            "Explicit difficulty record overrides legacy data regardless of key order");
    std::filesystem::remove(path);
}

void TestExistingGames() {
    games::snake::SnakeGame snake(42);
    snake.Start();
    Require(snake.State() == games::snake::Status::Playing, "Snake can start");
    games::tetris::TetrisGame tetris(42);
    tetris.Start();
    Require(tetris.State() == games::tetris::Status::Playing && tetris.Score() == 0,
            "Tetris start preserves the initial piece and score");
}

}  // namespace

int main(int argc, char** argv) {
    try {
        Require(argc == 2, "Pass a temporary test-data directory");
        const std::filesystem::path directory = argv[1];
        std::filesystem::create_directories(directory);
        TestMinesweeper();
        TestPaddleCollisions();
        TestBreakout();
        TestRecords(directory);
        TestSettings(directory);
        TestMinesweeperDifficulties(directory);
        TestExistingGames();
        std::cout << "Game rules and record persistence tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
