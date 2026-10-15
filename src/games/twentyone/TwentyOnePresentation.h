#pragma once
#include <array>
#include <cmath>
#include "ui/Motion.h"
#include "games/twentyone/TwentyOneMatch.h"
namespace games::twentyone {
struct Presentation {
    ui::motion::Orbit orbit;
    std::array<std::array<float,TWENTY_ONE_MAX_CARDS>,matchRules::maxSeats> cards{};
    std::array<int,matchRules::maxSeats> counts{},scores{};
    std::array<float,matchRules::maxSeats> scorePulse{},funds{};
    int round=0,focused=0,targetSeat=0;
    float frontProgress=1;
    MatchPhase phase=MatchPhase::Setup;
    float result=0;
    void Observe(const TwentyOneMatch& match,float dt) {
        if(match.Phase()==MatchPhase::Setup) { *this=Presentation{};return; }
        if(round!=match.Round()) {
            round=match.Round(); counts={};cards={};
            for(int index=0;index<match.Options().seats;++index) funds[index]=static_cast<float>(match.Seats()[index].funds);
        }
        if(match.Turn()>=0) targetSeat=match.Turn();
        orbit.Focus(targetSeat,match.Options().seats);orbit.Update(dt);
        if(focused!=targetSeat) {
            frontProgress=ui::motion::Smooth(frontProgress,0,ui::animationConfig::foregroundFadeRate,dt);
            if(frontProgress<.15f) focused=targetSeat;
        } else frontProgress=ui::motion::Smooth(frontProgress,orbit.Moving()?.4f:1.f,ui::animationConfig::foregroundRevealRate,dt);
        if(phase!=match.Phase()) {phase=match.Phase();result=0;}
        if(phase!=MatchPhase::Playing) result=ui::motion::Advance(result,ui::animationConfig::resultDuration,dt);
        for(int index=0;index<match.Options().seats;++index) {
            const auto& seat=match.Seats()[index];
            const int count=static_cast<int>(seat.hand.card_count);
            for(int card=counts[index];card<count;++card) cards[index][card]=0;
            counts[index]=count;
            for(int card=0;card<count;++card) cards[index][card]=ui::motion::Advance(cards[index][card],ui::animationConfig::dealDuration,dt);
            if(scores[index]!=seat.hand.score) { scores[index]=seat.hand.score;scorePulse[index]=1; }
            scorePulse[index]=std::max(0.f,scorePulse[index]-dt*ui::animationConfig::scoreDecay);
            funds[index]=ui::motion::Smooth(funds[index],static_cast<float>(seat.funds),14,dt);
        }
    }
    bool Busy() const {
        if(orbit.Moving()) return true;
        if(counts[focused]>0 && cards[focused][counts[focused]-1]<.85f) return true;
        return false;
    }
};
}  // namespace games::twentyone
