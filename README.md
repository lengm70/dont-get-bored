# don't get bored

A C++17 GUI collection with Snake, Tetris, Breakout, Minesweeper, Gomoku, and Fruit Merge, with Chinese and English menus. Every game starts with **Space** from its ready screen.

## Gomoku

Choose **Gomoku / 五子棋**, select Two players or Vs computer, then press Space or click Start game. Black plays first on a 15x15 board. Left-click near an intersection to place a stone. Five or more consecutive stones horizontally, vertically or diagonally wins; there are no forbidden moves. A full board without a winner is a draw. R resets the board to Ready; press Space to start again. Esc returns to game selection.

In computer mode you play Black. Easy, Normal and Hard use pattern evaluation and iterative Alpha-Beta search with maximum depths of 1, 3 and 5, and time budgets of 120, 500 and 1200 ms. Actual depth depends on the position and budget; these are search settings, not guaranteed skill ratings. The computer takes immediate wins and blocks immediate losses before considering other moves. Search runs on a board snapshot in a background worker, with cancellation on restart or leaving the game.

The Gomoku module separates rules/state (`GomokuGame`), search (`GomokuAI`), worker lifetime (`GomokuAiTurn`), presentation constants (`GomokuConfig`) and input/rendering (`GomokuScreen`). Tests cover win directions, overlines, draws, tactical replies, cancellation and mouse mapping.

## Public release: v0.2.4

- Six games, including Fruit Merge with eleven levels and persistent best score.
- Rotating geometric energy orbs, mass-dependent collisions, elastic rebounds and contact friction.
- Music player snaps to all four window edges after dragging; click the arrow tab to expand.
- Two-row game selection, bilingual labels and seven passing CTest targets.

