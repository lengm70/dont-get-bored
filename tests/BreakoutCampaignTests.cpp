#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "games/breakout/BreakoutProgress.h"
namespace {
using namespace games::breakout;
void Require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
void TestLevels() {
    for (int level = 0; level < rules::levelCount; ++level) {
        auto bricks = MakeLevel(level);
        Require(RemainingBricks(bricks) == (level == 4 ? 48 : 50),"Obstacle exclusion from victory count");
        for (int index = 0; index < rules::brickCount; ++index) {
            const auto bounds = CurrentBrickBounds(bricks,index,2.0f);
            Require(bounds.x >= 0 && bounds.x + bounds.width <= rules::boardWidth,"Moving bricks remain inside walls");
        }
        for (int i = 0; i < rules::brickCount; ++i) while (bricks[i].Alive() && bricks[i].Destructible()) DamageBricks(bricks,i);
        Require(RemainingBricks(bricks) == 0,"Every sector is clearable");
    }
    auto armor = MakeLevel(1);
    Require(DamageBricks(armor,0).destroyed.empty() && armor[0].health == 1,"Armor survives first hit");
    Require(DamageBricks(armor,0).destroyed.size() == 1,"Second hit destroys armor");
    BrickArray chain{};
    chain[0] = {BrickKind::Explosive,1}; chain[1] = {BrickKind::Explosive,1};
    chain[2] = {BrickKind::Normal,1}; chain[10] = {BrickKind::Armored,3};
    chain[11] = {BrickKind::Obstacle,1}; chain[9] = {BrickKind::Normal,1};
    const auto hit = DamageBricks(chain,0);
    Require(hit.destroyed.size() == 3 && chain[10].health == 1,"Chain explosions damage adjacent armor");
    Require(chain[11].Alive() && chain[9].Alive(),"Explosion respects obstacles and row edges");
}
void TestFeelAndPowers() {
    const Bounds paddle{200,rules::paddleY,rules::paddleWidth,rules::paddleHeight};
    Point stillPos{248,324}, movingPos = stillPos, stillVelocity{0,320}, movingVelocity = stillVelocity;
    BounceOffPaddle(stillPos,stillVelocity,paddle,0);
    BounceOffPaddle(movingPos,movingVelocity,paddle,rules::paddleSpeed);
    Require(movingVelocity.x > stillVelocity.x && movingVelocity.y < 0,"Moving paddle adds controlled directional influence");
    Point horizontal{320,0}; EnsureBallAngle(horizontal);
    Require(std::abs(horizontal.y) >= rules::ballSpeed * rules::minimumVerticalRatio - 0.001f &&
            std::abs(std::hypot(horizontal.x,horizontal.y) - rules::ballSpeed) < 0.001f,"Horizontal loops corrected without changing speed");
    Point vertical{0,-320}; EnsureBallAngle(vertical);
    Require(std::abs(vertical.x) >= rules::ballSpeed * rules::minimumHorizontalRatio - 0.001f,
            "Perfectly vertical columns cannot stall play");
    BreakoutGame game;
    game.Update(0.03f,1);
    game.Update(0.04f,-1);
    const float x = game.Paddle().x;
    game.Update(0.01f,-1);
    Require(game.Paddle().x < x,"Paddle reversal responds promptly");
    game.Reset(); game.Start();
    game.ActivatePowerUp(PowerUpKind::WidePaddle);
    game.ActivatePowerUp(PowerUpKind::Shield);
    game.ActivatePowerUp(PowerUpKind::MultiBall);
    game.ActivatePowerUp(PowerUpKind::MultiBall);
    Require(game.Balls().size() == rules::maximumBalls && game.Paddle().width > rules::paddleWidth && game.Powers().shield,
            "Three powers activate with a ball cap");
    const float wide = game.Powers().wideSeconds;
    const Point position = game.Ball();
    game.TogglePause(); game.Update(0.25f,1);
    Require(game.Powers().wideSeconds == wide && game.Ball().x == position.x,"Pause freezes buffs and balls");
    game.TogglePause();
    bool lostSingle = false;
    for (int frame = 0; frame < 20000 && game.Lives() == rules::initialLives; ++frame) {
        const auto oldBalls = game.Balls().size();
        const float away = game.Ball().x > rules::boardWidth / 2 ? -1.0f : 1.0f;
        game.Update(1.0f / 120,away);
        if (game.Balls().size() < oldBalls && game.State() == Status::Playing) {
            lostSingle = true;
            Require(game.Lives() == rules::initialLives,"Losing only some balls keeps the life");
        }
    }
    Require(lostSingle && game.Lives() == rules::initialLives - 1 && game.Combo() == 0 &&
            game.State() == Status::Ready && game.Balls().size() == 1,"All balls lost costs one life and resets combo");
    PowerUpState powers;
    powers.Activate(PowerUpKind::WidePaddle); powers.Advance(rules::wideDuration + 1);
    Require(powers.PaddleWidth() == rules::paddleWidth,"Extension expires");
    powers.Activate(PowerUpKind::Shield);
    Require(powers.ConsumeShield() && !powers.ConsumeShield(),"Shield absorbs exactly once");
    Require(DropTouchesPaddle({210,rules::paddleY - 5},paddle) && !DropTouchesPaddle({100,100},paddle),"Pickup hit testing");
    game.Reset();
    game.Update(std::numeric_limits<float>::quiet_NaN(),1);
    Require(std::isfinite(game.Ball().x),"Nonfinite elapsed time is ignored");
}
float PredictAim(const BreakoutGame& game, int frame) {
    const Point ball = game.Ball(), velocity = game.Velocity();
    float x = ball.x;
    if (velocity.y > 0) x += velocity.x * std::max(0.0f,(rules::paddleY - rules::ballRadius - ball.y) / velocity.y);
    const float span = rules::boardWidth - 2 * rules::ballRadius;
    x = std::fmod(x - rules::ballRadius,2 * span);
    if (x < 0) x += 2 * span;
    x = (x > span ? 2 * span - x : x) + rules::ballRadius;
    return x + 15 * std::sin(frame / 300.0f);
}
void TestCampaign(const std::filesystem::path& path) {
    Progress progress; BreakoutGame game;
    int completed = 0;
    for (int frame = 0; frame < 300000 && game.State() != Status::Won; ++frame) {
        if (game.State() == Status::LevelComplete) {
            Require(RecordProgress(progress,game),"Completion records best and unlocks next sector");
            Require(!RecordProgress(progress,game),"Completion cannot repeatedly award records");
            Require(game.CompletionBonus() == game.Lives() * rules::lifeBonus,"Life bonus is awarded once");
            const int score = game.Score(), lives = game.Lives(), next = game.Level() + 1;
            ++completed; game.Start();
            Require(game.Level() == next && game.Score() == score && game.Lives() == lives &&
                    game.State() == Status::Playing,"Next sector preserves campaign score and lives");
        }
        if (game.State() == Status::Ready) game.Start();
        if (game.State() == Status::GameOver) {
            std::cerr << "Autoplay failed on sector " << game.Level() + 1 << " score " << game.Score() << '\n';
            Require(false,"Campaign autoplay exhausted lives");
        }
        game.Update(1.0f / 120,0,PredictAim(game,frame));
    }
    Require(game.State() == Status::Won && completed == 4,"All five sectors complete without obstacle softlock");
    Require(RecordProgress(progress,game) && progress.unlockedLevels == 5 && game.MaxCombo() > rules::comboStep,
            "Final completion persists progress and combo scoring");
    progress.screenShake = false;
    Require(SaveProgress(progress,path),"Progress saves");
    Progress loaded;
    Require(LoadProgress(loaded,path) && loaded.levelBest == progress.levelBest && loaded.bestScore == game.Score() &&
            loaded.unlockedLevels == 5 && !loaded.screenShake,"Progress and shake preference survive restart");
    ++progress.bestScore;
    Require(SaveProgress(progress,path) && LoadProgress(loaded,path) && loaded.bestScore == progress.bestScore,
            "Atomic replacement preserves updated records");
    progress.levelBest[0] = -1;
    Require(!SaveProgress(progress,path) && LoadProgress(loaded,path) && loaded.levelBest[0] >= 0,
            "Invalid save preserves existing file");
    { std::ofstream corrupt(path); corrupt << "unlocked_levels=99\nbest_score=-1\nlevel_1_best=12junk\nscreen_shake=5\n"; }
    Require(LoadProgress(loaded,path) && loaded.unlockedLevels == 1 && loaded.bestScore == 0 && loaded.levelBest[0] == 0,
            "Invalid config values safely fall back");
    std::filesystem::remove(path);
}
} // namespace
int main(int argc, char** argv) {
    try {
        const std::filesystem::path path = argc > 1 ? argv[1] : "breakout-test.ini";
        TestLevels(); TestFeelAndPowers(); TestCampaign(path);
        std::cout << "Breakout campaign tests passed\n";
        return 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
