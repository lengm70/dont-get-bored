#pragma once
#include <array>
#include "games/twentyone/TwentyOneMatchRules.h"
namespace games::twentyone {
using Money = matchRules::Money;
struct Stake { Money amount = 0; int score = 0; bool bust = false; };
using Payouts = std::array<Money, matchRules::maxSeats>;
// Every layer can only be won by participants who contributed to that layer.
// If all eligible hands bust, return that layer to its contributors.
Payouts SettlePots(const std::array<Stake, matchRules::maxSeats>& stakes, int count);
}  // namespace games::twentyone
