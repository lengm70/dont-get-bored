#include "app/MediaController.h"
#include "ui/BackgroundImage.h"
#include "ui/PlayerBackground.h"
#include "config/UiConfig.h"
namespace app {
void MediaController::Persist() const {
    if (!library_.Save()) TraceLog(LOG_WARNING, "Could not save media history");
    if (!SaveSettings(settings_)) TraceLog(LOG_WARNING, "Could not save media selection");
}
void MediaController::Initialize() {
    library_.Load();
    const std::string paths[]{settings_.backgroundPath, settings_.playerBackgroundPath, settings_.musicPath};
    music_.Initialize({}, settings_.musicVolume);
    for (int index = 0; index < 3; ++index) {
        const auto kind = static_cast<media::Kind>(index);
        if (!paths[index].empty()) {
            library_.Remember(kind, std::filesystem::u8path(paths[index]).filename().u8string(), paths[index]);
        }
        if (!Apply(kind, paths[index])) Apply(kind, {});
    }
    Persist();
}
bool MediaController::Apply(media::Kind kind, const std::string& path) {
    switch (kind) {
        case media::Kind::Background:
            if (!ui::LoadBackgroundImage(path.empty() ? ui::config::backgroundPath : path.c_str())) return false;
            settings_.backgroundPath = path; break;
        case media::Kind::PlayerBackground:
            if (!ui::LoadPlayerBackground(path.empty() ? ui::config::musicPlayerBackgroundPath : path.c_str())) return false;
            settings_.playerBackgroundPath = path; break;
        case media::Kind::Music:
            if (!music_.SelectTrack(std::filesystem::u8path(path))) return false;
            settings_.musicPath = path; break;
    }
    return true;
}
bool MediaController::Import(media::Kind kind, const std::filesystem::path& path) {
    const auto imported = library_.Import(kind, path, [&](const auto& copied) { return Apply(kind, copied.generic_u8string()); });
    if (!imported) return false;
    Persist();
    return true;
}
bool MediaController::Select(media::Kind kind, int index) {
    const auto entries = library_.List(kind);
    if (index < 0 || index > static_cast<int>(entries.size())) return false;
    if (!Apply(kind, index == 0 ? std::string{} : entries[index - 1].path)) return false;
    Persist(); return true;
}
bool MediaController::CycleMusic(int direction) {
    const auto entries = library_.List(media::Kind::Music);
    int current = 0;
    for (int index = 0; index < static_cast<int>(entries.size()); ++index) if (entries[index].path == settings_.musicPath) current = index + 1;
    const int count = static_cast<int>(entries.size()) + 1;
    for (int attempt = 1; attempt <= count; ++attempt) {
        const int next = ((current + direction * attempt) % count + count) % count;
        if (Select(media::Kind::Music, next)) return true;
    }
    return false;
}
std::vector<std::string> MediaController::Names() const {
    std::vector<std::string> result;
    for (auto kind : {media::Kind::Background, media::Kind::PlayerBackground, media::Kind::Music})
        for (const auto& entry : library_.List(kind)) result.push_back(entry.name);
    return result;
}
}
