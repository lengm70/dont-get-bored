cmake_minimum_required(VERSION 3.16)

project(dont_get_bored VERSION 0.2.2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DONT_GET_BORED_RELEASE_VERSION "0.2.2")

find_package(raylib CONFIG REQUIRED)
find_package(Threads REQUIRED)

add_executable(dont_get_bored
    src/main.cpp
    src/app/Application.cpp
    src/app/MediaController.cpp
    src/media/MediaLibrary.cpp
    src/ui/MediaLibraryView.cpp
    src/ui/PlayerBackground.cpp
    src/audio/BackgroundMusic.cpp
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
    src/games/minesweeper/MinesweeperSetup.cpp
    src/games/gomoku/GomokuGame.cpp
    src/games/gomoku/GomokuAI.cpp
    src/games/gomoku/GomokuAiTurn.cpp
    src/games/gomoku/GomokuScreen.cpp
    src/ui/FontLoader.cpp
    src/ui/BackgroundImage.cpp
    src/ui/AdaptiveTheme.cpp
    src/ui/WindowIcon.cpp
    src/ui/MenuView.cpp
    src/ui/GameSelectionView.cpp
    src/ui/GameIllustration.cpp
    src/ui/Theme.cpp
    src/ui/raygui_impl.cpp
    src/platform/MusicFileDialog.cpp
)
target_compile_features(dont_get_bored PRIVATE cxx_std_17)
target_include_directories(dont_get_bored PRIVATE src external)
target_link_libraries(dont_get_bored PRIVATE raylib Threads::Threads)
add_custom_command(TARGET dont_get_bored POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:dont_get_bored>/assets")

if(WIN32)
    enable_language(RC)
    set_target_properties(dont_get_bored PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(dont_get_bored PRIVATE src/ui/WindowIdentityWin32.cpp)
    target_link_libraries(dont_get_bored PRIVATE shell32 comdlg32)
    configure_file(resources/AppIcon.rc.in "${CMAKE_CURRENT_BINARY_DIR}/AppIcon.rc" @ONLY)
    target_sources(dont_get_bored PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/AppIcon.rc")
    set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/AppIcon.rc" APPEND PROPERTY
        OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/app.ico")
endif()

option(BUILD_TESTING "Build game logic tests" OFF)
include(CTest)
if(BUILD_TESTING)
    add_executable(adaptive_theme_tests tests/AdaptiveThemeTests.cpp src/ui/AdaptiveTheme.cpp)
    target_include_directories(adaptive_theme_tests PRIVATE src)
    target_link_libraries(adaptive_theme_tests PRIVATE raylib)
    add_test(NAME adaptive_theme_tests COMMAND adaptive_theme_tests)
    add_executable(media_library_tests tests/MediaLibraryTests.cpp
        src/media/MediaLibrary.cpp src/config/AppSettings.cpp)
    target_include_directories(media_library_tests PRIVATE src)
    add_test(NAME media_library_tests COMMAND media_library_tests "${CMAKE_CURRENT_BINARY_DIR}/media-test-data")
    add_executable(gomoku_tests tests/GomokuTests.cpp
        src/games/gomoku/GomokuGame.cpp src/games/gomoku/GomokuAI.cpp
        src/games/gomoku/GomokuAiTurn.cpp)
    target_compile_features(gomoku_tests PRIVATE cxx_std_17)
    target_include_directories(gomoku_tests PRIVATE src)
    target_link_libraries(gomoku_tests PRIVATE Threads::Threads)
    add_test(NAME gomoku_tests COMMAND gomoku_tests)
    add_executable(game_logic_tests
        tests/GameLogicTests.cpp
        src/config/AppSettings.cpp
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
        src/games/gomoku/GomokuGame.cpp
        src/games/gomoku/GomokuAI.cpp
        src/games/gomoku/GomokuAiTurn.cpp
        src/games/gomoku/GomokuScreen.cpp
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
        src/ui/BackgroundImage.cpp
        src/ui/AdaptiveTheme.cpp
        src/ui/raygui_impl.cpp
        src/platform/MusicFileDialog.cpp
    )
    target_compile_features(game_input_tests PRIVATE cxx_std_17)
    target_include_directories(game_input_tests PRIVATE src external)
    target_link_libraries(game_input_tests PRIVATE raylib Threads::Threads)
    # Rename substituted queries so tests also link against static raylib builds.
    target_compile_definitions(game_input_tests PRIVATE
        IsKeyPressed=TestIsKeyPressed IsKeyDown=TestIsKeyDown
        IsKeyPressedRepeat=TestIsKeyPressedRepeat IsMouseButtonPressed=TestIsMouseButtonPressed
        GetMousePosition=TestGetMousePosition GetScreenWidth=TestGetScreenWidth
        GetScreenHeight=TestGetScreenHeight)
    if(WIN32)
        target_link_libraries(game_input_tests PRIVATE comdlg32)
    endif()
    add_test(NAME game_input_tests COMMAND game_input_tests)
    add_executable(background_image_tests
        tests/BackgroundImageTests.cpp
        src/ui/PlayerBackground.cpp
        src/ui/BackgroundImage.cpp
        src/ui/AdaptiveTheme.cpp
        src/platform/MusicFileDialog.cpp)
    target_compile_features(background_image_tests PRIVATE cxx_std_17)
    target_include_directories(background_image_tests PRIVATE src)
    target_link_libraries(background_image_tests PRIVATE raylib)
    target_compile_definitions(background_image_tests PRIVATE
        LoadTexture=TestLoadTexture IsTextureValid=TestIsTextureValid
        UnloadTexture=TestUnloadTexture SetTextureFilter=TestSetTextureFilter
        LoadImageFromTexture=TestLoadImageFromTexture
        DrawTexturePro=TestDrawTexturePro DrawRectangleGradientV=TestDrawRectangleGradientV)
    if(WIN32)
        target_link_libraries(background_image_tests PRIVATE comdlg32)
    endif()
    add_test(NAME background_image_tests COMMAND background_image_tests "${CMAKE_CURRENT_BINARY_DIR}/background-test-data")
endif()
