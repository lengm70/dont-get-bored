#pragma once

#include <filesystem>

namespace app {

enum class Language { Chinese, English };
enum class Resolution { P720, P1080, K2, K4 };

struct Settings {
    Language language = Language::Chinese;
    Resolution resolution = Resolution::P1080;
    int fpsLimit = 60;
    bool showFps = false;
};

struct WindowSize {
    int width;
    int height;
};

inline constexpr const char* settingsPath = "config/settings.ini";

WindowSize GetWindowSize(Resolution resolution);
bool LoadSettings(Settings& settings, const std::filesystem::path& path = settingsPath);
bool SaveSettings(const Settings& settings, const std::filesystem::path& path = settingsPath);

}  // namespace app
