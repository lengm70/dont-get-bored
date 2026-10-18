# v0.7.0 — Space Race

All twelve games are now present. Space Race has three distinct circuits and three-lap time trials, acceleration, braking, inertia and handbrake drift. Up/W accelerates, Down/S brakes, Left/Right (A/D) steers, Space drifts after starting. Left/Right selects a track on Ready. Ordered forward checkpoints prevent skipping laps; off-track travel slows the ship. Best total and best single lap are saved separately for each circuit. Time limit: three minutes.

# v0.6.0 — Rhythm

Eleven games. Three original synthesized songs (96/120/144 BPM), synchronized four-lane charts, Perfect/Good/Miss, empty-key penalties, combos, accuracy and grades. D/F/J/K hit; Left/Right selects a track on Ready, Up/Down changes a saved ±150ms judgment offset. Space starts without hitting a lane. P pauses both song and game; the global BGM suspends while playing. Records per track are in config/progress.ini. Audio assets must remain alongside the EXE; playback errors are shown rather than silently running an unsynchronized clock.

# v0.5.0 — Sokoban

Ten games. Sokoban adds 20 distinct puzzles, two introductory maps and progressively more boxes. Every map has a verified legal solution. Arrows/WASD move, Z undoes including a winning push, R resets. Left/Right on Ready selects unlocked levels; Space starts or advances. Completed level best moves and unlocks persist in config/progress.ini.

# v0.4.0 — Tank Battle

Nine games. Tank Battle: solo or local co-op, three waves, destructible brick/solid steel, enemy tanks and a shared base. Space starts; Tab selects co-op before starting; P1 WASD + F/Space, P2 arrows + Enter. Defend the base through all waves. P pauses, R restarts, Esc returns. Best scores are saved separately for solo/co-op in config/progress.ini (ignored by Git).

# don't get bored

A C++17 GUI collection with Snake, Tetris, Breakout, Minesweeper, Gomoku, Fruit Merge, Twenty One and Bomber, with Chinese and English menus. Every game starts with **Space** from its ready screen.

## Gomoku

Choose **Gomoku / 五子棋**, select Two players or Vs computer, then press Space or click Start game. Black plays first on a 15x15 board. Left-click near an intersection to place a stone. Five or more consecutive stones horizontally, vertically or diagonally wins; there are no forbidden moves. A full board without a winner is a draw. R resets the board to Ready; press Space to start again. Esc returns to game selection.

In computer mode you play Black. Easy, Normal and Hard use pattern evaluation and iterative Alpha-Beta search with maximum depths of 1, 3 and 5, and time budgets of 120, 500 and 1200 ms. Actual depth depends on the position and budget; these are search settings, not guaranteed skill ratings. The computer takes immediate wins and blocks immediate losses before considering other moves. Search runs on a board snapshot in a background worker, with cancellation on restart or leaving the game.

The Gomoku module separates rules/state (`GomokuGame`), search (`GomokuAI`), worker lifetime (`GomokuAiTurn`), presentation constants (`GomokuConfig`) and input/rendering (`GomokuScreen`). Tests cover win directions, overlines, draws, tactical replies, cancellation and mouse mapping.

## v0.3.0 — Stable release

Eight games, with Chinese and English menus and a consistent geometric science-fiction interface.

- **Bomber:** three solo sectors, local two-player duels, configurable target wins, chain reactions, equipment and persistent solo best.
- **Breakout:** five sectors, armored/explosive/moving bricks, obstacles, paddle/mouse improvements, three drops, combos, unlocks and persistent records.
- **Twenty One:** up to four players/AI, configurable starting funds and rounds, raising with a draw, pots/side pots, an orbiting table and animated turns.
- **Shared UI:** adaptive colors from the background, animated controls and transitions, illustrated game cards, custom background/music libraries and an edge-docking player.
- Windows icon and file metadata identify the stable v0.3.0 build. Previous settings and records remain compatible.

### Download and run

Download **dont-get-bored-v0.3.0-windows-x64.zip** from Releases, extract the entire archive, and double-click `dont_get_bored.exe`. Keep its DLLs and `assets` folder alongside it. No compiler or MSYS2 installation is needed to play. Settings, imported media and records are created locally after launch.

The separately attached EXE is the same game executable, for use with the archive's support files. **dont-get-bored-v0.3.0-source.zip** contains source and default assets; GitHub also provides automatic source archives. `SHA256SUMS.txt` lists attachment hashes.

## Previous internal beta: v0.3.0-beta.3

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

Choose **Breakout / 打砖块**. There are five progressively harder sectors: First Contact, Armor Array, Chain Reaction, Orbital Shift and Core Breach. Clear a sector to unlock the next one; use Up/Down or click a sector number on the ready screen to replay an unlocked sector.

