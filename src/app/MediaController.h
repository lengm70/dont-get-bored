#pragma once
#include "audio/BackgroundMusic.h"
#include "config/AppSettings.h"
#include "media/MediaLibrary.h"
namespace app {
class MediaController {
public:
    MediaController(Settings& settings, audio::BackgroundMusic& music) : settings_(settings), music_(music) {}
    void Initialize();
    bool Import(media::Kind kind, const std::filesystem::path& path);
    bool Select(media::Kind kind, int index);
    bool CycleMusic(int direction);
    const media::MediaLibrary& Library() const { return library_; }
    std::vector<std::string> Names() const;
private:
    bool Apply(media::Kind kind, const std::string& path);
    void Persist() const;
    Settings& settings_;
    audio::BackgroundMusic& music_;
    media::MediaLibrary library_;
};
}
