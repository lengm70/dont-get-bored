#include "ui/Theme.h"

#include <algorithm>
#include <cmath>

#include "raygui.h"
#include "config/UiConfig.h"
#include "ui/BackgroundImage.h"

namespace ui {
namespace {

int Scaled(int value, float scale) {
    return std::max(1, static_cast<int>(std::lround(value * scale)));
}

}  // namespace

void ConfigureGuiStyle(Font font, float scale) {
    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, Scaled(config::guiTextSize, scale));
    GuiSetStyle(DEFAULT, BORDER_WIDTH, Scaled(config::guiBorderWidth, scale));
    GuiSetStyle(DEFAULT, TEXT_PADDING, Scaled(config::guiTextPadding, scale));
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(config::baseNormal));
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, Scaled(config::guiListItemHeight, scale));
    GuiSetStyle(LISTVIEW, LIST_ITEMS_SPACING, Scaled(config::guiListItemSpacing, scale));
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH,
                Scaled(config::guiComboButtonWidth, scale));
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_SPACING,
                Scaled(config::guiComboButtonSpacing, scale));
    GuiSetStyle(CHECKBOX, TEXT_PADDING, Scaled(config::guiTextPadding, scale));
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(config::borderNormal));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(config::baseNormal));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(config::textColor));
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(config::accentColor));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(config::baseFocused));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(config::textColor));
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(config::accentColor));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(config::basePressed));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(config::textColor));
}

void DrawBackground(const UiLayout& layout, bool showAccent, Rectangle panel) {
    DrawBackgroundImage(GetScreenWidth(), GetScreenHeight());
    DrawRectangleRounded(layout.Rect(panel), config::panelRoundness,
                         config::panelSegments, config::panelColor);
    if (showAccent) {
        DrawRectangleRounded(layout.Rect(config::accentBar), config::accentRoundness,
                             config::accentSegments, config::accentColor);
    }
}

}  // namespace ui
