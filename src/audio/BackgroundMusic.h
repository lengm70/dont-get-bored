#pragma once

#include <filesystem>
#include <string>

#include "raylib.h"

namespace audio {

class BackgroundMusic {
public:
    bool Initialize(const std::string& customTrackPath, float volume);
    bool LoadCustomTrack(const std::filesystem::path& path);
    bool SelectTrack(const std::filesystem::path& path);
    void TogglePause();
    void SetVolume(float volume);
    void Update();
    void Shutdown();

private:
    bool LoadTrack(const std::filesystem::path& path);
    void UnloadTrack();

    Music track_{};
    bool initialized_ = false;
    bool trackLoaded_ = false;
    bool paused_ = false;
    float volume_ = 0.5f;
};

}  // namespace audio
