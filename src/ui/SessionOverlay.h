#pragma once
#include "app/SessionGuard.h"
#include "games/common/ArcadeUi.h"
namespace ui {
struct HelpText {
    const char *goal;
    const char *first;
    const char *second;
};
HelpText GameHelp(app::Screen screen, app::Language language);
void DrawSessionOverlay(const app::SessionGuard &guard, app::Screen screen, Font font,
                        app::Language language);
inline constexpr const char *sessionGlyphs =
    "操作说明 按F1或Esc关闭 按P继续 失去焦点后不会自动继续 空格开始 暂停 重开 返回";
} // namespace ui