- **Controls:** move the mouse inside the board, or use Left/Right / A/D. Space serves or advances after clearing a sector; the starting key never moves the ball immediately. P pauses, R starts a fresh run at the current sector, Esc returns, and S toggles screen shake (saved).
- **Feel:** short paddle acceleration and braking; impact position and paddle motion control the rebound angle. Circle collisions include paddle corners and sides. Small physics steps keep motion independent of the display FPS cap, with minimum bounce angles preventing horizontal and vertical loops.
- **Bricks:** armor needs two hits (three in the final sector); explosives damage eight neighboring cells and can trigger chains. Moving bricks shift sideways. Cross-marked solid obstacles cannot be destroyed and are excluded from the victory condition.
- **Drops:** every six destroyed bricks produces the next of three cyclic drops, with at most three falling at once: paddle extension for 12 seconds, up to three balls, or a shield that absorbs one falling ball. Collect with the paddle. Repeat pickups refresh extension or shield without stacking; all balls must fall before one life is lost. Buff timers and movement freeze when paused; buffs clear after losing a life or advancing a sector.
- **Scoring:** each brick is worth 10 points. Every five consecutive destroyed bricks raises the multiplier, up to x4. A lost life resets the combo. Each sector completion awards 100 points per remaining life and displays score, maximum combo and life bonus. Score and remaining lives continue to the next sector.
- **Persistence:** overall best remains in `config/highscores.ini`; unlocked sectors, completed-sector bests, campaign best and shake preference are saved in `config/breakout.ini`. The latter is created on first launch and written through a temporary file so a failed save preserves the previous copy. Both personal files are ignored by Git; `config/breakout.example.ini` documents defaults.
- **Feedback:** geometric bricks with health markers, cyan ball trails, impact flashes, blast fragments and optional contained screen shake match the collection's science-fiction style. All gameplay effects stay clipped to the board.

## Bomber / 炸弹人

Select **Bomber / 炸弹人** on the second page of game selection. Choose **Solo campaign** or **Local duel**; duels allow a target of 1–9 wins (default 3). Press **Space** or click the start button. The starting press does not also plant a bomb.

- **Solo:** WASD or arrows move; F or Space plants. Clear three sectors by eliminating all drones. You have three lives, with two, three and four drones in successive sectors. Drones pursue or patrol and use pathfinding to escape predicted blast lanes, including chain reactions; they do not plant bombs. Contact with a drone or any active blast, including your own, costs a life.
- **Local duel:** P1 uses WASD and F; P2 uses arrows and Enter. Both play simultaneously on a rotationally symmetric arena. Last survivor wins the round; simultaneous elimination is a draw and awards no wins. First to the configured number of wins takes the match. Keyboard hardware may limit how many keys can register at once.
- **Shared controls:** P pauses movement, fuses and the round timer. R returns to setup and resets the match. Esc returns to game selection. Space advances after a lost life, cleared sector or finished duel round. Each arena has a three-minute limit: timeout loses a solo life or draws a duel round.
- **Explosions:** bombs detonate after 2.3 seconds. Cross-shaped blasts stop at solid walls and the first crate or bomb. Bombs trigger other bombs immediately. Simultaneous explosions use a shared map snapshot, so destroying a crate cannot let another ray pass through it in the same chain reaction. A brief settlement window allows simultaneous casualties to resolve fairly.
- **Movement:** continuous cardinal movement, latest-pressed direction priority and gentle corridor alignment make corners forgiving. A newly planted bomb lets any overlapping actor leave; that actor cannot pass through it again after leaving.
- **Equipment:** some crates reveal blue capacity, orange range and green speed upgrades. Limits are five active bombs, seven cells of range and 5.3 cells/second. New drops remain protected until the initial flame fades; later explosions can destroy exposed drops. Upgrades carry to the next solo sector, reset after losing a life, and reset each duel round.
- **Scoring:** solo awards 10 per crate, 100 per drone and 300 times the sector number for clearing it. The solo best persists in `config/highscores.ini` under `bomber`; old game records remain compatible. Duel rounds do not affect solo records.
- **Art:** hexagonal energy cores, cyan/pink players, violet drones, bomb fuse rings, predicted blast cells, cross-beams, crate fragments and colored pickup pulses match the collection's geometric science-fiction style. Effects are clipped to the arena.

Code lives in `src/games/bomber/`: map generation, match rules, movement, explosions, AI and upgrades form a raylib-independent logic library; input/presentation, drawing, labels and visual configuration remain separate. `tests/BomberTests.cpp` covers map fairness, escape routes, chain reactions, collision, simultaneous outcomes, campaign/duel flow, AI safety, upgrade limits, frame rates and persistent records. Input adapter coverage is in `tests/GameInputTests.cpp`.

