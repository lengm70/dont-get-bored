#include "ui/MediaLibraryView.h"
#include <algorithm>
#include "raygui.h"
#include "ui/MediaLibraryConfig.h"
#include "ui/UiText.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
namespace ui {
MenuResult DrawMediaLibrary(Font font, app::Language language, const media::MediaLibrary& library,
                           const app::Settings& settings, MediaLibraryViewState& state) {
    const auto layout = CurrentLayout();
    const auto& labels = text::MediaForLanguage(language);
    DrawBackground(layout, false, mediaConfig::panel);
    DrawCenteredText(font, labels.title, mediaConfig::titleY, mediaConfig::titleSize, config::textColor, layout);
    const bool categoryChanged = GuiComboBox(layout.Rect(mediaConfig::category), labels.categories, &state.category);
    if (categoryChanged) {
        state.selected = state.scroll = 0; state.failed = false;
    }
    const auto entries = library.List(static_cast<media::Kind>(state.category));
    const std::string* current = state.category == 0 ? &settings.backgroundPath :
        state.category == 1 ? &settings.playerBackgroundPath : &settings.musicPath;
    if (categoryChanged) {
        for (int index = 0; index < static_cast<int>(entries.size()); ++index)
            if (entries[index].path == *current) state.selected = index + 1;
    }
    state.selected = std::clamp(state.selected, 0, static_cast<int>(entries.size()));
    std::vector<char*> names{const_cast<char*>(labels.defaultItem)};
    for (const auto& entry : entries) names.push_back(const_cast<char*>(entry.name.c_str()));
    int focus = -1;
    GuiListViewEx(layout.Rect(mediaConfig::list), names.data(), static_cast<int>(names.size()),
                  &state.scroll, &state.selected, &focus);
    state.selected = std::clamp(state.selected, 0, static_cast<int>(entries.size()));
    const bool active = state.selected == 0 ? current->empty() : *current == entries[state.selected - 1].path;
    DrawCenteredText(font, state.failed ? labels.failure : active ? labels.current : labels.hint,
                     mediaConfig::statusY, mediaConfig::statusSize, config::mutedColor, layout);
    if (GuiButton(layout.Rect(mediaConfig::importButton), labels.import)) return {MenuAction::ImportMedia};
    if (GuiButton(layout.Rect(mediaConfig::useButton), labels.use)) return {MenuAction::UseMedia};
    if (GuiButton(layout.Rect(mediaConfig::defaultButton), labels.restore)) { state.selected = 0; return {MenuAction::UseMedia}; }
    if (GuiButton(layout.Rect(mediaConfig::backButton), text::ForLanguage(language).back)) return {MenuAction::Back};
    return {};
}
}
