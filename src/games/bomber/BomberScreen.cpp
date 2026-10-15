#include "games/bomber/BomberScreen.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include "games/bomber/BomberConfig.h"
#include "games/bomber/BomberLabels.h"
#include "games/bomber/BomberVisual.h"
#include "ui/AnimatedControls.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiPalette.h"
namespace games::bomber {
namespace {
void Text(Font font, const std::string& text, float y, float size, Color color, const ui::UiLayout& layout) {
    const float available = (config::panel.width - 40) * layout.scale;
    const float measured = MeasureTextEx(font,text.c_str(),size * layout.scale,ui::config::textSpacing * layout.scale).x;
    if (measured > available) size *= available / measured;
    ui::DrawCenteredText(font,text.c_str(),y,size,color,layout);
}
void Start(BomberGame& game, ViewState& view) {
    if (view.setup) { game.Reset(view.options); view.setup = false; }
    view.held = {}; game.Start();
}
Direction ReadDirection(int player, bool solo, Direction previous) {
    const std::array<int,4> first{{KEY_W,KEY_D,KEY_S,KEY_A}}, second{{KEY_UP,KEY_RIGHT,KEY_DOWN,KEY_LEFT}};
    std::array<bool,4> held{}, pressed{};
    for (int i = 0; i < 4; ++i) {
        const int key = player == 0 ? first[i] : second[i];
        held[i] = IsKeyDown(key); pressed[i] = IsKeyPressed(key);
        if (solo) { held[i] = held[i] || IsKeyDown(second[i]); pressed[i] = pressed[i] || IsKeyPressed(second[i]); }
    }
    for (int i = 0; i < 4; ++i) if (pressed[i]) return static_cast<Direction>(i + 1);
    if (previous != Direction::None && held[static_cast<int>(previous) - 1]) return previous;
    for (int i = 0; i < 4; ++i) if (held[i]) return static_cast<Direction>(i + 1);
    return Direction::None;
}
void DrawSetup(BomberGame& game, ViewState& view, Font font, const Labels& t, const ui::UiLayout& layout) {
    Text(font,t.choose,config::setupTitleY,config::setupTitleSize,ui::palette::text,layout);
    if (ui::AnimatedButton(layout.Rect(config::soloButton),t.solo)) view.options.mode = Mode::Solo;
    if (ui::AnimatedButton(layout.Rect(config::duelButton),t.duel)) view.options.mode = Mode::Duel;
    DrawRectangleLinesEx(layout.Rect(view.options.mode == Mode::Solo ? config::soloButton : config::duelButton),
                         2 * layout.scale,config::playerOne);
    Text(font,view.options.mode == Mode::Solo ? t.soloDescription : t.duelDescription,config::setupDescriptionY,config::setupDescriptionSize,ui::palette::muted,layout);
    if (view.options.mode == Mode::Duel) {
        Text(font,t.targetWins,config::winsLabelY,17,ui::palette::muted,layout);
        if (ui::AnimatedButton(layout.Rect(config::fewerButton),"-")) view.options.targetWins = std::max(1,view.options.targetWins - 1);
        if (ui::AnimatedButton(layout.Rect(config::moreButton),"+")) view.options.targetWins = std::min(rules::maxTargetWins,view.options.targetWins + 1);
        Text(font,std::to_string(view.options.targetWins),config::winsValueY,24,config::playerOne,layout);
    } else Text(font,t.upgrades,config::winsValueY,16,config::playerOne,layout);
    Text(font,t.setupHint,config::setupHintY,15,ui::palette::muted,layout);
    if (ui::AnimatedButton(layout.Rect(config::startButton),t.start)) Start(game,view);
}
void DrawOverlay(const BomberGame& game, Font font, const Labels& t, const ui::UiLayout& layout) {
    if (game.State() == Status::Playing) return;
    const char* title = t.paused; const char* hint = t.resume;
    switch (game.State()) {
        case Status::Paused: break;
        case Status::RoundOver:
            title = game.Progress().Settings().mode == Mode::Solo ? t.lostLife :
                game.Progress().Winner() == 0 ? t.playerOneWins : game.Progress().Winner() == 1 ? t.playerTwoWins : t.draw;
            hint = t.nextRound; break;
        case Status::StageComplete: title = t.cleared; hint = t.nextStage; break;
        case Status::GameOver: title = t.gameOver; hint = t.restart; break;
        case Status::Won: title = t.won; hint = t.restart; break;
        case Status::MatchOver: title = t.matchOver; hint = t.restart; break;
        case Status::Ready: title = t.start; hint = t.start; break;
        case Status::Playing: break;
    }
    Text(font,title,config::overlayTitleY,config::overlayTitleSize,ui::palette::text,layout);
    const bool duel = game.Progress().Settings().mode == Mode::Duel;
    std::string detail;
    if (duel && game.State() != Status::Paused) {
        const int winner = game.Progress().Winner();
        detail = winner == 0 ? t.playerOneWins : winner == 1 ? t.playerTwoWins : t.draw;
        if (game.State() == Status::RoundOver) detail = "P1 " + std::to_string(game.Progress().Wins()[0]) + " : " + std::to_string(game.Progress().Wins()[1]) + " P2";
    } else if (!duel) detail = std::string(t.score) + ": " + std::to_string(game.Score()) + "   " + t.lives + ": " + std::to_string(game.Progress().Lives());
    Text(font,detail,config::overlayDetailY,config::overlayDetailSize,config::playerOne,layout);
    Text(font,hint,config::overlayHintY,config::overlayHintSize,ui::palette::muted,layout);
}
} // namespace
void UpdateFromInput(BomberGame& game, ViewState& view, float elapsedSeconds) {
    if (IsKeyPressed(KEY_R)) { game.Reset(view.options); view.setup = true; view.held = {}; return; }
    if (IsKeyPressed(KEY_SPACE) && (view.setup || game.State() == Status::Ready || game.State() == Status::RoundOver || game.State() == Status::StageComplete)) {
        Start(game,view); return;
    }
    if (view.setup) return;
    if (IsKeyPressed(KEY_P)) { game.TogglePause(); return; }
    const bool solo = game.Progress().Settings().mode == Mode::Solo;
    std::array<Input,2> input;
    for (int player = 0; player < (solo ? 1 : 2); ++player) {
        view.held[player] = ReadDirection(player,solo,view.held[player]);
        input[player].movement = view.held[player];
        input[player].plant = player == 0 ? IsKeyPressed(KEY_F) || (solo && IsKeyPressed(KEY_SPACE)) : IsKeyPressed(KEY_ENTER);
    }
    game.Update(elapsedSeconds,input);
}
void DrawScreen(BomberGame& game, ViewState& view, Font font, app::Language language, int bestScore) {
    const auto layout = ui::CurrentLayout(); const auto& t = ForLanguage(language);
    ui::DrawBackground(layout,false,config::panel);
    Text(font,t.name,config::titleY,config::titleSize,ui::palette::text,layout);
    const bool solo = (view.setup ? view.options.mode : game.Progress().Settings().mode) == Mode::Solo;
    std::string stats;
    if (solo) stats = std::string(t.score) + ": " + std::to_string(game.Score()) + "    " + t.best + ": " + std::to_string(bestScore) +
        "    " + t.lives + ": " + std::to_string(game.Progress().Lives());
    else stats = "P1 " + std::to_string(game.Progress().Wins()[0]) + " : " + std::to_string(game.Progress().Wins()[1]) + " P2    " +
        t.targetWins + ": " + std::to_string(view.setup ? view.options.targetWins : game.Progress().Settings().targetWins);
    Text(font,stats,config::statsY,config::statsSize,ui::palette::text,layout);
    if (!view.setup) {
        const int seconds = static_cast<int>(std::ceil(game.RemainingSeconds()));
        std::string hud = std::string(solo ? t.stage : t.round) + " " + std::to_string(solo ? game.Progress().Stage() : game.Progress().Round()) +
            (solo ? "/" + std::to_string(rules::stages) : "") + "   " + t.time + " " + std::to_string(seconds / 60) + ":" + (seconds % 60 < 10 ? "0" : "") + std::to_string(seconds % 60);
        if (solo) hud += "   " + std::string(t.enemies) + " " + std::to_string(game.EnemyCount());
        for (int i = 0; i < (solo ? 1 : 2); ++i) {
            const Actor& actor = game.Actors()[i];
            hud += "    P" + std::to_string(i + 1) + " " + t.capacity + " " + std::to_string(actor.capacity) + " / " + t.range + " " + std::to_string(actor.range);
            char speed[16]; std::snprintf(speed,sizeof(speed),"%.1f",actor.speed);
            hud += " / " + std::string(t.speed) + " " + speed;
        }
        if (game.Settling() && game.State() == Status::Playing) hud += "   " + std::string(t.settling);
        Text(font,hud,config::upgradesY,config::upgradesSize,config::playerOne,layout);
    }
    const Rectangle board = layout.Rect({config::boardX,config::boardY,config::boardWidth,config::boardHeight});
    DrawRectangleRec(board,ui::palette::board);
    const int clipX = static_cast<int>(std::ceil(board.x)), clipY = static_cast<int>(std::ceil(board.y));
    BeginScissorMode(clipX,clipY,static_cast<int>(std::floor(board.x + board.width)) - clipX,
                     static_cast<int>(std::floor(board.y + board.height)) - clipY);
    DrawArena(game,layout);
    if (view.setup || game.State() != Status::Playing) {
        DrawRectangleRec(board,ui::palette::overlay);
        if (view.setup) DrawSetup(game,view,font,t,layout);
        else DrawOverlay(game,font,t,layout);
    }
    EndScissorMode();
    DrawRectangleLinesEx(board,config::border * layout.scale,ui::palette::accent);
    Text(font,solo ? t.soloControls : t.duelControls,config::controlsY,config::controlsSize,ui::palette::muted,layout);
    Text(font,t.footer,config::footerY,config::footerSize,ui::palette::muted,layout);
}
} // namespace games::bomber
