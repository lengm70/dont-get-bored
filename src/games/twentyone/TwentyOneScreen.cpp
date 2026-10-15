#include "ui/AnimatedControls.h"
#include "games/twentyone/TwentyOneScreen.h"
#include <algorithm>
#include <cmath>
#include <string>
#include "raygui.h"
#include "games/twentyone/PlayingCardArt.h"
#include "games/twentyone/TwentyOneLabels.h"
#include "games/twentyone/TwentyOneMatchLabels.h"
#include "games/twentyone/TwentyOneScreenConfig.h"
#include "games/twentyone/TwentyOneArtConfig.h"
#include "ui/TextDraw.h"
#include "ui/Theme.h"
#include "ui/UiText.h"
#include "ui/UiPalette.h"
#include "ui/GeometricPanel.h"
namespace games::twentyone {
namespace {
namespace cfg=screenConfig;
struct ControlState {
    int previous=GuiGetState();
    explicit ControlState(bool enabled) { if(!enabled) GuiSetState(STATE_DISABLED); }
    ~ControlState() { GuiSetState(previous); }
};
std::string SeatName(int index,bool ai,const MatchLabels& labels) { return std::string(ai?labels.ai:labels.human)+" "+std::to_string(index+1); }
const char* HandState(const Seat& seat,const Labels& labels,const MatchLabels& matchLabels) {
    if(!seat.participating) return matchLabels.out;
    if(seat.hand.state==TWENTY_ONE_LOST) return labels.bust;
    if(seat.hand.state==TWENTY_ONE_WON) return labels.twentyone;
    if(seat.hand.state==TWENTY_ONE_HELD) return labels.held;
    return labels.active;
}
void DrawHand(const TwentyOnePlayer& hand,Rectangle design,Font font,const ui::UiLayout& layout,const Presentation& presentation,int seatIndex) {
    const int count=static_cast<int>(hand.card_count); if(count==0) return;
    const int columns=std::min(count,cfg::handColumns), rows=(count+columns-1)/columns;
    const float width=std::min({cfg::maxCardWidth,(design.width-(columns-1)*cfg::cardGap)/columns,
        (design.height-(rows-1)*cfg::cardGap)/rows/cfg::cardRatio});
    const float height=width*cfg::cardRatio;
    for(int index=0;index<count;++index) {
        const int row=index/columns, inRow=std::min(columns,count-row*columns);
        const float left=design.x+(design.width-inRow*width-(inRow-1)*cfg::cardGap)/2;
        const float progress=presentation.cards[seatIndex][index];
        if(progress<=0) continue;
        const float ease=ui::motion::Ease(progress), zoom=.65f+.35f*ease;
        const float targetX=left+(index%columns)*(width+cfg::cardGap),targetY=design.y+row*(height+cfg::cardGap);
        DrawPlayingCard(hand.cards[index],layout.Rect({cfg::tableX+(targetX-cfg::tableX)*ease,
            cfg::tableY+(targetY-cfg::tableY)*ease,width*zoom,height*zoom}),font);
    }
}
void DrawSetup(ScreenData& data,Font font,app::Language language,const ui::UiLayout& layout) {
    const auto& labels=MatchForLanguage(language);
    ui::DrawCenteredText(font,labels.setup,cfg::titleY,cfg::titleSize,ui::palette::text,layout);
    ui::DrawCenteredText(font,labels.limits,105,cfg::hintSize,ui::palette::muted,layout);
    const char* names[]{labels.funds,labels.rounds,labels.seats};
    for(int row=0;row<3;++row) ui::DrawScaledText(font,names[row],cfg::setupLabelX,cfg::setupFields[row].y+cfg::setupLabelOffset,18,ui::palette::text,layout);
    if(ui::AnimatedValueBox(layout.Rect(cfg::setupFields[0]),nullptr,&data.options.funds,matchRules::minFunds,matchRules::maxFunds,data.fundsEdit)) data.fundsEdit=!data.fundsEdit;
    if(ui::AnimatedValueBox(layout.Rect(cfg::setupFields[1]),nullptr,&data.options.rounds,1,matchRules::maxRounds,data.roundsEdit)) data.roundsEdit=!data.roundsEdit;
    if(ui::AnimatedSpinner(layout.Rect(cfg::setupFields[2]),nullptr,&data.options.seats,matchRules::minSeats,matchRules::maxSeats,data.seatsEdit)) data.seatsEdit=!data.seatsEdit;
    const int count=std::clamp(data.options.seats,matchRules::minSeats,matchRules::maxSeats);
    const std::string roles=std::string(labels.human)+";"+labels.ai;
    for(int index=0;index<count;++index) {
        const float x=cfg::setupSeatX+index*cfg::setupSeatStep;
        const auto name=std::to_string(index+1)+" / "+labels.roles;
        ui::DrawScaledText(font,name.c_str(),x,cfg::setupSeatY-24,14,ui::palette::muted,layout);
        int role=data.options.ai[index]?1:0;
        ui::AnimatedComboBox(layout.Rect({x,cfg::setupSeatY,cfg::setupSeatWidth,38}),roles.c_str(),&role);
        data.options.ai[index]=role!=0;
    }
    ui::DrawCenteredText(font,labels.rules,421,cfg::hintSize,ui::palette::muted,layout);
    const bool editing=data.fundsEdit || data.roundsEdit || data.seatsEdit;
    const bool enabled=TwentyOneMatch::Valid(data.options) && !editing;
    ControlState state(enabled);
    if(ui::AnimatedButton(layout.Rect(cfg::startButton),labels.start) || (enabled && !GuiIsLocked() && IsKeyPressed(KEY_SPACE))) data.match.Start(data.options);
}
void DrawSeats(const ScreenData& data,Font font,app::Language language,const ui::UiLayout& layout) {
    const auto& labels=ForLanguage(language); const auto& matchLabels=MatchForLanguage(language);
    const auto center=layout.Point(cfg::tableX,cfg::tableY);
    DrawEllipse(static_cast<int>(center.x),static_cast<int>(center.y),
        cfg::tableRadiusX*layout.scale,(cfg::tableRadiusY+8)*layout.scale,ui::palette::control);
    DrawEllipse(static_cast<int>(center.x),static_cast<int>(center.y),
        cfg::tableRadiusX*layout.scale,cfg::tableRadiusY*layout.scale,ui::palette::board);
    const float rotation=data.presentation.orbit.Angle();
    for(int segment=0;segment<cfg::tableSegments;++segment) {
        const float a=rotation+segment*2*PI/cfg::tableSegments, b=rotation+(segment+1)*2*PI/cfg::tableSegments;
        const auto point=[&](float angle,float radius){return layout.Point(cfg::tableX+std::cos(angle)*cfg::tableRadiusX*radius,
            cfg::tableY+std::sin(angle)*cfg::tableRadiusY*radius);};
        DrawLineEx(point(a,1),point(b,1),2*layout.scale,Fade(ui::palette::accent,.6f));
        DrawLineEx(point(a,.72f),point(b,.72f),layout.scale,Fade(ui::palette::accent,.18f));
        if(segment%4==0) DrawLineEx(point(a,.92f),point(a,1),2*layout.scale,ui::palette::accent);
    }
    DrawPolyLinesEx(center,6,42*layout.scale,rotation*RAD2DEG,layout.scale,Fade(ui::palette::accent,.22f));
    const auto pot=std::string(matchLabels.pot)+": "+std::to_string(data.match.Pot());
    ui::DrawCenteredText(font,pot.c_str(),cfg::tableY-9,20,ui::palette::accent,layout);
    const int focus=data.presentation.focused;
    for(int index=0;index<data.match.Options().seats;++index) {
        if(index==focus) continue;
        const auto& seat=data.match.Seats()[index];
        const float angle=PI/2+index*2*PI/data.match.Options().seats+rotation;
        const float x=cfg::tableX+std::cos(angle)*cfg::orbitRadiusX, y=cfg::tableY+std::sin(angle)*cfg::orbitRadiusY;
        const Rectangle bounds{x-cfg::distantWidth/2,y-cfg::distantHeight/2,cfg::distantWidth,cfg::distantHeight};
        ui::DrawGeometricPanel(layout.Rect(bounds),8*layout.scale,ui::palette::board,
            Fade(ui::palette::border,.8f),layout.scale);
        const auto name=SeatName(index,seat.ai,matchLabels);
        ui::DrawScaledText(font,name.c_str(),bounds.x+10,bounds.y+5,14,ui::palette::text,layout);
        const auto funds=std::string(matchLabels.balance)+": "+std::to_string(static_cast<Money>(std::llround(data.presentation.funds[index])));
        ui::DrawScaledText(font,funds.c_str(),bounds.x+10,bounds.y+23,12,ui::palette::muted,layout);
        const auto color=seat.hand.state==TWENTY_ONE_LOST?art::warmSuit:ui::palette::accent;
        ui::DrawScaledText(font,std::to_string(seat.hand.score).c_str(),bounds.x+bounds.width-43,bounds.y+5,
            23*(1+.08f*data.presentation.scorePulse[index]),color,layout);
        DrawHand(seat.hand,{bounds.x+8,bounds.y+38,bounds.width-16,44},font,layout,data.presentation,index);
    }
    // The seat arriving at the camera is shown large in the foreground.
    const auto& seat=data.match.Seats()[focus];
    Rectangle bounds=cfg::frontHand; bounds.y+=(1-data.presentation.frontProgress)*14;
    ui::DrawGeometricPanel(layout.Rect(bounds),cfg::seatCut*layout.scale,ui::palette::board,ui::palette::accent,2*layout.scale);
    const auto name=SeatName(focus,seat.ai,matchLabels)+"  "+matchLabels.balance+": "+
        std::to_string(static_cast<Money>(std::llround(data.presentation.funds[focus])));
    ui::DrawScaledText(font,name.c_str(),bounds.x+14,bounds.y+10,18,ui::palette::text,layout);
    const Color scoreColor=seat.hand.state==TWENTY_ONE_LOST?art::warmSuit:ui::palette::accent;
    const Rectangle badge{bounds.x+bounds.width-cfg::scoreBadgeWidth-8,bounds.y+5,cfg::scoreBadgeWidth,cfg::scoreBadgeHeight};
    ui::DrawGeometricPanel(layout.Rect(badge),6*layout.scale,ui::palette::control,scoreColor,layout.scale);
    ui::DrawScaledText(font,labels.score,badge.x+8,badge.y+3,12,ui::palette::text,layout);
    ui::DrawScaledText(font,std::to_string(seat.hand.score).c_str(),badge.x+8,badge.y+17,
        cfg::scoreSize*(1+.08f*data.presentation.scorePulse[focus]),scoreColor,layout);
    ui::DrawScaledText(font,"/21",badge.x+52,badge.y+30,13,ui::palette::muted,layout);
    const auto detail=std::string(matchLabels.stake)+": "+std::to_string(seat.stake)+"  "+
        (data.match.Phase()==MatchPhase::Playing?std::string(HandState(seat,labels,matchLabels)):
            std::string(matchLabels.payout)+": "+std::to_string(seat.payout));
    ui::DrawScaledText(font,detail.c_str(),bounds.x+14,bounds.y+37,14,ui::palette::muted,layout);
    DrawHand(seat.hand,{bounds.x+14,bounds.y+57,bounds.width-28,84},font,layout,data.presentation,focus);
    if(data.presentation.frontProgress<.99f) DrawRectangleRec(layout.Rect(bounds),Fade(ui::palette::board,1-data.presentation.frontProgress));
}
}
// Retained entry point resets to setup; Space starts the configured match.
void StartRound(ScreenData& data) { data=ScreenData{}; }
void UpdateRound(ScreenData& data,float seconds) {
    data.presentation.Observe(data.match,seconds);
    if(!data.presentation.Busy()) data.match.Update(seconds);
    data.presentation.Observe(data.match,0);
}
void LoadAssets() {}
void UnloadAssets() {}
ui::MenuResult DrawScreen(ScreenData& data,Font font,app::Language language) {
    const auto layout=ui::CurrentLayout(); const auto& labels=ForLanguage(language); const auto& moneyLabels=MatchForLanguage(language);
    ui::DrawBackground(layout,false,cfg::panel);
    if(data.match.Phase()==MatchPhase::Setup) DrawSetup(data,font,language,layout);
    else {
        ui::DrawCenteredText(font,labels.title,cfg::titleY,cfg::titleSize,ui::palette::text,layout);
        const auto summary=std::string(moneyLabels.round)+" "+std::to_string(data.match.Round())+" / "+std::to_string(data.match.Options().rounds);
        ui::DrawCenteredText(font,summary.c_str(),cfg::summaryY,18,ui::palette::text,layout);
        DrawSeats(data,font,language,layout);
        if(data.match.Phase()==MatchPhase::Playing) {
            const auto& seat=data.match.Seats()[data.match.Turn()];
            const auto turn=std::string(moneyLabels.turn)+": "+SeatName(data.match.Turn(),seat.ai,moneyLabels);
            ui::DrawScaledText(font,turn.c_str(),cfg::drawButton.x,cfg::resultY,18,ui::palette::accent,layout);
            const bool human=!seat.ai && !data.presentation.Busy(); const bool keyboard=human && !GuiIsLocked() && !data.raiseEdit;
            bool draw=false,hold=false,raise=false;
            { ControlState state(human);
              draw=ui::AnimatedButton(layout.Rect(cfg::drawButton),labels.draw); hold=ui::AnimatedButton(layout.Rect(cfg::holdButton),labels.hold); }
            ui::DrawScaledText(font,moneyLabels.raiseAmount,cfg::raiseLabelX,cfg::raiseLabelY,cfg::raiseLabelSize,ui::palette::text,layout);
            { ControlState state(human && seat.funds>0);
              const int previousTextSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
              GuiSetStyle(DEFAULT,TEXT_SIZE,static_cast<int>(std::lround(cfg::raiseValueSize*layout.scale)));
              if(ui::AnimatedValueBox(layout.Rect(cfg::raiseField),nullptr,&data.raiseAmount,1,static_cast<int>(std::max<Money>(1,seat.funds)),data.raiseEdit)) data.raiseEdit=!data.raiseEdit;
              GuiSetStyle(DEFAULT,TEXT_SIZE,previousTextSize); }
            if(!data.raiseEdit) data.raiseAmount=std::clamp(data.raiseAmount,1,static_cast<int>(std::max<Money>(1,seat.funds)));
            const bool canRaise=human && seat.funds>=data.raiseAmount && data.raiseAmount>0 && !data.raiseEdit;
            { ControlState state(canRaise); raise=ui::AnimatedButton(layout.Rect(cfg::raiseButton),moneyLabels.raise); }
            if(human && !data.raiseEdit) {
                if(draw || (keyboard && IsKeyPressed(KEY_D))) data.match.Act(MatchAction::Draw);
                else if(hold || (keyboard && IsKeyPressed(KEY_H))) data.match.Act(MatchAction::Hold);
                else if(raise || (keyboard && canRaise && IsKeyPressed(KEY_B))) data.match.Act(MatchAction::RaiseAndDraw,data.raiseAmount);
            }
        } else if(data.match.Phase()==MatchPhase::Finished) {
            const float reveal=ui::motion::Ease(data.presentation.result);
            auto resultLayout=layout; resultLayout.offsetY+=(1-reveal)*30*layout.scale;
            DrawRectangleRec(layout.Rect(cfg::panel),Fade(BLACK,.70f*reveal));
            ui::DrawGeometricPanel(resultLayout.Rect(cfg::winnerPanel),18*layout.scale,
                ui::palette::panel,ui::palette::accent,2*layout.scale);
            ui::DrawCenteredText(font,moneyLabels.finished,cfg::winnerTitleY,cfg::winnerTitleSize,ui::palette::text,resultLayout);
            std::string names;
            const auto leaders=data.match.Leaders();
            for(const int index:leaders) { if(!names.empty()) names+=" / "; names+=SeatName(index,data.match.Seats()[index].ai,moneyLabels); }
            float nameSize=cfg::winnerNamesSize;
            const float available=(cfg::winnerPanel.width-2*cfg::winnerPadding)*layout.scale;
            while(nameSize>18 && MeasureTextEx(font,names.c_str(),nameSize*layout.scale,ui::config::textSpacing*layout.scale).x>available) nameSize-=1;
            ui::DrawCenteredText(font,names.c_str(),cfg::winnerNamesY,nameSize,ui::palette::accent,resultLayout);
            ui::DrawCenteredText(font,moneyLabels.leaders,cfg::winnerFundsY-26,16,ui::palette::muted,resultLayout);
            const auto funds=std::to_string(data.match.Seats()[leaders.front()].funds);
            ui::DrawCenteredText(font,funds.c_str(),cfg::winnerFundsY,cfg::winnerFundsSize,ui::palette::text,resultLayout);
            ControlState resultState(data.presentation.result>.9f);
            const bool restart=ui::AnimatedButton(resultLayout.Rect(cfg::winnerButton),moneyLabels.restart);
            if(data.presentation.result>.9f && (restart || (!GuiIsLocked() && IsKeyPressed(KEY_SPACE)))) data.match=TwentyOneMatch{};
        } else {
            const float reveal=ui::motion::Ease(data.presentation.result);
            ui::DrawCenteredText(font,labels.again,cfg::resultY+(1-reveal)*12,22,Fade(ui::palette::accent,reveal),layout);
            ControlState nextState(data.presentation.result>.9f);
            const bool next=ui::AnimatedButton(layout.Rect(cfg::nextButton),moneyLabels.next);
            if(data.presentation.result>.9f && (next || (!GuiIsLocked() && IsKeyPressed(KEY_SPACE)))) data.match.NextRound();
        }
    }
    if(!GuiIsLocked() && !data.fundsEdit && !data.roundsEdit && !data.seatsEdit && !data.raiseEdit && IsKeyPressed(KEY_R)) data.match=TwentyOneMatch{};
    if(ui::AnimatedButton(layout.Rect(cfg::backButton),ui::text::ForLanguage(language).back)) return {ui::MenuAction::OpenGameSelection};
    ui::DrawCenteredText(font,moneyLabels.controls,cfg::footerY,cfg::hintSize,ui::palette::muted,layout);
    return {};
}
}  // namespace games::twentyone
