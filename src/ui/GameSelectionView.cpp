#include "ui/GameSelectionView.h"

#include <array>

#include "raygui.h"
#include "config/UiConfig.h"
#include "ui/GameIllustration.h"
#include "ui/GameSelectionConfig.h"
#include "ui/TextDraw.h"
#include "ui/UiText.h"

namespace ui {
namespace {

struct Entry {
    const char* name;
    const char* description;
    GameIllustration illustration;
    MenuAction action;
    Color accent;
};

bool CardButton(Rectangle bounds) {
    // Keep raygui's input behavior while drawing the rounded card ourselves.
    constexpr std::array<int, 6> properties{{
        BORDER_COLOR_NORMAL, BASE_COLOR_NORMAL, BORDER_COLOR_FOCUSED,
        BASE_COLOR_FOCUSED, BORDER_COLOR_PRESSED, BASE_COLOR_PRESSED
    }};
    std::array<int, properties.size()> saved{};
    for (std::size_t index = 0; index < properties.size(); ++index) {
        saved[index] = GuiGetStyle(BUTTON, properties[index]);
        GuiSetStyle(BUTTON, properties[index], 0);
    }
    const bool activated = GuiButton(bounds, nullptr);
    for (std::size_t index = 0; index < properties.size(); ++index) {
        GuiSetStyle(BUTTON, properties[index], saved[index]);
    }
    return activated;
}

bool DrawCard(const Entry& entry, Rectangle design, Font font, const char* play,
              const UiLayout& layout) {
    const Rectangle bounds = layout.Rect(design);
    const bool activated = CardButton(bounds);
    const bool hovered = !GuiIsLocked() && GuiGetState() != STATE_DISABLED &&
                         CheckCollisionPointRec(GetMousePosition(), bounds);
    const bool pressed = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    Rectangle shadow = bounds;
    shadow.y += selection::shadowOffset * layout.scale;
    DrawRectangleRounded(shadow, selection::cornerRoundness, selection::cornerSegments,
                         selection::shadowColor);
    DrawRectangleRounded(bounds, selection::cornerRoundness, selection::cornerSegments,
                         pressed ? selection::pressedColor : hovered ? selection::focusedColor : selection::cardColor);
    DrawRectangleRoundedLinesEx(bounds, selection::cornerRoundness, selection::cornerSegments,
        (hovered ? selection::focusBorderWidth : selection::borderWidth) * layout.scale,
        hovered ? entry.accent : selection::borderColor);
    const Rectangle artwork = layout.Rect({design.x + selection::contentPadding,
        design.y + selection::contentPadding, design.width - 2 * selection::contentPadding,
        selection::illustrationHeight});
    DrawGameIllustration(entry.illustration, artwork, font);
    const float textX = design.x + selection::contentPadding;
    DrawScaledText(font, entry.name, textX, design.y + selection::titleOffsetY,
                   selection::nameSize, config::textColor, layout);
    DrawScaledText(font, entry.description, textX, design.y + selection::descriptionOffsetY,
                   selection::descriptionSize, config::mutedColor, layout);
    DrawScaledText(font, play, textX, design.y + selection::playOffsetY,
                   selection::playSize, entry.accent, layout);
    const Vector2 arrow = layout.Point(design.x + design.width - selection::arrowInset,
                                      design.y + selection::playOffsetY + selection::playSize / 2);
    const float size = selection::arrowSize * layout.scale;
    DrawLineEx({arrow.x - size, arrow.y - size}, arrow, layout.scale, entry.accent);
    DrawLineEx(arrow, {arrow.x - size, arrow.y + size}, layout.scale, entry.accent);
    return activated;
}

}  // namespace

MenuResult DrawGameSelection(Font font, app::Language language, const UiLayout& layout) {
    const auto& labels = text::SelectionForLanguage(language);
    DrawCenteredText(font, text::ForLanguage(language).selectGame, selection::titleY,
                     selection::titleSize, config::textColor, layout);
    DrawCenteredText(font, labels.subtitle, selection::subtitleY, selection::subtitleSize,
                     config::mutedColor, layout);
    const std::array<Entry, selection::cardCount> entries{{
        {text::SnakeForLanguage(language).name, labels.snakeHint,
         GameIllustration::Snake, MenuAction::StartSnake, config::accentColor},
        {text::TetrisForLanguage(language).name, labels.tetrisHint,
         GameIllustration::Tetris, MenuAction::StartTetris, config::accentColor},
        {text::BreakoutForLanguage(language).name, labels.breakoutHint,
         GameIllustration::Breakout, MenuAction::StartBreakout, config::accentColor},
        {text::MinesweeperForLanguage(language).name, labels.minesweeperHint,
         GameIllustration::Minesweeper, MenuAction::StartMinesweeper, config::accentColor}
    }};
    constexpr float gridWidth = selection::cardCount * selection::cardWidth +
                               (selection::cardCount - 1) * selection::cardGap;
    for (int index = 0; index < selection::cardCount; ++index) {
        const Rectangle card{(config::designWidth - gridWidth) / 2 +
            index * (selection::cardWidth + selection::cardGap), selection::cardsY,
            selection::cardWidth, selection::cardHeight};
        if (DrawCard(entries[index], card, font, labels.play, layout)) return {entries[index].action};
    }
    if (GuiButton(layout.Rect(selection::backButton), text::ForLanguage(language).back)) {
        return {MenuAction::Back};
    }
    return {};
}

}  // namespace ui