Download the full Windows x64 ZIP from [GitHub Releases](https://github.com/lengm70/dont-get-bored/releases/tag/v0.2.4), extract it and run `dont_get_bored.exe`. Keep the DLLs and assets beside the EXE.

## Development history: v0.3.0-beta.3

- UI colors adapt automatically to the main background's dominant hue.
- Menus, controls, cards and game surfaces share the generated palette.
- Dark surfaces and bright text keep the interface readable across background colors.
- Grayscale and transparent images use neutral colors; failed imports preserve the current theme.
- The theme is restored from the saved background on startup.
- Distributed only from the private development repository.

## Previous internal beta: v0.3.0-beta.2

- Gomoku with two-player and computer modes.
- Custom player backdrops and persistent media libraries for backgrounds and music.
- Reuse previous imports or restore defaults; imported files are stored locally.
- Breakout now scales and clips the playfield and ball to the game panel.
- Distributed only from the private development repository.

## Previous internal beta: v0.3.0-beta.1

- Minesweeper difficulty selection: Easy (9x9, 10 mines), Normal (16x16, 40 mines), Hard (30x16, 99 mines).
- Custom boards: 5-40 columns, 5-30 rows, 1 to (columns * rows - 9) mines.
- First reveal and its neighbors remain safe; boards scale to fit the window.
- Each preset keeps a separate best winning time. Custom games never create records.
- Legacy Minesweeper records migrate to Easy. Existing settings and other games' scores remain compatible.
- This beta is distributed only from the private development repository.

## What's new in v0.2.1

- Cosmic pixel-art background with a matching violet UI palette.
- Illustrated game selection cards with hover feedback and localized descriptions.
- Snake head eyes follow its direction; body colors fade clearly from head to tail.
- Background artwork loads once and preserves its aspect ratio at different window sizes.

## Fruit Merge

Choose **Fruit Merge / 合成大西瓜**, then press Space to start. Move the mouse inside the jar or use Left/Right (A/D) to aim; click inside the jar or press Space to drop. Identical touching fruits merge into the next of eleven levels and award points. Creating the final watermelon wins. If an older fruit remains above the dashed danger line for 1.5 seconds, the game ends; new drops have a 1.2-second grace period. P pauses/resumes, R resets to Ready, and Esc returns to selection. Best score persists in `config/highscores.ini`.

Objects use rotating neon polygon shells and glowing energy cores inside circular collision envelopes, matching the cosmic UI. Impact pulses show collisions and merges. Each level doubles mass; friction transfers motion into spin, and restitution produces visible rebounds. Merges carry linear and angular momentum (spin has a safety cap).

Rules and circle physics are independent of raylib. Physics uses a 120 Hz step, mass-weighted circle separation/impulses, side/floor constraints and repeated contact solving. Drop cooldown and blocked-spawn checks prevent overlapping rapid drops. Rendering and input adapters are separate from rules and presentation constants.

## Snake

Open **Select Game / 选择游戏** and choose **Snake / 贪吃蛇**. Press Space to start. Use the arrow keys or WASD to move, Space to pause or resume, R to restart, and Esc to return to game selection. Eat the food to gain points. The game ends if the snake hits a wall or itself; filling the board wins.

The snake moves at a fixed game speed, independent of the display FPS cap. Starting a new game resets its score and board.

## Tetris

Open **Select Game / 选择游戏** and choose **Tetris / 俄罗斯方块**. Press Space to start. The starting press does not hard-drop the first piece. Move with Left/Right (or A/D), rotate clockwise with Up, X, or W, rotate counterclockwise with Z, soft drop with Down or S, and hard drop with Space during play. Press P to pause or resume, R to restart, and Esc to return to game selection. Clear lines to score points; each ten lines raises the level and speeds up gravity.

## Breakout

Choose **Breakout / 打砖块**. Move the paddle with Left/Right or A/D, and press Space to serve. Hit all 50 bricks to win; missing the ball costs one of three lives. After losing a life, press Space to serve again. Paddle impact position determines the bounce angle. Press P to pause or resume, R to reset the board, and Esc to return. Physics uses small time steps independently of the display FPS cap.

## Minesweeper

Choose **Minesweeper / 扫雷**, select Easy, Normal, Hard or Custom, then press Space or click Start game. Custom controls change columns, rows and mine count. Click a value to edit it, and press Enter or click outside to finish editing before starting. Left-click to reveal and right-click to place or remove a flag. Mines are placed on the first reveal, keeping that cell and its neighbors safe. Empty areas open automatically. Clicking a revealed number opens its remaining neighbors when the adjacent flag count matches the number; incorrect flags can cause a loss. Reveal every safe cell to win. Press P to pause or resume, R to reset, and Esc to return. The timer begins on the first reveal and stops while paused or after the game ends.

## High scores

Snake, Tetris, and Breakout each keep their own best score. Minesweeper keeps separate shortest winning times in milliseconds for Easy, Normal and Hard (`minesweeper_easy_ms`, `minesweeper_normal_ms`, `minesweeper_hard_ms`; zero means no record yet). Custom games are not recorded. Old `minesweeper_best_ms` records migrate to Easy. Records are stored in `config/highscores.ini` and remain after restarting. Existing Snake/Tetris records load without migration; missing new keys default to zero. This personal file is ignored by Git; `config/highscores.example.ini` shows the defaults.

## Settings

### Automatic UI theme

Loading or selecting a main background extracts its dominant hue and updates menus, controls, cards and game surfaces. The palette keeps dark panels and bright text for readability. Grayscale and transparent images produce a neutral theme. Analysis runs once per successful background change; failed imports keep the previous theme. Player backdrops do not change the global theme. The theme is recreated from the saved background at startup.

### Music and custom backgrounds

The Media Library provides separate histories for the main background, player backdrop and background music. Open it from Basic settings (background) or Music settings (music/player backdrop). Select a saved entry and click Use selected, or Restore default. Add file imports and immediately selects a validated file. Previous/Next on the player cycles through the default track and saved music, skipping unavailable files.

New imports are copied into `config/media/` under content-based filenames, so deleting the original source does not lose them. The catalog in `config/media/library.txt` and the current selections in `config/settings.ini` survive restarting. The entire personal media folder is ignored by Git. Existing custom paths are remembered when first upgrading; those legacy files must remain available until re-imported. Failed imports keep the current selection and never replace older history entries.

The floating player supports pause/resume and dragging its header. Releasing a drag within 28 design units of the nearest window edge docks and collapses it into a small arrow tab. Click the inward-facing arrow to expand; drag it away to move freely. Hidden controls do not capture game input, and docking follows actual window edges across resolutions. Player mouse input is captured before menus and mouse-controlled games, including Minesweeper and Gomoku.

Music volume and all three current media selections are saved in the local settings file. Missing legacy files fall back to defaults on startup and remain listed so they can be identified and re-imported.

The settings page supports Chinese and English, 720p (1280×720), 1080p (1920×1080), 2K (2560×1440), and 4K (3840×2160), plus FPS caps of 60, 120, 165, and 240. The default is Chinese, 1080p, and 60 FPS. Changes apply immediately and are saved in `config/settings.ini` for the next launch. The FPS counter toggle is saved there too. This personal file is ignored by Git; `config/settings.example.ini` shows the defaults.

The selected resolution sets the window size. Press F9 to reset the window to 720p if a larger resolution does not fit your monitor.

## Requirements

Use the MSYS2 UCRT64 environment. Install the build tools and raylib:

```sh
pacman -S mingw-w64-ucrt-x86_64-raylib mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

raygui 5.0 is included in `external/raygui.h` under its zlib license (`external/raygui-LICENSE`). Its implementation lives only in `src/ui/raygui_impl.cpp`.

The menu font is a subset of [Noto Sans SC](https://github.com/google/fonts/tree/main/ofl/notosanssc), licensed under the SIL Open Font License in `assets/fonts/OFL.txt`. It includes UI labels, ASCII and the basic Chinese characters supported by the source font, so common imported filenames display correctly. Only UI and media filename glyphs are loaded into the runtime atlas; adding other scripts may require expanding the font asset.

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
- `assets/`: menu font, font license, and the cosmic pixel-art background.
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
