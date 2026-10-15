#pragma once
#include "games/twentyone/TwentyOneMatch.h"
#include "games/twentyone/TwentyOnePresentation.h"
#include "ui/MenuView.h"
namespace games::twentyone {
struct ScreenData {
    TwentyOneMatch match{};
    MatchOptions options{};
    Presentation presentation{};
    int raiseAmount=static_cast<int>(matchRules::defaultRaise);
    bool fundsEdit=false, roundsEdit=false, seatsEdit=false, raiseEdit=false;
};
void StartRound(ScreenData& data);
void UpdateRound(ScreenData& data,float elapsedSeconds);
ui::MenuResult DrawScreen(ScreenData& data,Font font,app::Language language);
void LoadAssets();
void UnloadAssets();
}  // namespace games::twentyone
