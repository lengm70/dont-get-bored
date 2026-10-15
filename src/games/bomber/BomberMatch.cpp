#include "games/bomber/BomberMatch.h"
#include <algorithm>
namespace games::bomber {
Match::Match(Options options) { Reset(options); }
void Match::Reset(Options options) {
    options_ = options;
    if (options_.mode != Mode::Solo && options_.mode != Mode::Duel) options_.mode = Mode::Solo;
    options_.targetWins = std::clamp(options.targetWins,1,rules::maxTargetWins);
    stage_ = round_ = 1; lives_ = rules::initialLives; score_ = 0; winner_ = -1;
    wins_ = {}; status_ = Status::Ready;
}
void Match::Start() { if (status_ == Status::Ready) status_ = Status::Playing; }
void Match::TogglePause() {
    if (status_ == Status::Playing) status_ = Status::Paused;
    else if (status_ == Status::Paused) status_ = Status::Playing;
}
bool Match::Advance() {
    if (status_ != Status::RoundOver && status_ != Status::StageComplete) return false;
    if (options_.mode == Mode::Solo && status_ == Status::StageComplete) ++stage_;
    ++round_; winner_ = -1; status_ = Status::Ready;
    return true;
}
void Match::AddScore(int points) { if (options_.mode == Mode::Solo && points > 0) score_ += points; }
void Match::Finish(Outcome outcome) {
    if (status_ != Status::Playing) return;
    if (options_.mode == Mode::Solo) {
        if (outcome == Outcome::SoloVictory) {
            AddScore(rules::stageBonus * stage_);
            status_ = stage_ == rules::stages ? Status::Won : Status::StageComplete;
        } else {
            --lives_; status_ = lives_ == 0 ? Status::GameOver : Status::RoundOver;
        }
    } else {
        winner_ = outcome == Outcome::PlayerOne ? 0 : outcome == Outcome::PlayerTwo ? 1 : -1;
        if (winner_ >= 0) ++wins_[winner_];
        status_ = winner_ >= 0 && wins_[winner_] >= options_.targetWins ? Status::MatchOver : Status::RoundOver;
    }
}
} // namespace games::bomber
