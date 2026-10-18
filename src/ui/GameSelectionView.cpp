#include "ui/AnimatedControls.h"
#include "ui/GameSelectionView.h"
#include "games/bomber/BomberLabels.h"
#include "ui/GeometricPanel.h"

#include <array>

#include "raygui.h"
#include "games/twentyone/TwentyOneLabels.h"
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
    const bool activated = ui::AnimatedButton(bounds, nullptr);
    for (std::size_t index = 0; index < properties.size(); ++index) {
        GuiSetStyle(BUTTON, properties[index], saved[index]);
    }
    return activated;
}

bool DrawCard(const Entry& entry, Rectangle design, Font font, const char* play,
              const UiLayout& layout) {
    const Rectangle hitBounds=layout.Rect(design);
    const bool activated = CardButton(hitBounds);
    const auto effect=ui::GetControlMotion(hitBounds,nullptr);
    design.y-=effect.hover*3-effect.pressed*2;
    const Rectangle bounds=layout.Rect(design);
    const bool hovered = !GuiIsLocked() && GuiGetState() != STATE_DISABLED &&
                         CheckCollisionPointRec(GetMousePosition(), hitBounds);
    const bool pressed = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    Rectangle shadow = bounds;
    shadow.y += selection::shadowOffset * layout.scale;
    DrawGeometricPanel(shadow, selection::cornerCut * layout.scale, selection::shadowColor, BLANK, 0);
    const Color border = hovered ? entry.accent : selection::borderColor;
    DrawGeometricPanel(bounds, selection::cornerCut * layout.scale,
        pressed ? selection::pressedColor : hovered ? selection::focusedColor : selection::cardColor,
        border, (hovered ? selection::focusBorderWidth : selection::borderWidth) * layout.scale);
    // Short illuminated rails echo the playing-card frame.
    const float rail = selection::railLength * layout.scale;
    DrawLineEx({bounds.x + rail, bounds.y}, {bounds.x + 2 * rail, bounds.y}, 2 * layout.scale, entry.accent);
    DrawLineEx({bounds.x + bounds.width - 2 * rail, bounds.y + bounds.height},
               {bounds.x + bounds.width - rail, bounds.y + bounds.height}, layout.scale, Fade(entry.accent, 0.7f));
    const Rectangle artwork = layout.Rect({design.x + selection::contentPadding,
        design.y + selection::illustrationOffsetY, selection::illustrationHeight,
        selection::illustrationHeight});
    DrawGameIllustration(entry.illustration, artwork, font);
    const float textX = design.x + 2 * selection::contentPadding + selection::illustrationHeight;
    DrawScaledText(font, entry.name, textX, design.y + selection::titleOffsetY,
                   selection::nameSize, config::textColor, layout);
    const float available = (design.x + design.width - selection::contentPadding - textX) * layout.scale;
    const float measured = MeasureTextEx(font,entry.description,
        selection::descriptionSize*layout.scale,config::textSpacing*layout.scale).x;
    const float factor = measured > available ? available / measured : 1.f;
    DrawTextEx(font,entry.description,layout.Point(textX,design.y+selection::descriptionOffsetY),
        selection::descriptionSize*layout.scale*factor,config::textSpacing*layout.scale*factor,config::mutedColor);
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
    const std::array<Entry, 12> entries{{
        {text::SnakeForLanguage(language).name, labels.snakeHint,
         GameIllustration::Snake, MenuAction::StartSnake, config::accentColor},
        {text::TetrisForLanguage(language).name, labels.tetrisHint,
         GameIllustration::Tetris, MenuAction::StartTetris, config::accentColor},
        {text::BreakoutForLanguage(language).name, labels.breakoutHint,
         GameIllustration::Breakout, MenuAction::StartBreakout, config::accentColor},
        {text::MinesweeperForLanguage(language).name, labels.minesweeperHint,
         GameIllustration::Minesweeper, MenuAction::StartMinesweeper, config::accentColor},
        {text::GomokuForLanguage(language).name, labels.gomokuHint,
         GameIllustration::Gomoku, MenuAction::StartGomoku, config::accentColor},
        {text::FruitForLanguage(language).name, labels.fruitHint,
         GameIllustration::Fruit, MenuAction::StartFruit, config::accentColor},
        {games::twentyone::ForLanguage(language).title, games::twentyone::ForLanguage(language).description,
         GameIllustration::TwentyOne, MenuAction::StartTwentyOne, config::accentColor},
        {games::bomber::ForLanguage(language).name, games::bomber::ForLanguage(language).description,
         GameIllustration::Bomber, MenuAction::StartBomber, config::accentColor},
        {language==app::Language::Chinese?"坦克大战":"Tank Battle",language==app::Language::Chinese?"保护基地，击退敌军":"Defend the base; solo or co-op", GameIllustration::Tank,MenuAction::StartTank,config::accentColor},
        {language==app::Language::Chinese?"推箱子":"Sokoban",language==app::Language::Chinese?"将箱子推至全部目标":"20 puzzles, undo and best moves",GameIllustration::Sokoban,MenuAction::StartSokoban,config::accentColor},
        {language==app::Language::Chinese?"节奏点击":"Rhythm",language==app::Language::Chinese?"三首原创电子曲":"Three original tracks; four lanes",GameIllustration::Rhythm,MenuAction::StartRhythm,config::accentColor},
        {language==app::Language::Chinese?"太空竞速":"Space Race",language==app::Language::Chinese?"三条赛道，漂移计时":"Three tracks; drift time trials",GameIllustration::Race,MenuAction::StartRace,config::accentColor}
    }};
    static int page = 0;
    const int pageCount = (static_cast<int>(entries.size()) + selection::cardCount - 1) / selection::cardCount;
    if (ui::AnimatedButton(layout.Rect({300,509,60,38}), "<")) page = (page + pageCount - 1) % pageCount;
    if (ui::AnimatedButton(layout.Rect({600,509,60,38}), ">")) page = (page + 1) % pageCount;
    DrawCenteredText(font, TextFormat("%d / %d",page+1,pageCount),550,12,config::mutedColor,layout);
    constexpr float gridWidth = selection::columns * selection::cardWidth +
                               (selection::columns - 1) * selection::cardGap;
    for (int index = 0; index < selection::cardCount; ++index) {
        const int entryIndex = page * selection::cardCount + index;
        if (entryIndex >= static_cast<int>(entries.size())) break;
        const Rectangle card{(config::designWidth - gridWidth) / 2 +
            (index % selection::columns) * (selection::cardWidth + selection::cardGap),
            selection::cardsY + (index / selection::columns) * (selection::cardHeight + selection::rowGap),
            selection::cardWidth, selection::cardHeight};
        if (DrawCard(entries[entryIndex], card, font, labels.play, layout)) return {entries[entryIndex].action};
    }
    if (ui::AnimatedButton(layout.Rect(selection::backButton), text::ForLanguage(language).back)) {
        return {MenuAction::Back};
    }
    return {};
}

}  // namespace ui
