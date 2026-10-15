#include "games/twentyone/TwentyOneMatch.h"
#include <algorithm>
#include <cmath>
namespace games::twentyone {
bool TwentyOneMatch::Valid(const MatchOptions& options) {
    if(options.funds<matchRules::minFunds || options.funds>matchRules::maxFunds || options.rounds<1 ||
       options.rounds>matchRules::maxRounds || options.seats<matchRules::minSeats || options.seats>matchRules::maxSeats) return false;
    for(int index=0;index<options.seats;++index) if(!options.ai[index]) return true;
    return false;
}
bool TwentyOneMatch::Start(const MatchOptions& options) {
    if(!Valid(options)) return false;
    options_=options; seats_={}; round_=0;
    for(int index=0;index<options.seats;++index) { seats_[index].funds=options.funds; seats_[index].ai=options.ai[index]; }
    BeginRound(); return true;
}
Money TwentyOneMatch::Pot() const { Money total=0; for(int index=0;index<options_.seats;++index) total+=seats_[index].stake; return total; }
void TwentyOneMatch::BeginRound() {
    ++round_; turn_=-1; aiElapsed_=0; phase_=MatchPhase::Playing;
    deck_.clear();
    for(int suit=0;suit<4;++suit) for(int rank=1;rank<=13;++rank) deck_.push_back({rank,static_cast<TwentyOneSuit>(suit)});
    std::shuffle(deck_.begin(),deck_.end(),random_);
    for(int index=0;index<options_.seats;++index) {
        auto& seat=seats_[index]; seat.stake=seat.payout=0;
        twenty_one_init(&seat.hand,"Seat"); seat.participating=seat.funds>0;
        if(!seat.participating) { twenty_one_hold(&seat.hand); continue; }
        seat.stake=std::min(matchRules::ante,seat.funds); seat.funds-=seat.stake;
        GiveCard(index);
    }
    AdvanceTurn();
}
bool TwentyOneMatch::GiveCard(int index) {
    if(deck_.empty() || seats_[index].hand.card_count>=TWENTY_ONE_MAX_CARDS) { twenty_one_hold(&seats_[index].hand); return false; }
    const auto card=deck_.back();
    if(!twenty_one_add_card(&seats_[index].hand,card.rank,card.suit)) return false;
    deck_.pop_back(); return true;
}
bool TwentyOneMatch::Act(MatchAction action,Money raise) {
    if(phase_!=MatchPhase::Playing || turn_<0) return false;
    if(action!=MatchAction::Draw && action!=MatchAction::RaiseAndDraw && action!=MatchAction::Hold) return false;
    auto& seat=seats_[turn_];
    if(action==MatchAction::RaiseAndDraw) {
        if(raise<=0 || raise>seat.funds || deck_.empty() || seat.hand.card_count>=TWENTY_ONE_MAX_CARDS) return false;
        seat.funds-=raise; seat.stake+=raise;
    }
    if(action==MatchAction::Hold) twenty_one_hold(&seat.hand);
    else GiveCard(turn_);
    AdvanceTurn(); return true;
}
void TwentyOneMatch::AdvanceTurn() {
    aiElapsed_=0;
    for(int step=1;step<=options_.seats;++step) {
        const int next=(turn_+step)%options_.seats;
        if(seats_[next].participating && seats_[next].hand.state==TWENTY_ONE_ACTIVE) { turn_=next; return; }
    }
    FinishRound();
}
void TwentyOneMatch::FinishRound() {
    std::array<Stake,matchRules::maxSeats> stakes{};
    for(int index=0;index<options_.seats;++index) stakes[index]={seats_[index].stake,seats_[index].hand.score,seats_[index].hand.state==TWENTY_ONE_LOST};
    const auto payouts=SettlePots(stakes,options_.seats);
    int solvent=0;
    for(int index=0;index<options_.seats;++index) {
        seats_[index].payout=payouts[index]; seats_[index].funds+=payouts[index];
        if(seats_[index].funds>0) ++solvent;
    }
    turn_=-1; phase_=round_>=options_.rounds || solvent<=1 ? MatchPhase::Finished : MatchPhase::RoundResult;
}
bool TwentyOneMatch::NextRound() {
    if(phase_!=MatchPhase::RoundResult) return false;
    BeginRound(); return true;
}
void TwentyOneMatch::Update(float seconds) {
    if(phase_!=MatchPhase::Playing || turn_<0 || !seats_[turn_].ai || !std::isfinite(seconds) || seconds<=0) return;
    aiElapsed_+=std::min(seconds,0.25f);
    if(aiElapsed_<matchRules::aiDelay) return;
    const auto& seat=seats_[turn_];
    if(!twenty_one_ai_should_draw(&seat.hand)) { Act(MatchAction::Hold); return; }
    if(seat.hand.score>=9 && seat.hand.score<=14 && seat.funds>=matchRules::defaultRaise &&
       std::uniform_int_distribution<int>(0,3)(random_)==0) Act(MatchAction::RaiseAndDraw,matchRules::defaultRaise);
    else Act(MatchAction::Draw);
}
std::vector<int> TwentyOneMatch::Leaders() const {
    Money best=-1; std::vector<int> leaders;
    for(int index=0;index<options_.seats;++index) {
        if(seats_[index].funds>best) { best=seats_[index].funds; leaders.clear(); }
        if(seats_[index].funds==best) leaders.push_back(index);
    }
    return leaders;
}
}  // namespace games::twentyone
