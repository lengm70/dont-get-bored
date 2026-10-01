#include "config/AppSettings.h"

#include <fstream>
#include <string>
#include <system_error>

namespace app {
namespace {

std::string Trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

const char* ResolutionId(Resolution resolution) {
    switch (resolution) {
        case Resolution::P720: return "720p";
        case Resolution::P1080: return "1080p";
        case Resolution::K2: return "2k";
        case Resolution::K4: return "4k";
    }
    return "1080p";
}

}  // namespace

WindowSize GetWindowSize(Resolution resolution) {
    switch (resolution) {
        case Resolution::P720: return {1280, 720};
        case Resolution::P1080: return {1920, 1080};
        case Resolution::K2: return {2560, 1440};
        case Resolution::K4: return {3840, 2160};
    }
    return {1920, 1080};
}

bool LoadSettings(Settings& settings, const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) return false;

    Settings loaded;
    std::string line;
    while (std::getline(input, line)) {
        const auto separator = line.find('=');
        if (separator == std::string::npos) continue;
        const std::string key = Trim(line.substr(0, separator));
        const std::string value = Trim(line.substr(separator + 1));

        if (key == "language") {
            if (value == "zh") loaded.language = Language::Chinese;
            else if (value == "en") loaded.language = Language::English;
        } else if (key == "resolution") {
            if (value == "720p") loaded.resolution = Resolution::P720;
            else if (value == "1080p") loaded.resolution = Resolution::P1080;
            else if (value == "2k") loaded.resolution = Resolution::K2;
            else if (value == "4k") loaded.resolution = Resolution::K4;
        } else if (key == "fps_limit") {
            if (value == "60" || value == "120" || value == "165" || value == "240") {
                loaded.fpsLimit = std::stoi(value);
            }
        } else if (key == "show_fps") {
            if (value == "true" || value == "1") loaded.showFps = true;
            else if (value == "false" || value == "0") loaded.showFps = false;
        }
    }
    if (input.bad()) return false;
    settings = loaded;
    return true;
}

bool SaveSettings(const Settings& settings, const std::filesystem::path& path) {
    std::error_code error;
    const auto parent = path.parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent, error);
    if (error) return false;

    std::ofstream output(path, std::ios::trunc);
    if (!output) return false;
    output << "# don't get bored settings\n"
           << "language=" << (settings.language == Language::English ? "en" : "zh") << '\n'
           << "resolution=" << ResolutionId(settings.resolution) << '\n'
           << "fps_limit=" << settings.fpsLimit << '\n'
           << "show_fps=" << (settings.showFps ? "true" : "false") << '\n';
    output.close();
    return output.good();
}

}  // namespace app
