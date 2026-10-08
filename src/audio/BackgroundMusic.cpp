#include "audio/BackgroundMusic.h"

#include <algorithm>

#include "config/UiConfig.h"

namespace audio {
bool BackgroundMusic::Initialize(const std::string& customTrackPath, float volume) {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return false;
    initialized_ = true;
    volume_ = std::clamp(volume, 0.0f, 1.0f);
    if (!LoadTrack(ui::config::defaultMusicPath)) return false;
    if (!customTrackPath.empty()) LoadCustomTrack(std::filesystem::u8path(customTrackPath));
    return true;
}

bool BackgroundMusic::LoadCustomTrack(const std::filesystem::path& path) {
    return SelectTrack(path);
}

bool BackgroundMusic::SelectTrack(const std::filesystem::path& path) {
    if (!initialized_) return false;
    const auto selected = path.empty() ? std::filesystem::path(ui::config::defaultMusicPath) : path;
    std::error_code error;
    if (!std::filesystem::is_regular_file(selected, error) || error) return false;
    return LoadTrack(selected);
}

void BackgroundMusic::TogglePause() {
    if (!trackLoaded_) return;
    paused_ = !paused_;
    if (paused_) PauseMusicStream(track_);
    else ResumeMusicStream(track_);
}

void BackgroundMusic::SetVolume(float volume) {
    volume_ = std::clamp(volume, 0.0f, 1.0f);
    if (trackLoaded_) SetMusicVolume(track_, volume_);
}

void BackgroundMusic::Update() {
    if (!initialized_ || !trackLoaded_ || paused_) return;
    UpdateMusicStream(track_);
    if (!IsMusicStreamPlaying(track_)) PlayMusicStream(track_);
}

void BackgroundMusic::Shutdown() {
    if (!initialized_) return;
    UnloadTrack();
    CloseAudioDevice();
    initialized_ = false;
}

bool BackgroundMusic::LoadTrack(const std::filesystem::path& path) {
    Music loaded = LoadMusicStream(path.u8string().c_str());
    if (!IsMusicValid(loaded)) return false;

    UnloadTrack();
    track_ = loaded;
    trackLoaded_ = true;
    paused_ = false;
    SetMusicVolume(track_, volume_);
    PlayMusicStream(track_);
    return true;
}

void BackgroundMusic::UnloadTrack() {
    if (!trackLoaded_) return;
    StopMusicStream(track_);
    UnloadMusicStream(track_);
    trackLoaded_ = false;
}

}  // namespace audio
