cmake_minimum_required(VERSION 3.16)

project(dont_get_bored VERSION 0.1.0 LANGUAGES CXX)

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
    src/ui/FontLoader.cpp
    src/ui/MenuView.cpp
    src/ui/Theme.cpp
    src/ui/raygui_impl.cpp
)
target_compile_features(dont_get_bored PRIVATE cxx_std_17)
target_include_directories(dont_get_bored PRIVATE src external)
target_link_libraries(dont_get_bored PRIVATE raylib)
