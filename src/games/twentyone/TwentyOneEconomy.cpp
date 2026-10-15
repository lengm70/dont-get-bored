#include "games/twentyone/TwentyOneEconomy.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
namespace games::twentyone {
Payouts SettlePots(const std::array<Stake, matchRules::maxSeats>& stakes, int count) {
    if(count < 1 || count > matchRules::maxSeats) throw std::invalid_argument("Invalid seat count");
    std::vector<Money> levels; Money total = 0;
    for(int index=0;index<count;++index) {
        const auto& stake=stakes[index];
        if(stake.score < 0 || stake.amount < 0 || stake.amount > std::numeric_limits<Money>::max() - total)
            throw std::invalid_argument("Invalid stake");
        total += stake.amount;
        if(stake.amount > 0) levels.push_back(stake.amount);
    }
    std::sort(levels.begin(),levels.end()); levels.erase(std::unique(levels.begin(),levels.end()),levels.end());
    Payouts payouts{}; Money previous=0;
    for(const Money level : levels) {
        Money pot=0; int best=-1; std::vector<int> winners;
        for(int index=0;index<count;++index) if(stakes[index].amount >= level) {
            pot += level-previous;
            if(!stakes[index].bust && stakes[index].score <= 21) best=std::max(best,stakes[index].score);
        }
        if(best < 0) {
            for(int index=0;index<count;++index) if(stakes[index].amount >= level) payouts[index] += level-previous;
        } else {
            for(int index=0;index<count;++index) if(stakes[index].amount >= level && !stakes[index].bust && stakes[index].score==best) winners.push_back(index);
            const Money share=pot/static_cast<Money>(winners.size()), remainder=pot%static_cast<Money>(winners.size());
            for(std::size_t index=0;index<winners.size();++index) payouts[winners[index]] += share+(static_cast<Money>(index)<remainder?1:0);
        }
        previous=level;
    }
    return payouts;
}
}  // namespace games::twentyone
