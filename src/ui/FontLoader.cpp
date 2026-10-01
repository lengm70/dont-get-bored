#include "ui/FontLoader.h"

#include <algorithm>
#include <vector>

#include "config/UiConfig.h"
#include "ui/UiText.h"

namespace ui {

Font LoadMenuFont() {
    if (!FileExists(config::fontPath)) {
        TraceLog(LOG_ERROR, "Missing menu font: %s", config::fontPath);
        return {};
    }

    std::vector<int> codepoints;
    const auto collect = [&codepoints](const char* label) {
        int count = 0;
        int* loaded = LoadCodepoints(label, &count);
        if (loaded != nullptr) {
            codepoints.insert(codepoints.end(), loaded, loaded + count);
            UnloadCodepoints(loaded);
        }
    };
    collect(text::title);
    collect(text::resolutionChoices);
    collect(text::fpsChoices);
    collect(text::generatedTextGlyphs);
    for (const auto* labels : {&text::chinese, &text::english}) {
        for (const char* label : text::AllLabels(*labels)) collect(label);
    }
    for (const auto* labels : {&text::snakeChinese, &text::snakeEnglish}) {
        for (const char* label : text::AllLabels(*labels)) collect(label);
    }
    for (const auto* labels : {&text::tetrisChinese, &text::tetrisEnglish}) {
        for (const char* label : text::AllLabels(*labels)) collect(label);
    }

    std::sort(codepoints.begin(), codepoints.end());
    codepoints.erase(std::unique(codepoints.begin(), codepoints.end()), codepoints.end());

    Font font = LoadFontEx(config::fontPath, config::fontLoadSize,
                           codepoints.data(), static_cast<int>(codepoints.size()));
    if (!IsFontValid(font)) {
        TraceLog(LOG_ERROR, "Could not load menu font: %s", config::fontPath);
        return {};
    }
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    return font;
}

}  // namespace ui
