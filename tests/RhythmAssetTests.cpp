#include "games/rhythm/RhythmCharts.h"
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
int main(int argc, char **argv) {
    try {
        if (argc < 2)
            throw std::runtime_error("Asset directory required");
        for (int i = 0; i < 3; ++i) {
            auto name = std::filesystem::path(games::rhythm::GetTrack(i).path).filename();
            std::ifstream in(std::filesystem::path(argv[1]) / name, std::ios::binary);
            std::array<unsigned char, 44> h{};
            in.read(reinterpret_cast<char *>(h.data()), 44);
            if (in.gcount() != 44 || std::string(h.begin(), h.begin() + 4) != "RIFF" ||
                std::string(h.begin() + 8, h.begin() + 12) != "WAVE")
                throw std::runtime_error("Invalid WAV");
            auto u32 = [&](int p) {
                return unsigned(h[p]) | (unsigned(h[p + 1]) << 8) | (unsigned(h[p + 2]) << 16) |
                       (unsigned(h[p + 3]) << 24);
            };
            if (h[20] != 1 || h[22] != 1 || h[34] != 16 || u32(24) != 22050)
                throw std::runtime_error("Unexpected PCM format");
            double duration = u32(40) / (22050.0 * 2);
            double expected = 4 + 64 * 60.0 / games::rhythm::GetTrack(i).bpm;
            if (std::abs(duration - expected) > .01)
                throw std::runtime_error("Music/chart duration mismatch");
        }
        std::cout << "3 original audio assets match chart tempos and duration\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
