#include <iostream>
#include <numeric>
#include <set>
#include <stdexcept>
#include "games/twentyone/TwentyOneMatch.h"
namespace {
void Require(bool value,const char* message) { if(!value) throw std::runtime_error(message); }
games::twentyone::Money Total(const games::twentyone::TwentyOneMatch& match) {
    games::twentyone::Money total=0;
    for(int index=0;index<match.Options().seats;++index) total+=match.Seats()[index].funds;
    return total+(match.Phase()==games::twentyone::MatchPhase::Playing?match.Pot():0);
}
}
int main() {
    using namespace games::twentyone;
    try {
        std::array<Stake,4> stakes{{{50,21,false},{100,20,false},{200,18,false},{200,25,true}}};
        auto payouts=SettlePots(stakes,4);
        Require(payouts==Payouts{200,150,200,0}, "Main and side pots exclude under-contributors and busted hands");
        stakes={{{51,20,false},{51,20,false},{51,25,true},{0,0,false}}};
        payouts=SettlePots(stakes,3);
        Require(payouts==Payouts{77,76,0,0}, "Ties conserve remainder and deterministic distribution");
        stakes={{{50,23,true},{100,24,true},{200,30,true},{0,0,false}}};
        Require(SettlePots(stakes,3)==Payouts{50,100,200,0}, "All-bust pots refund every contributor");
        stakes={{{50,19,false},{200,22,true},{200,25,true},{0,0,false}}};
        Require(SettlePots(stakes,3)==Payouts{150,150,150,0}, "Unwinnable side pot refunds its contributors");
        bool invalid=false;
        try { stakes[0].amount=-1; SettlePots(stakes,3); } catch(const std::invalid_argument&) { invalid=true; }
        Require(invalid,"Negative stakes rejected");
        MatchOptions options;
        Require(options.funds==1000 && options.rounds==10 && options.seats==2,"Requested defaults");
        TwentyOneMatch match(42);
        Require(match.Phase()==MatchPhase::Setup && !match.Act(MatchAction::Draw),"Setup cannot act");
        options.seats=4; options.ai.fill(false); options.rounds=10;
        Require(match.Start(options),"Four-human local match supported");
        Require(match.Pot()==200 && match.Seats()[0].funds==950 && Total(match)==4000,"Ante deducted exactly once");
        const int actor=match.Turn();
        Require(!match.Act(MatchAction::RaiseAndDraw,100000) && match.Turn()==actor && match.Seats()[actor].funds==950,"Invalid raise changes nothing");
        const auto cardCount=match.Seats()[actor].hand.card_count;
        Require(match.Act(MatchAction::RaiseAndDraw,75),"Raise draws a card");
        Require(match.Seats()[actor].funds==875 && match.Seats()[actor].stake==125 && match.Seats()[actor].hand.card_count==cardCount+1,"Raise and draw commit together");
        Require(match.Turn()!=actor || match.Phase()!=MatchPhase::Playing,"Turn passes to next seat");
        Require(Total(match)==4000,"Raising conserves total funds");
        for(int action=0;action<1000 && match.Phase()!=MatchPhase::Finished;++action) {
            if(match.Phase()==MatchPhase::Playing) match.Act(MatchAction::Hold);
            else match.NextRound();
            Require(Total(match)==4000,"Money conserved across every round");
            for(const auto& seat:match.Seats()) Require(seat.funds>=0,"Funds never negative");
        }
        Require(match.Phase()==MatchPhase::Finished && match.Round()==10 && !match.Leaders().empty(),"Custom ten-round match completes and ranks funds");
        Require(!match.NextRound() && !match.Act(MatchAction::Draw),"Finished match cannot advance or pay again");
        options.rounds=1; options.seats=2; options.ai[1]=true;
        Require(match.Start(options),"Human/AI match restarts");
        match.Act(MatchAction::Hold);
        const auto humanCards=match.Seats()[0].hand.card_count;
        for(int tick=0;tick<200 && match.Phase()==MatchPhase::Playing;++tick) match.Update(.1f);
        Require(match.Phase()==MatchPhase::Finished && match.Seats()[0].hand.card_count==humanCards,"AI completes while human holds");
        Require(Total(match)==2000,"AI raises preserve funds");
        options.funds=50; options.rounds=100; options.seats=4; options.ai.fill(false);
        Require(match.Start(options),"All-in ante supported");
        for(int action=0;action<1000 && match.Phase()!=MatchPhase::Finished;++action) {
            if(match.Phase()==MatchPhase::Playing) match.Act(MatchAction::Hold);
            else match.NextRound();
            Require(Total(match)==200,"Bankruptcy and side pots conserve funds");
        }
        Require(match.Phase()==MatchPhase::Finished,"Bankruptcy eventually ends match or reaches round cap");
        for(unsigned seed=0;seed<40;++seed) {
            TwentyOneMatch simulation(seed); options.funds=1000;options.rounds=10;
            Require(simulation.Start(options),"Simulation starts");
            for(int action=0;action<2000 && simulation.Phase()!=MatchPhase::Finished;++action) {
                if(simulation.Phase()==MatchPhase::RoundResult) simulation.NextRound();
                else {
                    const auto& seat=simulation.Seats()[simulation.Turn()];
                    if(seat.hand.score>=17) simulation.Act(MatchAction::Hold);
                    else if(seat.funds>=50 && action%3==0) simulation.Act(MatchAction::RaiseAndDraw,50);
                    else simulation.Act(MatchAction::Draw);
                }
                Require(Total(simulation)==4000,"Simulated multi-round money invariant");
                std::set<int> deck;
                for(const auto& seat:simulation.Seats()) for(std::size_t index=0;index<seat.hand.card_count;++index)
                    Require(deck.insert(seat.hand.cards[index].rank+13*seat.hand.cards[index].suit).second,"Shared deck has no duplicate cards in round");
            }
            Require(simulation.Phase()==MatchPhase::Finished,"Simulation terminates");
        }
        options.ai.fill(true); Require(!TwentyOneMatch::Valid(options),"At least one human required");
        options.ai[0]=false;options.seats=5;Require(!match.Start(options),"Seat upper bound enforced");
        options.seats=2;options.rounds=0;Require(!match.Start(options),"Round bounds enforced");
        std::cout<<"Twenty One match/economy tests passed\n";
    } catch(const std::exception& error) { std::cerr<<error.what()<<'\n';return 1; }
}
