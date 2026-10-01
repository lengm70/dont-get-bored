# don't get bored

A C++17 GUI collection of small games. The collection includes Snake and Tetris, with Chinese and English menus.

## Download for Windows

Download `dont-get-bored-windows-x64.zip` from the [latest release](https://github.com/lengm70/dont-get-bored/releases/latest), extract the whole archive, and run `dont_get_bored.exe` inside the extracted folder. Keep the DLL files and `assets` folder beside the program. The separate EXE release asset is provided for users who already have those dependencies.

## Snake

Open **Select Game / 选择游戏** and choose **Snake / 贪吃蛇**. Press Space to start. Use the arrow keys or WASD to move, Space to pause or resume, R to restart, and Esc to return to game selection. Eat the food to gain points. The game ends if the snake hits a wall or itself; filling the board wins.

The snake moves at a fixed game speed, independent of the display FPS cap. Starting a new game resets its score and board.

## Tetris

Open **Select Game / 选择游戏** and choose **Tetris / 俄罗斯方块**. Press Enter to start. Move with Left/Right (or A/D), rotate clockwise with Up, X, or W, rotate counterclockwise with Z, soft drop with Down or S, and hard drop with Space. Press P to pause or resume, R to restart, and Esc to return to game selection. Clear lines to score points; each ten lines raises the level and speeds up gravity.

## High scores

Snake and Tetris each keep their own best score. Records update during play and are stored in `config/highscores.ini`, so they remain after restarting the program. This personal file is ignored by Git; `config/highscores.example.ini` shows the default values.

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
