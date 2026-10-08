#pragma once
#include "media/MediaLibrary.h"
#include "ui/MenuView.h"
namespace ui {
struct MediaLibraryViewState {
    int category = 0;
    int selected = 0;
    int scroll = 0;
    bool failed = false;
};
MenuResult DrawMediaLibrary(Font font, app::Language language, const media::MediaLibrary& library,
                           const app::Settings& settings, MediaLibraryViewState& state);
}
