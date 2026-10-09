#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "games/fruit/FruitGame.h"
#include "games/fruit/FruitPhysics.h"
#include "config/HighScores.h"
namespace {
void Require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
void Advance(games::fruit::FruitGame& game, float seconds, float dt = 1.0f / 120) {
    for (int step = 0; step < static_cast<int>(std::lround(seconds / dt)); ++step) game.Update(dt);
}
}
int main(int argc, char** argv) {
    using namespace games::fruit;
    try {
        Fruit light{0, {100, 100}, {160, 40}}, heavy{4, {130, 100}, {0, 0}};
        const float momentumX = physics::Mass(light) * light.velocity.x;
        const float momentumY = physics::Mass(light) * light.velocity.y;
        physics::Resolve(light, heavy);
        Require(std::abs(momentumX - physics::Mass(light) * light.velocity.x - physics::Mass(heavy) * heavy.velocity.x) < 0.01f, "Collision conserves linear momentum X");
        Require(std::abs(momentumY - physics::Mass(light) * light.velocity.y - physics::Mass(heavy) * heavy.velocity.y) < 0.01f, "Friction transfers momentum between bodies");
        Require(heavy.velocity.x < 40 && light.velocity.x < 0, "Heavy body resists impact while light body rebounds");
        Require(std::abs(light.angularVelocity) > 0.01f && light.impact > 0, "Tangential collision creates rotation and feedback");
        Fruit floor{3, {150, rules::height}, {100, 220}};
        physics::Constrain(floor);
        Require(floor.velocity.y < 0 && floor.angularVelocity > 0, "Floor impact rebounds and converts slip to spin");
        Fruit a{1, {100, 100}, {10, 8}, 2, 0.2f, 1}, b{1, {125, 100}, {-10, -8}, 2, 0.8f, -0.5f};
        const auto mergedBody = physics::Combine(a, b);
        Require(physics::Mass(mergedBody) == physics::Mass(a) + physics::Mass(b), "Merged body preserves mass");
        Require(std::isfinite(mergedBody.angularVelocity) && mergedBody.impact == 1, "Merge carries rotation and pulse feedback");
        FruitGame game(42);
        Require(game.State() == Status::Ready && !game.Drop(), "Ready rejects drops");
        game.Start(); game.SetAim(-100);
        Require(game.Aim() == rules::radii[game.CurrentLevel()], "Aim clamped to jar");
        game.SetAim(rules::width / 2); Require(game.Drop(), "First drop accepted");
        Require(!game.Drop(), "Rapid second drop rejected");
        const auto spawn = game.Fruits().front().position;
        game.TogglePause(); Advance(game, 1);
        Require(game.Fruits().front().position.y == spawn.y && !game.Drop(), "Pause freezes simulation and drops");
        game.TogglePause(); game.Update(-1); game.Update(std::numeric_limits<float>::quiet_NaN());
        Require(game.Fruits().front().position.y == spawn.y, "Invalid time ignored");
        Advance(game, 5);
        const auto resting = game.Fruits().front();
        Require(std::abs(resting.position.y + rules::radii[resting.level] - rules::height) < 0.1f,
                "Fruit settles on floor");
        Require(game.State() == Status::Playing, "New fruit does not trigger overflow during descent");
        bool merged = false;
        for (unsigned seed = 0; seed < 100 && !merged; ++seed) {
            FruitGame pair(seed);
            if (pair.CurrentLevel() != pair.NextLevel()) continue;
            const int level = pair.CurrentLevel();
            pair.Start(); pair.Drop(); Advance(pair, 2); pair.Drop(); Advance(pair, 2);
            Require(pair.Fruits().size() == 1 && pair.Fruits()[0].level == level + 1, "Equal fruit contact merges exactly once");
            Require(pair.Score() == (level + 2) * 10, "Merged level awards points"); merged = true;
        }
        Require(merged, "Deterministic merge fixture found");
        FruitGame lowFps(18), highFps(18);
        lowFps.Start(); highFps.Start(); lowFps.Drop(); highFps.Drop();
        Advance(lowFps, 2, 1.0f / 60); Advance(highFps, 2, 1.0f / 240);
        Require(std::abs(lowFps.Fruits()[0].position.y - highFps.Fruits()[0].position.y) < 0.1f,
                "Physics independent of display frame rate");
        FruitGame pile(7); pile.Start();
        for (int drop = 0; drop < 500 && pile.State() == Status::Playing; ++drop) {
            pile.SetAim(rules::width / 2); pile.Drop(); Advance(pile, 1);
            for (const auto& fruit : pile.Fruits()) {
                Require(std::isfinite(fruit.position.x) && std::isfinite(fruit.position.y), "Stack positions finite");
                const float radius = rules::radii[fruit.level];
                Require(fruit.position.x >= radius - 0.1f && fruit.position.x <= rules::width - radius + 0.1f &&
                    fruit.position.y <= rules::height - radius + 0.1f, "Fruit stays within sides and floor");
            }
        }
        Require(pile.State() == Status::GameOver || pile.State() == Status::Won, "Stack eventually ends game");
        pile.Reset(); Require(pile.State() == Status::Ready && pile.Score() == 0 && pile.Fruits().empty(), "Reset clears game");
        const auto path = std::filesystem::path(argc > 1 ? argv[1] : "fruit-test-data") / "records.ini";
        app::HighScores scores; scores.fruit = 120; scores.snake = 80;
        Require(app::SaveHighScores(scores, path), "Fruit record saved");
        app::HighScores loaded; Require(app::LoadHighScores(loaded, path) && loaded.fruit == 120 && loaded.snake == 80, "Fruit record reload preserves old games");
        { std::ofstream old(path); old << "snake=30\n"; }
        Require(app::LoadHighScores(loaded, path) && loaded.fruit == 0 && loaded.snake == 30, "Legacy record defaults fruit score");
        std::cout << "Fruit game tests passed\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
