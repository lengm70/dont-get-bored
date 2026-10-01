#include "app/Application.h"

#include "raylib.h"
#include "app/Screen.h"
#include "config/AppSettings.h"
#include "config/HighScores.h"
#include "config/UiConfig.h"
#include "games/snake/SnakeGame.h"
#include "games/snake/SnakeScreen.h"
#include "games/tetris/TetrisGame.h"
#include "games/tetris/TetrisScreen.h"
#include "ui/FontLoader.h"
#include "ui/MenuView.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"

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
    InitWindow(initialSize.width, initialSize.height, ui::config::windowTitle);
    SetTargetFPS(settings.fpsLimit);
    SetExitKey(KEY_NULL);

    Font font = ui::LoadMenuFont();
    if (!IsFontValid(font)) {
        CloseWindow();
        return 1;
    }
    ui::ConfigureGuiStyle(font, ui::CurrentLayout().scale);

    Screen screen = Screen::MainMenu;
    games::snake::SnakeGame snake;
    games::tetris::TetrisGame tetris;
    bool running = true;

    while (running && !WindowShouldClose()) {
        const Settings previous = settings;
        if (IsKeyPressed(KEY_F9)) settings.resolution = Resolution::P720;
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (screen == Screen::MainMenu) running = false;
            else if (screen == Screen::Snake || screen == Screen::Tetris) {
                screen = Screen::GameSelection;
            }
            else screen = Screen::MainMenu;
        }

        if (screen == Screen::Snake) {
            games::snake::UpdateFromInput(snake, GetFrameTime());
            if (snake.Score() > highScores.snake) {
                highScores.snake = snake.Score();
                if (!SaveHighScores(highScores)) {
                    TraceLog(LOG_WARNING, "Could not save high scores: %s", highScoresPath);
                }
            }
        } else if (screen == Screen::Tetris) {
            games::tetris::UpdateFromInput(tetris, GetFrameTime());
            if (tetris.Score() > highScores.tetris) {
                highScores.tetris = tetris.Score();
                if (!SaveHighScores(highScores)) {
                    TraceLog(LOG_WARNING, "Could not save high scores: %s", highScoresPath);
                }
            }
        }

        BeginDrawing();
        ui::MenuResult result;
        if (screen == Screen::Snake) {
            games::snake::DrawScreen(snake, font, settings.language, highScores.snake);
        } else if (screen == Screen::Tetris) {
            games::tetris::DrawScreen(tetris, font, settings.language,
                                      highScores.tetris);
        } else {
            result = ui::DrawMenu(screen, font, settings);
        }
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
            if (!SaveSettings(settings)) {
                TraceLog(LOG_WARNING, "Could not save settings file: %s", settingsPath);
            }
        }

        switch (result.action) {
            case ui::MenuAction::OpenGameSelection: screen = Screen::GameSelection; break;
            case ui::MenuAction::OpenSettings: screen = Screen::Settings; break;
            case ui::MenuAction::StartSnake:
                snake.Reset();
                screen = Screen::Snake;
                break;
            case ui::MenuAction::StartTetris:
                tetris.Reset();
                screen = Screen::Tetris;
                break;
            case ui::MenuAction::Back: screen = Screen::MainMenu; break;
            case ui::MenuAction::Quit: running = false; break;
            case ui::MenuAction::None: break;
        }
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}

}  // namespace app
