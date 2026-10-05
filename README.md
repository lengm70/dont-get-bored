# don't get bored

A C++17 GUI collection with Snake, Tetris, Breakout, and Minesweeper, with Chinese and English menus. Every game starts with **Space** from its ready screen.

## Download for Windows

Download `dont-get-bored-windows-x64.zip` from the [latest release](https://github.com/lengm70/dont-get-bored/releases/latest), extract the entire archive, and run `dont_get_bored.exe` in the extracted folder. Keep the DLL files and `assets` folder beside the program. The separate EXE asset requires those dependencies. Source code archives are available on the release page.

## Snake

Open **Select Game / 选择游戏** and choose **Snake / 贪吃蛇**. Press Space to start. Use the arrow keys or WASD to move, Space to pause or resume, R to restart, and Esc to return to game selection. Eat the food to gain points. The game ends if the snake hits a wall or itself; filling the board wins.

The snake moves at a fixed game speed, independent of the display FPS cap. Starting a new game resets its score and board.

## Tetris

Open **Select Game / 选择游戏** and choose **Tetris / 俄罗斯方块**. Press Space to start. The starting press does not hard-drop the first piece. Move with Left/Right (or A/D), rotate clockwise with Up, X, or W, rotate counterclockwise with Z, soft drop with Down or S, and hard drop with Space during play. Press P to pause or resume, R to restart, and Esc to return to game selection. Clear lines to score points; each ten lines raises the level and speeds up gravity.

## Breakout

Choose **Breakout / 打砖块**. Move the paddle with Left/Right or A/D, and press Space to serve. Hit all 50 bricks to win; missing the ball costs one of three lives. After losing a life, press Space to serve again. Paddle impact position determines the bounce angle. Press P to pause or resume, R to reset the board, and Esc to return. Physics uses small time steps independently of the display FPS cap.

## Minesweeper

Choose **Minesweeper / 扫雷**, then press Space to start the 9×9 board with 10 mines. Left-click to reveal and right-click to place or remove a flag. Mines are placed on the first reveal, keeping that cell and its neighbors safe. Empty areas open automatically. Clicking a revealed number opens its remaining neighbors when the adjacent flag count matches the number; incorrect flags can cause a loss. Reveal every safe cell to win. Press P to pause or resume, R to reset, and Esc to return. The timer begins on the first reveal and stops while paused or after the game ends.

## High scores

Snake, Tetris, and Breakout each keep their own best score. Minesweeper keeps the shortest winning time in milliseconds (`minesweeper_best_ms`, where zero means no record yet). Records are stored in `config/highscores.ini` and remain after restarting. Existing Snake/Tetris records load without migration; missing new keys default to zero. This personal file is ignored by Git; `config/highscores.example.ini` shows the defaults.

## Settings

The settings page supports Chinese and English, 720p (1280×720), 1080p (1920×1080), 2K (2560×1440), and 4K (3840×2160), plus FPS caps of 60, 120, 165, and 240. The default is Chinese, 1080p, and 60 FPS. Changes apply immediately and are saved in `config/settings.ini` for the next launch. The FPS counter toggle is saved there too. This personal file is ignored by Git; `config/settings.example.ini` shows the defaults.

The selected resolution sets the window size. Press F9 to reset the window to 720p if a larger resolution does not fit your monitor.

## Requirements

Use the MSYS2 UCRT64 environment. Install the build tools and raylib:

```sh
pacman -S mingw-w64-ucrt-x86_64-raylib mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

raygui 5.0 is included in `external/raygui.h` under its zlib license (`external/raygui-LICENSE`). Its implementation lives only in `src/ui/raygui_impl.cpp`.

The menu font is a subset of [Noto Sans SC](https://github.com/google/fonts/tree/main/ofl/notosanssc), licensed under the SIL Open Font License in `assets/fonts/OFL.txt`. The included font contains the current Chinese labels and ASCII characters; add new glyphs to the font asset when adding new text.

## Project structure

- `src/main.cpp`: entry point.
- `src/app/`: window lifetime and screen navigation.
- `src/config/`: persistent settings and UI constants.
- `src/games/snake/SnakeGame.*`: snake rules and state, independent of raylib.
- `src/games/snake/SnakeScreen.*`: snake input and rendering.
- `src/games/tetris/TetrisGame.*`: Tetris rules and state, independent of raylib.
- `src/games/tetris/TetrisScreen.*`: Tetris input and rendering.
- `src/games/breakout/BreakoutGame.*`: ball, paddle, bricks, collisions, and lives, independent of raylib.
- `src/games/breakout/BreakoutScreen.*`: Breakout input and rendering.
- `src/games/minesweeper/MinesweeperGame.*`: board generation, reveals, flags, timer, and win/loss rules, independent of raylib.
- `src/games/minesweeper/MinesweeperScreen.*`: Minesweeper mouse input and rendering.
- Each game's `Rules.h` contains gameplay constants; `Config.h` contains presentation constants.
- `src/ui/`: menus, scaling, style, labels, and font loading.
- `config/`: local settings, high scores, and default examples.
- `assets/`: menu font and license.
- `external/`: vendored raygui header and license.

To add a menu label, define it in `src/ui/UiText.h`; `FontLoader` collects codepoints from both language sets. To add a game, create a module under `src/games/`, add an entry in `MenuView.cpp`, and wire its screen in `Application.cpp`.

## Build and run

From the project directory in an MSYS2 UCRT64 terminal:

```sh
cmake -S . -B build -G Ninja
cmake --build build
./build/dont_get_bored.exe
```

In VS Code, press Ctrl+Shift+B to build or F5 to debug with GDB.

## Tests

```sh
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests cover first-reveal safety, flood reveals, flags, pause/timing, win/loss states, Breakout collisions and life transitions, record persistence and old-file compatibility, Space start behavior, and Minesweeper mouse mapping at all supported resolutions. Input adapters are tested with substituted input queries so a graphical window is not required.