## Minesweeper

Choose **Minesweeper / 扫雷**, select Easy, Normal, Hard or Custom, then press Space or click Start game. Custom controls change columns, rows and mine count. Click a value to edit it, and press Enter or click outside to finish editing before starting. Left-click to reveal and right-click to place or remove a flag. Mines are placed on the first reveal, keeping that cell and its neighbors safe. Empty areas open automatically. Clicking a revealed number opens its remaining neighbors when the adjacent flag count matches the number; incorrect flags can cause a loss. Reveal every safe cell to win. Press P to pause or resume, R to reset, and Esc to return. The timer begins on the first reveal and stops while paused or after the game ends.

## High scores

Snake, Tetris, Breakout, Fruit Merge and solo Bomber each keep their own best score. Minesweeper keeps separate shortest winning times in milliseconds for Easy, Normal and Hard (`minesweeper_easy_ms`, `minesweeper_normal_ms`, `minesweeper_hard_ms`; zero means no record yet). Custom games are not recorded. Old `minesweeper_best_ms` records migrate to Easy. Records are stored in `config/highscores.ini` and remain after restarting. Existing Snake/Tetris records load without migration; missing new keys default to zero. This personal file is ignored by Git; `config/highscores.example.ini` shows the defaults.

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
- `src/games/breakout/BreakoutGame.*` / `BreakoutPhysics.cpp`: campaign lifecycle, scoring and fixed-step motion, independent of raylib.
- `src/games/breakout/BreakoutLevels.*`: five sector layouts, brick damage and explosion chains.
- `src/games/breakout/BreakoutPowerUps.*`: pickup types, buff timers and collision checks.
- `src/games/breakout/BreakoutProgress.*`: validated persistent unlocks, bests and shake preference.
- `src/games/breakout/BreakoutVisual.*` / `BreakoutConfig.h` / `BreakoutLabels.h`: geometric effects, visual configuration and Chinese/English text.
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


## 21 点 / Twenty One

进入二十一点后先配置对局，按空格开始。初始资金默认 1000（50–1000000），轮数默认 10（1–100），支持 2–4 席；每席可选择本地玩家或 AI，至少一位玩家。所有席位使用同一键鼠轮流行动，高亮框标记当前席位。每轮使用洗牌后的共享 52 张牌，不重复抽同一张。

每轮每席底注 50；资金不足时投入剩余资金。D / 普通抽牌免费；B / 加注并抽牌投入所选金额；H / 停牌。加注金额不能超过可用资金。所有未结束的席位轮流行动；AI 每 0.85 秒行动，低于 17 点抽牌，部分中等点数选择加注。21 点自动停牌，爆牌不能继续行动。

轮末按投入额逐层建立筹码池：每个分池只由投入该层且未爆牌的席位竞争，最高点数者获胜。平局均分；无法整除的余数按席位顺序分配。某层所有有资格席位均爆牌时，该层原额退回贡献者。零资金席位退出后续轮次，剩余一席有资金时提前结束。完成指定轮数后，资金最多者获胜；并列资金允许共同获胜。

空格 / 下一轮；R 返回设置，Esc 返回游戏选择。资金属于当前对局，不写入永久最高分或设置文件。牌面规则继续使用纯 C 的 `TwentyOne.c`；多轮状态在 `TwentyOneMatch`，资金结算在 `TwentyOneEconomy`，界面与常量分别在 Screen / ScreenConfig 中。纸牌与背景保持科幻几何风格和自动主题配色。

Tests additionally cover side-pot eligibility, ties/remainders, all-bust refunds, invalid raises, shared deck uniqueness, round progression, bankruptcy, AI turns and conservation of total funds across simulated matches.

### Motion and round-table presentation

二十一点采用透视圆桌：席位围绕桌面排列，轮到下一席时沿最短角度平滑转动视角，并淡入前景手牌；切换期间暂缓出牌和 AI 操作。支持发牌滑入、总分脉冲、资金数值过渡，以及结算面板入场。动画只管理展示，不改变牌面、轮数和筹码结算规则。

菜单/设置/游戏操作按钮增加平滑悬停与按压反馈、点击波纹；选项、数值输入、滑条、勾选也有反馈。游戏卡片悬停抬升，页面短渐显，背景图片交叉淡化和主题色渐变。播放器吸附收起及展开使用矩形插值，动画期间不激活隐藏控件。时间参数集中在 `ui/AnimationConfig.h` 和 `MusicPlayerConfig.h`；核心插值在 `ui/Motion.h`，具有独立测试。
