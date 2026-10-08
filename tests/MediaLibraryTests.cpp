#include <fstream>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include "media/MediaLibrary.h"
#include "config/AppSettings.h"
namespace {
void Require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
}
int main(int argc, char** argv) {
    try {
        Require(argc == 2, "Data directory required");
        const std::filesystem::path root = std::filesystem::path(argv[1]) /
            ("run-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(root);
        const auto first = root / std::filesystem::u8path("星空;一号.png");
        const auto second = root / "second.png";
        std::ofstream(first) << "first image";
        std::ofstream(second) << "second image";
        media::MediaLibrary library(root / "media");
        const auto use = [](const auto& path) { return std::filesystem::is_regular_file(path); };
        const auto a = library.Import(media::Kind::Background, first, use);
        Require(a.has_value() && a->name == "星空;一号.png", "Unicode names and list separators preserved");
        Require(library.Import(media::Kind::Background, first, use).has_value() && library.List(media::Kind::Background).size() == 1,
                "Repeated imports do not duplicate history");
        const auto b = library.Import(media::Kind::Background, second, use);
        Require(b && b->path != a->path && std::filesystem::exists(std::filesystem::u8path(a->path)), "New image preserves older image");
        Require(!library.Import(media::Kind::Music, second, [](const auto&) { return false; }), "Rejected candidate is not registered");
        Require(library.List(media::Kind::Music).empty(), "Failed import keeps history intact");
        for (const auto& file : std::filesystem::directory_iterator(root / "media/music")) {
            (void)file; Require(false, "Rejected candidate cleaned up");
        }
        Require(library.Import(media::Kind::PlayerBackground, first, use).has_value(), "Player background has independent history");
        Require(library.Import(media::Kind::Music, first, use).has_value(), "Music has independent history");
        Require(library.Save(), "History saved");
        std::filesystem::remove(first); std::filesystem::remove(second);
        media::MediaLibrary reopened(root / "media");
        Require(reopened.Load() && reopened.List(media::Kind::Background).size() == 2 &&
                reopened.List(media::Kind::PlayerBackground).size() == 1 && reopened.List(media::Kind::Music).size() == 1,
                "All categories survive restart and source deletion");
        Require(std::filesystem::is_regular_file(std::filesystem::u8path(reopened.List(media::Kind::Background)[0].path)), "Copied image remains usable");
        app::Settings settings;
        settings.playerBackgroundPath = reopened.List(media::Kind::PlayerBackground)[0].path;
        const auto ini = root / "settings.ini";
        Require(app::SaveSettings(settings, ini), "Selection saved");
        app::Settings loaded;
        Require(app::LoadSettings(loaded, ini) && loaded.playerBackgroundPath == settings.playerBackgroundPath,
                "Player background selection persists");
        settings.playerBackgroundPath.clear(); app::SaveSettings(settings, ini);
        Require(app::LoadSettings(loaded, ini) && loaded.playerBackgroundPath.empty(), "Default selection persists");
        std::ofstream(ini) << "language=en\n";
        Require(app::LoadSettings(loaded, ini) && loaded.playerBackgroundPath.empty(), "Legacy settings retain default player background");
        Require(reopened.Save() && reopened.Load(), "Catalog can replace an existing saved file");
        std::cout << "Media library tests passed\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
