#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include "ui/BackgroundImage.h"
#include "ui/PlayerBackground.h"

namespace {
unsigned nextId = 1, drawnId = 0, unloadedId = 0;
void Require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
std::string Read(const std::filesystem::path& path) {
    std::ifstream input(path);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
}
// Exercise replacement and real file operations without opening a graphics window.
extern "C" {
Texture2D LoadTexture(const char* path) {
    return Read(path) == "valid" ? Texture2D{nextId++, 16, 16, 1, 0} : Texture2D{};
}
Image LoadImageFromTexture(Texture2D) { return GenImageColor(1, 1, RED); }
bool IsTextureValid(Texture2D texture) { return texture.id != 0; }
void UnloadTexture(Texture2D texture) { unloadedId = texture.id; }
void SetTextureFilter(Texture2D, int) {}
void DrawTexturePro(Texture2D texture, Rectangle, Rectangle, Vector2, float, Color) { drawnId = texture.id; }
void DrawRectangleGradientV(int, int, int, int, Color, Color) { drawnId = 0; }
}

int main(int argc, char** argv) {
    try {
        Require(argc == 2, "Test data directory is required");
        const std::filesystem::path directory = argv[1];
        std::filesystem::create_directories(directory);
        const auto original = directory / "custom.png";
        const auto corrupt = directory / "corrupt.png";
        const auto valid = directory / "valid.png";
        std::ofstream(original) << "valid";
        std::ofstream(corrupt) << "corrupt";
        std::ofstream(valid) << "valid";
        Require(ui::LoadBackgroundImage(original.string().c_str()), "Initial background loads");
        ui::DrawBackgroundImage(960, 600);
        const auto oldId = drawnId;
        const auto oldThemeRevision = ui::palette::revision;
        Require(!ui::LoadBackgroundImage(corrupt.string().c_str()), "Corrupt load fails");
        ui::DrawBackgroundImage(960, 600);
        Require(drawnId == oldId && unloadedId == 0, "Failed load preserves original texture");
        Require(!ui::ImportBackgroundImage(corrupt, original), "Corrupt import fails");
        Require(Read(original) == "valid", "Failed import preserves original file");
        ui::DrawBackgroundImage(960, 600);
        Require(drawnId == oldId && unloadedId == 0, "Failed import preserves original texture");
        Require(ui::palette::revision == oldThemeRevision, "Failed import preserves theme");
        Require(!ui::ImportBackgroundImage(directory / "missing.png", original), "Missing source fails safely");
        const auto blocked = directory / "blocked.png";
        std::filesystem::create_directories(blocked);
        Require(!ui::ImportBackgroundImage(valid, blocked), "Failed file replacement rejects candidate");
        ui::DrawBackgroundImage(960, 600);
        Require(drawnId == oldId, "Failed replacement preserves current texture");
        Require(ui::ImportBackgroundImage(valid, original), "Valid import replaces original");
        ui::DrawBackgroundImage(960, 600);
        Require(drawnId != oldId && unloadedId == oldId, "Successful import swaps textures");
        Require(Read(original) == "valid", "Imported file is persisted");
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            Require(entry.path().filename().string().find("import-") != 0, "Staging files are cleaned up");
        }
        ui::UnloadBackgroundImage();
        Require(ui::LoadPlayerBackground(original.string().c_str()), "Player backdrop loads");
        ui::DrawPlayerBackground({0, 0, 224, 126});
        const auto playerId = drawnId;
        const auto previousUnloaded = unloadedId;
        Require(!ui::LoadPlayerBackground(corrupt.string().c_str()), "Invalid player backdrop rejected");
        ui::DrawPlayerBackground({0, 0, 224, 126});
        Require(drawnId == playerId && unloadedId == previousUnloaded, "Failed backdrop load preserves current texture");
        ui::UnloadPlayerBackground();
        std::cout << "Background replacement tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
