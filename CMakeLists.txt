cmake_minimum_required(VERSION 3.16)

project(dont_get_bored VERSION 0.2.0 LANGUAGES CXX)

find_package(raylib CONFIG REQUIRED)

add_executable(dont_get_bored
    src/main.cpp
    src/app/Application.cpp
    src/config/AppSettings.cpp
    src/config/HighScores.cpp
    src/games/snake/SnakeGame.cpp
    src/games/snake/SnakeScreen.cpp
    src/games/tetris/TetrisGame.cpp
    src/games/tetris/TetrisScreen.cpp
    src/games/breakout/BreakoutGame.cpp
    src/games/breakout/BreakoutCollision.cpp
    src/games/breakout/BreakoutScreen.cpp
    src/games/minesweeper/MinesweeperGame.cpp
    src/games/minesweeper/MinesweeperScreen.cpp
    src/ui/FontLoader.cpp
    src/ui/MenuView.cpp
    src/ui/Theme.cpp
    src/ui/raygui_impl.cpp
)
target_compile_features(dont_get_bored PRIVATE cxx_std_17)
target_include_directories(dont_get_bored PRIVATE src external)
target_link_libraries(dont_get_bored PRIVATE raylib)

option(BUILD_TESTING "Build game logic tests" OFF)
include(CTest)
if(BUILD_TESTING)
    add_executable(game_logic_tests
        tests/GameLogicTests.cpp
        src/config/HighScores.cpp
        src/games/breakout/BreakoutGame.cpp
        src/games/breakout/BreakoutCollision.cpp
        src/games/minesweeper/MinesweeperGame.cpp
        src/games/snake/SnakeGame.cpp
        src/games/tetris/TetrisGame.cpp
    )
    target_compile_features(game_logic_tests PRIVATE cxx_std_17)
    target_include_directories(game_logic_tests PRIVATE src)
    add_test(NAME game_logic_tests COMMAND game_logic_tests "${CMAKE_CURRENT_BINARY_DIR}/test-data")
    add_executable(game_input_tests
        tests/GameInputTests.cpp
        src/games/breakout/BreakoutGame.cpp
        src/games/breakout/BreakoutCollision.cpp
        src/games/breakout/BreakoutScreen.cpp
        src/games/minesweeper/MinesweeperGame.cpp
        src/games/minesweeper/MinesweeperScreen.cpp
        src/games/snake/SnakeGame.cpp
        src/games/snake/SnakeScreen.cpp
        src/games/tetris/TetrisGame.cpp
        src/games/tetris/TetrisScreen.cpp
        src/ui/Theme.cpp
        src/ui/raygui_impl.cpp
    )
    target_compile_features(game_input_tests PRIVATE cxx_std_17)
    target_include_directories(game_input_tests PRIVATE src external)
    target_link_libraries(game_input_tests PRIVATE raylib)
    add_test(NAME game_input_tests COMMAND game_input_tests)
endif()
