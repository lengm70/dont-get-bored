#include "app/Application.h"

#include <algorithm>
#include <filesystem>


#include "raylib.h"
#include "raygui.h"
#include "audio/BackgroundMusic.h"
#include "app/Screen.h"
#include "app/MediaController.h"
#include "ui/MediaLibraryView.h"
#include "ui/PlayerBackground.h"
#include "config/AppSettings.h"
#include "config/HighScores.h"
#include "config/UiConfig.h"
#include "games/fruit/FruitScreen.h"
#include "games/snake/SnakeGame.h"
#include "games/snake/SnakeScreen.h"
#include "games/tetris/TetrisGame.h"
#include "games/tetris/TetrisScreen.h"
#include "games/breakout/BreakoutGame.h"
#include "games/breakout/BreakoutScreen.h"
#include "games/minesweeper/MinesweeperGame.h"
#include "games/minesweeper/MinesweeperScreen.h"
#include "games/minesweeper/MinesweeperSetup.h"
#include "games/gomoku/GomokuScreen.h"
#include "ui/FontLoader.h"
#include "ui/BackgroundImage.h"
#include "ui/MenuView.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/WindowIcon.h"
#include "platform/MusicFileDialog.h"

namespace app {

int Run() {
    Settings settings;
    if (!LoadSettings(settings) && !SaveSettings(settings)) {
        TraceLog(LOG_WARNING, "Could not create settings file: %s", settingsPath);
    }
    HighScores highScores;
    if (!LoadHighScores(highScores) && !SaveHighScores(highScores)) {
        TraceLog(LOG_WARNING, "Could not create high score file: %s", highScoresPath);
    }

    const WindowSize initialSize = GetWindowSize(settings.resolution);
    if (!ui::ConfigureApplicationIdentity()) {
        TraceLog(LOG_WARNING, "Could not set application taskbar identity");
    }
    InitWindow(initialSize.width, initialSize.height, ui::config::windowTitle);
    ui::ConfigureWindowIcon();
    SetTargetFPS(settings.fpsLimit);
    SetExitKey(KEY_NULL);

    audio::BackgroundMusic backgroundMusic;
    MediaController mediaController(settings, backgroundMusic);
    mediaController.Initialize();
    ui::MediaLibraryViewState mediaView;
    Font font = ui::LoadMenuFont(mediaController.Names());
    if (!IsFontValid(font)) {
        ui::UnloadPlayerBackground(); ui::UnloadBackgroundImage(); backgroundMusic.Shutdown();
        CloseWindow();
        return 1;
    }
    ui::ConfigureGuiStyle(font, ui::CurrentLayout().scale);

    unsigned themeRevision = ui::palette::revision;
    Screen screen = Screen::MainMenu;
    games::fruit::FruitGame fruit;
    games::snake::SnakeGame snake;
    games::tetris::TetrisGame tetris;
    games::breakout::BreakoutGame breakout;
    games::minesweeper::MinesweeperGame minesweeper;
    games::minesweeper::SetupOptions minesweeperOptions;
    games::gomoku::GomokuGame gomoku;
    games::gomoku::GomokuAiTurn gomokuAi;
    games::gomoku::SetupOptions gomokuOptions;
    bool running = true;

    while (running && !WindowShouldClose()) {
        if (themeRevision != ui::palette::revision) {
            ui::ConfigureGuiStyle(font, ui::CurrentLayout().scale);
            themeRevision = ui::palette::revision;
        }
        backgroundMusic.Update();
        const Settings previous = settings;
        if (IsKeyPressed(KEY_F9)) settings.resolution = Resolution::P720;
        if (IsKeyPressed(KEY_ESCAPE)) {
            gomokuAi.Cancel();
            if (screen == Screen::MainMenu) running = false;
            else if (screen == Screen::Fruit || screen == Screen::Snake || screen == Screen::Tetris ||
                     screen == Screen::Breakout || screen == Screen::Minesweeper ||
                     screen == Screen::MinesweeperSetup || screen == Screen::Gomoku ||
                     screen == Screen::GomokuSetup) {
                screen = Screen::GameSelection;
            }
            else screen = Screen::MainMenu;
        }

        const HighScores previousScores = highScores;
        const bool mouseCaptured = ui::UpdateMusicPlayerInput();
        if (screen == Screen::Fruit) {
            games::fruit::UpdateFromInput(fruit, GetFrameTime(), mouseCaptured);
            highScores.fruit = std::max(highScores.fruit, fruit.Score());
        } else if (screen == Screen::Snake) {
            games::snake::UpdateFromInput(snake, GetFrameTime());
            highScores.snake = std::max(highScores.snake, snake.Score());
        } else if (screen == Screen::Tetris) {
            games::tetris::UpdateFromInput(tetris, GetFrameTime());
            highScores.tetris = std::max(highScores.tetris, tetris.Score());
        } else if (screen == Screen::Breakout) {
            games::breakout::UpdateFromInput(breakout, GetFrameTime());
            highScores.breakout = std::max(highScores.breakout, breakout.Score());
        } else if (screen == Screen::Minesweeper) {
            games::minesweeper::UpdateFromInput(minesweeper, GetFrameTime(), mouseCaptured);
            if (minesweeper.State() == games::minesweeper::Status::Won) {
                RecordMinesweeperWin(highScores, minesweeper.Level(),
                                     std::max(1, minesweeper.ElapsedMilliseconds()));
            }
        }
        if (screen == Screen::Gomoku) {
            games::gomoku::UpdateFromInput(gomoku, gomokuAi,
                static_cast<games::gomoku::Strength>(gomokuOptions.strength), mouseCaptured);
        }
        const bool recordsChanged = highScores.fruit != previousScores.fruit || highScores.snake != previousScores.snake ||
            highScores.tetris != previousScores.tetris ||
            highScores.breakout != previousScores.breakout ||
            highScores.minesweeperEasyMilliseconds != previousScores.minesweeperEasyMilliseconds ||
            highScores.minesweeperNormalMilliseconds != previousScores.minesweeperNormalMilliseconds ||
            highScores.minesweeperHardMilliseconds != previousScores.minesweeperHardMilliseconds;
        if (recordsChanged && !SaveHighScores(highScores)) {
            TraceLog(LOG_WARNING, "Could not save high scores: %s", highScoresPath);
        }

        BeginDrawing();
        ui::MenuResult result;
        const bool guiWasLocked = GuiIsLocked();
        if (mouseCaptured) GuiLock();
        if (screen == Screen::Fruit) {
            games::fruit::DrawScreen(fruit, font, settings.language, highScores.fruit);
        } else if (screen == Screen::Snake) {
            games::snake::DrawScreen(snake, font, settings.language, highScores.snake);
        } else if (screen == Screen::Tetris) {
            games::tetris::DrawScreen(tetris, font, settings.language,
                                      highScores.tetris);
        } else if (screen == Screen::Breakout) {
            games::breakout::DrawScreen(breakout, font, settings.language, highScores.breakout);
        } else if (screen == Screen::Minesweeper) {
            games::minesweeper::DrawScreen(minesweeper, font, settings.language,
                                          MinesweeperBest(highScores, minesweeper.Level()));
        } else if (screen == Screen::MinesweeperSetup) {
            result = games::minesweeper::DrawSetup(font, settings.language, minesweeperOptions, highScores);
        } else if (screen == Screen::Gomoku) {
            games::gomoku::DrawScreen(gomoku, font, settings.language, gomokuAi.Thinking());
        } else if (screen == Screen::GomokuSetup) {
            result = games::gomoku::DrawSetup(font, settings.language, gomokuOptions);
        } else if (screen == Screen::MediaLibrary) {
            result = ui::DrawMediaLibrary(font, settings.language, mediaController.Library(), settings, mediaView);
        } else {
            result = ui::DrawMenu(screen, font, settings);
        }
        if (!guiWasLocked) GuiUnlock();
        const ui::MenuResult player = ui::DrawMusicPlayer(font, settings);
        if (player.action != ui::MenuAction::None) result.action = player.action;
        result.settingsChanged |= player.settingsChanged;
        if (settings.showFps) {
            const ui::UiLayout layout = ui::CurrentLayout();
            const Vector2 fpsPosition = layout.Point(ui::config::fpsCounterX,
                                                      ui::config::fpsCounterY);
            DrawFPS(static_cast<int>(fpsPosition.x), static_cast<int>(fpsPosition.y));
        }
        EndDrawing();

        if (result.settingsChanged || settings.resolution != previous.resolution) {
            if (settings.resolution != previous.resolution) {
                const WindowSize size = GetWindowSize(settings.resolution);
                SetWindowSize(size.width, size.height);
                ui::ConfigureGuiStyle(font, ui::CurrentLayout().scale);
            }
            if (settings.fpsLimit != previous.fpsLimit) SetTargetFPS(settings.fpsLimit);
            if (settings.musicVolume != previous.musicVolume) {
                backgroundMusic.SetVolume(settings.musicVolume);
            }
            if (!SaveSettings(settings)) {
                TraceLog(LOG_WARNING, "Could not save settings file: %s", settingsPath);
            }
        }

        switch (result.action) {
            case ui::MenuAction::OpenGameSelection: screen = Screen::GameSelection; break;
            case ui::MenuAction::OpenSettings: screen = Screen::Settings; break;
            case ui::MenuAction::OpenMusicSettings: screen = Screen::MusicSettings; break;
            case ui::MenuAction::OpenMediaLibrary: {
                mediaView.category = result.mediaCategory >= 0 ? result.mediaCategory : 0;
                mediaView.selected = mediaView.scroll = 0;
                mediaView.failed = false;
                const auto entries = mediaController.Library().List(static_cast<media::Kind>(mediaView.category));
                const auto& current = mediaView.category == 0 ? settings.backgroundPath :
                    mediaView.category == 1 ? settings.playerBackgroundPath : settings.musicPath;
                for (int index = 0; index < static_cast<int>(entries.size()); ++index)
                    if (entries[index].path == current) mediaView.selected = index + 1;
                screen = Screen::MediaLibrary;
                break;
            }
            case ui::MenuAction::ImportMusic:
            case ui::MenuAction::ImportBackground:
                mediaView.category = result.action == ui::MenuAction::ImportMusic ? 2 : 0;
                screen = Screen::MediaLibrary;
                break;
            case ui::MenuAction::ImportMedia: {
                const auto kind = static_cast<media::Kind>(mediaView.category);
                const auto source = kind == media::Kind::Music ? platform::ChooseMusicFile() : platform::ChooseBackgroundFile();
                if (source.empty()) break;
                mediaView.failed = !mediaController.Import(kind, source);
                if (!mediaView.failed) {
                    const auto entries = mediaController.Library().List(kind);
                    const auto& selectedPath = kind == media::Kind::Background ? settings.backgroundPath :
                        kind == media::Kind::PlayerBackground ? settings.playerBackgroundPath : settings.musicPath;
                    for (int index = 0; index < static_cast<int>(entries.size()); ++index)
                        if (entries[index].path == selectedPath) mediaView.selected = index + 1;
                    Font updated = ui::LoadMenuFont(mediaController.Names());
                    if (IsFontValid(updated)) {
                        const Font old = font; font = updated;
                        ui::ConfigureGuiStyle(font, ui::CurrentLayout().scale);
                        UnloadFont(old);
                    }
                }
                break;
            }
            case ui::MenuAction::UseMedia:
                mediaView.failed = !mediaController.Select(static_cast<media::Kind>(mediaView.category), mediaView.selected);
                break;
            case ui::MenuAction::StartFruit:
                fruit.Reset(); screen = Screen::Fruit; break;
            case ui::MenuAction::StartSnake:
                snake.Reset();
                screen = Screen::Snake;
                break;
            case ui::MenuAction::StartTetris:
                tetris.Reset();
                screen = Screen::Tetris;
                break;
            case ui::MenuAction::PreviousMusic:
                mediaController.CycleMusic(-1);
                break;
            case ui::MenuAction::ToggleMusic:
                backgroundMusic.TogglePause();
                break;
            case ui::MenuAction::NextMusic:
                mediaController.CycleMusic(1);
                break;
            case ui::MenuAction::Back:
                if (screen == Screen::MinesweeperSetup || screen == Screen::GomokuSetup) screen = Screen::GameSelection;
                else if (screen == Screen::MusicSettings || screen == Screen::MediaLibrary) screen = Screen::Settings;
                else screen = Screen::MainMenu;
                break;
            case ui::MenuAction::StartBreakout:
                breakout.Reset();
                screen = Screen::Breakout;
                break;
            case ui::MenuAction::StartMinesweeper:
                screen = Screen::MinesweeperSetup;
                break;
            case ui::MenuAction::LaunchMinesweeper:
                if (minesweeper.Configure(static_cast<games::minesweeper::Difficulty>(minesweeperOptions.selected),
                                          minesweeperOptions.custom)) {
                    minesweeper.Start();
                    screen = Screen::Minesweeper;
                }
                break;
            case ui::MenuAction::StartGomoku: screen = Screen::GomokuSetup; break;
            case ui::MenuAction::LaunchGomoku:
                gomokuAi.Cancel();
                gomoku.Configure(static_cast<games::gomoku::Mode>(gomokuOptions.mode));
                gomoku.Start();
                screen = Screen::Gomoku;
                break;
            case ui::MenuAction::Quit: running = false; break;
            case ui::MenuAction::None: break;
        }
    }

    gomokuAi.Cancel();
    ui::UnloadPlayerBackground();
    ui::UnloadBackgroundImage();
    UnloadFont(font);
    backgroundMusic.Shutdown();
    CloseWindow();
    return 0;
}

}  // namespace app
