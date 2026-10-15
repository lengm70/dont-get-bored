#include "games/breakout/BreakoutScreen.h"
#include <algorithm>
#include <cmath>
#include <string>
#include "config/UiConfig.h"
#include "games/breakout/BreakoutConfig.h"
#include "games/breakout/BreakoutLabels.h"
#include "games/breakout/BreakoutVisual.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiLayout.h"
#include "ui/UiText.h"
namespace games::breakout {
namespace {
Rectangle SelectionRect(int level, const ui::UiLayout& layout) {
    return layout.Rect({config::selectionX + level * config::selectionGap, config::selectionY, config::selectionWidth, config::selectionHeight});
}
void Center(Font font, const std::string& text, float y, float size, Color color, const ui::UiLayout& layout) {
    ui::DrawCenteredText(font, text.c_str(), y, size, color, layout);
}
void DrawOverlay(const BreakoutGame& game, Font font, app::Language language,
                 const Progress& progress, const ui::UiLayout& layout) {
    if (game.State() == Status::Playing) return;
    const auto& base = ui::text::BreakoutForLanguage(language);
    const auto& labels = CampaignFor(language);
    const char* title = base.ready; const char* hint = base.startHint;
    switch (game.State()) {
        case Status::Paused: title = base.paused; hint = base.resumeHint; break;
        case Status::GameOver: title = base.gameOver; hint = base.restartHint; break;
        case Status::Won: title = base.won; hint = base.restartHint; break;
        case Status::LevelComplete: title = labels.cleared; hint = labels.next; break;
        case Status::Playing: break;
        case Status::Ready: break;
    }
    DrawRectangleRec(layout.Rect({config::boardX,config::boardY,rules::boardWidth,rules::boardHeight}), config::overlayColor);
    Center(font,title,config::overlayTitleY,config::overlayTitleSize,ui::config::textColor,layout);
    Center(font,hint,config::overlayHintY,config::overlayHintSize,config::paddleColor,layout);
    const bool complete = game.State() == Status::Won || game.State() == Status::LevelComplete;
    if (complete || game.State() == Status::GameOver) {
        Center(font,std::string(base.score) + ": " + std::to_string(game.Score()) + "    " + labels.levelScore + ": " +
               std::to_string(game.LevelScore()),config::summaryScoreY,config::summaryScoreSize,ui::config::textColor,layout);
        Center(font,std::string(labels.maxCombo) + ": " + std::to_string(game.MaxCombo()) + "    " + base.lives + ": " +
               std::to_string(game.Lives()),config::summaryComboY,config::summaryComboSize,ui::config::mutedColor,layout);
        if (complete) Center(font,std::string(labels.bonus) + ": +" + std::to_string(game.CompletionBonus()),config::summaryBonusY,config::summaryBonusSize,config::paddleColor,layout);
        if (complete) Center(font,std::string(labels.levelBest) + ": " + std::to_string(progress.levelBest[game.Level()]),config::summaryBestY,config::summaryBestSize,ui::config::mutedColor,layout);
    } else {
        Center(font,labels.names[game.Level()],config::levelNameY,config::levelNameSize,ui::config::textColor,layout);
        Center(font,labels.legend,config::legendY,config::legendSize,ui::config::mutedColor,layout);
        if (game.CanSelectLevel()) {
            for (int level = 0; level < rules::levelCount; ++level) {
                const Rectangle rect = SelectionRect(level,layout);
                const bool unlocked = level < progress.unlockedLevels;
                const bool selected = level == game.Level();
                const Color color = unlocked ? config::paddleColor : ui::config::mutedColor;
                const bool hovered = unlocked && CheckCollisionPointRec(GetMousePosition(),rect);
                DrawRectangleRec(rect,Fade(color,selected ? 0.25f : hovered ? 0.15f : 0.06f));
                DrawRectangleLinesEx(rect,(selected ? 2 : 1) * layout.scale,Fade(color,unlocked ? 1 : 0.4f));
                const std::string text = std::to_string(level + 1) + (unlocked ? "" : " X");
                const auto size = MeasureTextEx(font,text.c_str(),config::selectionFontSize * layout.scale,ui::config::textSpacing * layout.scale);
                DrawTextEx(font,text.c_str(),{rect.x + (rect.width - size.x) / 2,rect.y + (rect.height - size.y) / 2},
                           config::selectionFontSize * layout.scale,ui::config::textSpacing * layout.scale,color);
            }
            Center(font,labels.select,config::selectionHintY,config::selectionHintSize,ui::config::mutedColor,layout);
        }
    }
}
} // namespace
void UpdateFromInput(BreakoutGame& game, float elapsedSeconds, bool mouseCaptured, const Progress* progress) {
    if (IsKeyPressed(KEY_R)) { game.Reset(game.Level()); return; }
    if (IsKeyPressed(KEY_SPACE) && (game.State() == Status::Ready || game.State() == Status::LevelComplete)) {
        game.Start(); return;
    }
    if (IsKeyPressed(KEY_P)) { game.TogglePause(); return; }
    if (game.CanSelectLevel()) {
        const int unlocked = progress ? progress->unlockedLevels : 1;
        const int direction = static_cast<int>(IsKeyPressed(KEY_DOWN)) - static_cast<int>(IsKeyPressed(KEY_UP));
        if (direction) { game.Reset(std::clamp(game.Level() + direction,0,unlocked - 1)); return; }
        if (!mouseCaptured && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            for (int level = 0; level < unlocked; ++level)
                if (CheckCollisionPointRec(GetMousePosition(),SelectionRect(level,ui::CurrentLayout()))) {
                    game.Reset(level); return;
                }
    }
    const float movement = static_cast<float>(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) -
                           static_cast<float>(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    std::optional<float> target;
    if (mouseCaptured) game.ClearMouseTarget();
    if (!mouseCaptured && movement == 0) {
        const Vector2 delta = GetMouseDelta();
        const Vector2 mouse = GetMousePosition();
        const ui::UiLayout layout = ui::CurrentLayout();
        const Rectangle board = layout.Rect({config::boardX,config::boardY,rules::boardWidth,rules::boardHeight});
        if (!CheckCollisionPointRec(mouse,board)) game.ClearMouseTarget();
        if ((delta.x != 0 || delta.y != 0) && CheckCollisionPointRec(mouse,board))
            target = (mouse.x - layout.offsetX) / layout.scale - config::boardX;
    }
    game.Update(elapsedSeconds,movement,target);
}
void DrawScreen(const BreakoutGame& game, Font font, app::Language language, int bestScore, const Progress* saved) {
    const Progress defaults;
    const Progress& progress = saved ? *saved : defaults;
    const ui::UiLayout layout = ui::CurrentLayout();
    const auto& base = ui::text::BreakoutForLanguage(language);
    const auto& labels = CampaignFor(language);
    ui::DrawBackground(layout,false,config::gamePanel);
    Center(font,base.name,config::titleY,config::titleSize,ui::config::textColor,layout);
    const std::string stats = std::string(base.score) + ": " + std::to_string(game.Score()) + "    " + base.best + ": " +
        std::to_string(std::max(bestScore,progress.bestScore)) + "    " + base.lives + ": " + std::to_string(game.Lives());
    Center(font,stats,config::statsY,config::statsSize,ui::config::textColor,layout);
    std::string hud = std::string(labels.level) + " " + std::to_string(game.Level() + 1) + "/" + std::to_string(rules::levelCount) + "   " +
        labels.combo + " " + std::to_string(game.Combo()) + " x" + std::to_string(game.Multiplier()) + "   " +
        labels.levelBest + " " + std::to_string(progress.levelBest[game.Level()]);
    if (game.Powers().wideSeconds > 0) hud += "   " + std::string(labels.wide) + " " + std::to_string(static_cast<int>(std::ceil(game.Powers().wideSeconds))) + "s";
    if (game.Balls().size() > 1) hud += "   " + std::string(labels.multi) + " " + std::to_string(game.Balls().size());
    if (game.Powers().shield) hud += "   " + std::string(labels.shield);
    Center(font,hud,config::secondaryHudY,config::secondarySize,config::paddleColor,layout);
    const Rectangle board = layout.Rect({config::boardX,config::boardY,rules::boardWidth,rules::boardHeight});
    DrawRectangleRec(board,config::boardColor);
    // The clip stays fixed while the contents shake, including particles and missed balls.
    const int x = static_cast<int>(std::ceil(board.x)), y = static_cast<int>(std::ceil(board.y));
    BeginScissorMode(x,y,static_cast<int>(std::floor(board.x + board.width)) - x,
                     static_cast<int>(std::floor(board.y + board.height)) - y);
    DrawPlayfield(game,ImpactLayout(game,layout,progress.screenShake));
    DrawOverlay(game,font,language,progress,layout);
    EndScissorMode();
    DrawRectangleLinesEx(board,config::borderWidth * layout.scale,ui::config::accentColor);
    Center(font,labels.controls,config::footerY,config::footerSize,ui::config::mutedColor,layout);
    Center(font,std::string(progress.screenShake ? labels.shakeOn : labels.shakeOff) + " | " + labels.shortControls,
           config::secondaryFooterY,config::secondaryFooterSize,ui::config::mutedColor,layout);
}
} // namespace games::breakout
