#pragma once
#include <array>
#include <random>
#include <vector>
#include "games/twentyone/TwentyOne.h"
#include "games/twentyone/TwentyOneEconomy.h"
namespace games::twentyone {
struct MatchOptions {
    int funds=matchRules::defaultFunds, rounds=matchRules::defaultRounds, seats=2;
    std::array<bool,matchRules::maxSeats> ai{{false,true,true,true}};
};
struct Seat {
    TwentyOnePlayer hand{};
    Money funds=0, stake=0, payout=0;
    bool ai=false, participating=false;
};
enum class MatchPhase { Setup, Playing, RoundResult, Finished };
enum class MatchAction { Draw, RaiseAndDraw, Hold };
class TwentyOneMatch {
public:
    explicit TwentyOneMatch(unsigned seed=std::random_device{}()) : random_(seed) {}
    static bool Valid(const MatchOptions& options);
    bool Start(const MatchOptions& options);
    bool Act(MatchAction action,Money raise=0);
    bool NextRound();
    void Update(float seconds);
    MatchPhase Phase() const { return phase_; }
    const std::array<Seat,matchRules::maxSeats>& Seats() const { return seats_; }
    const MatchOptions& Options() const { return options_; }
    int Round() const { return round_; }
    int Turn() const { return turn_; }
    Money Pot() const;
    std::vector<int> Leaders() const;
private:
    void BeginRound();
    void AdvanceTurn();
    void FinishRound();
    bool GiveCard(int index);
    std::mt19937 random_;
    MatchOptions options_{};
    std::array<Seat,matchRules::maxSeats> seats_{};
    std::vector<TwentyOneCard> deck_;
    MatchPhase phase_=MatchPhase::Setup;
    int round_=0, turn_=-1;
    float aiElapsed_=0;
};
}  // namespace games::twentyone
