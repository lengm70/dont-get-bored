#include "ui/Theme.h"
#include "ui/GeometricPanel.h"

#include <algorithm>
#include <array>
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
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, ColorToInt(config::borderNormal));
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED, ColorToInt(config::baseNormal));
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, ColorToInt(config::mutedColor));
}

void DrawBackground(const UiLayout& layout, bool showAccent, Rectangle panel) {
    DrawBackgroundImage(GetScreenWidth(), GetScreenHeight());
    // Quiet constellation geometry stays near the edges, clear of game content.
    constexpr std::array<Vector2,8> nodes{{{48,88},{140,36},{908,110},{824,42},
                                         {900,510},{820,572},{54,480},{138,558}}};
    for (std::size_t index=0;index<nodes.size();index+=2) {
        const Vector2 first=layout.Point(nodes[index].x,nodes[index].y);
        const Vector2 second=layout.Point(nodes[index+1].x,nodes[index+1].y);
        DrawLineEx(first,second,layout.scale,Fade(config::accentColor,.18f));
        DrawPolyLinesEx(first,6,18*layout.scale,30,layout.scale,Fade(config::accentColor,.25f));
        DrawPolyLinesEx(second,4,7*layout.scale,45,layout.scale,Fade(config::accentColor,.35f));
    }
    const Rectangle bounds=layout.Rect(panel);
    DrawGeometricPanel(bounds,config::panelCornerCut*layout.scale,config::panelColor,
        Fade(config::borderNormal,.55f),layout.scale);
    const float inset=config::panelCornerCut*layout.scale;
    const float rail=config::panelRailLength*layout.scale;
    DrawLineEx({bounds.x+inset,bounds.y},{bounds.x+inset+rail,bounds.y},layout.scale,config::accentColor);
    DrawLineEx({bounds.x+bounds.width-inset-rail,bounds.y+bounds.height},
        {bounds.x+bounds.width-inset,bounds.y+bounds.height},layout.scale,config::accentColor);
    if (showAccent) {
        DrawRectangleRounded(layout.Rect(config::accentBar), config::accentRoundness,
                             config::accentSegments, config::accentColor);
    }
}

}  // namespace ui
