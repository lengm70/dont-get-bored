#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include "games/bomber/BomberGame.h"
#include "games/bomber/BomberPowerUps.h"
#include "config/HighScores.h"
namespace {
using namespace games::bomber;
void Require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
bool Has(const std::vector<Cell>& cells, Cell cell) { return std::find(cells.begin(),cells.end(),cell) != cells.end(); }
Arena OpenArena() {
    Arena map;
    for (int y = 0; y < rules::rows; ++y) for (int x = 0; x < rules::columns; ++x)
        map.tiles[Arena::Index({x,y})] = x == 0 || y == 0 || x == rules::columns - 1 || y == rules::rows - 1 ? Tile::Wall : Tile::Floor;
    return map;
}
void Advance(BomberGame& game, float duration, std::array<Input,2> input = {}) {
    while (duration > 0) {
        const float step = std::min(duration,1.0f/120);
        game.Update(step,input);
        for (auto& player : input) player.plant = false;
        duration -= step;
    }
}
void TestMaps() {
    for (unsigned seed = 1; seed <= 80; ++seed) for (int stage = 1; stage <= rules::stages; ++stage) {
        const auto map = GenerateArena(seed,Mode::Solo,stage);
        Require(map.At({-1,0}) == Tile::Wall && map.At({rules::columns,3}) == Tile::Wall,"Out-of-range cells are solid");
        for (int actor = 0; actor < stage + 2; ++actor) {
            const Cell spawn = OccupiedCell(SpawnPosition(actor));
            Require(map.At(spawn) == Tile::Floor,"All actors spawn on free cells");
            for (Cell offset : directions) {
                const Cell next{spawn.x + offset.x,spawn.y + offset.y};
                Require(map.At(next) != Tile::Crate,"Spawn escape routes cannot be crated");
            }
        }
        const auto duel = GenerateArena(seed,Mode::Duel,stage);
        for (int i = 0; i < rules::cellCount; ++i)
            Require(duel.tiles[i] == duel.tiles[rules::cellCount - 1 - i] && duel.hidden[i] == duel.hidden[rules::cellCount - 1 - i],
                    "Duel terrain and equipment are rotationally symmetric");
        Require(std::count(map.tiles.begin(),map.tiles.end(),Tile::Crate) > 0,"Arenas contain destructible content");
    }
}
void TestExplosions() {
    auto map = OpenArena();
    map.tiles[Arena::Index({6,5})] = Tile::Wall; map.tiles[Arena::Index({5,3})] = Tile::Crate;
    std::vector<Bomb> bombs{{{5,5},0,4,0,0}};
    auto blast = ResolveExplosions(map,bombs,{0});
    Require(Has(blast.flames,{5,3}) && !Has(blast.flames,{5,2}) && !Has(blast.flames,{6,5}) && !Has(blast.flames,{7,5}),
            "Crates stop rays after being hit; walls block without burning");
    map = OpenArena(); bombs = {{{3,5},0,3,0.2f,0},{{5,5},1,2,2,0},{{7,5},0,3,2,0}};
    blast = ResolveExplosions(map,bombs,{0});
    Require(blast.bombs.size() == 3 && Has(blast.flames,{10,5}),"Bombs propagate chains immediately");
    const auto danger = PredictDanger(map,bombs,{});
    Require(std::abs(danger[Arena::Index({10,5})] - 0.2f) < 0.001f,"AI danger prediction propagates chain fuse times");
    map.tiles[Arena::Index({5,5})] = Tile::Crate;
    bombs = {{{3,5},0,5,0,0},{{5,3},1,5,0,0}};
    blast = ResolveExplosions(map,bombs,{0,1});
    Require(blast.crates.size() == 1 && !Has(blast.flames,{6,5}) && !Has(blast.flames,{5,6}),
            "Simultaneous explosions use one terrain snapshot and score crates once");
    const auto reversed = ResolveExplosions(map,bombs,{1,0});
    Require(blast.flames.size() == reversed.flames.size() && std::all_of(blast.flames.begin(),blast.flames.end(),[&](Cell c){ return Has(reversed.flames,c); }),
            "Trigger order does not alter the blast footprint");
}
void TestMatch() {
    Match solo;
    for (int stage = 1; stage <= rules::stages; ++stage) {
        solo.Start(); solo.AddScore(20); solo.Finish(Outcome::SoloVictory);
        const int score = solo.Score(); solo.Finish(Outcome::SoloVictory);
        Require(solo.Score() == score,"Completed round cannot award its bonus twice");
        if (stage < rules::stages) Require(solo.State() == Status::StageComplete && solo.Advance() && solo.Stage() == stage + 1,"Solo campaign advances through sectors");
    }
    Require(solo.State() == Status::Won && !solo.Advance(),"Final sector ends the campaign");
    solo.Reset({});
    for (int life = rules::initialLives; life > 0; --life) {
        solo.Start(); solo.Finish(Outcome::SoloDefeat);
        Require(solo.Lives() == life - 1,"A defeat removes exactly one life");
        if (life > 1) Require(solo.Advance() && solo.Stage() == 1,"Retry keeps the same sector");
    }
    Require(solo.State() == Status::GameOver,"No lives ends the run");
    Match duel({Mode::Duel,2}); duel.Start(); duel.Finish(Outcome::Draw);
    Require(duel.Wins()[0] == 0 && duel.Wins()[1] == 0 && duel.Advance(),"Draws do not award a win");
    for (int round = 0; round < 2; ++round) {
        duel.Start(); duel.Finish(Outcome::PlayerTwo);
        if (round == 0) Require(duel.State() == Status::RoundOver && duel.Advance(),"Duel continues before target wins");
    }
    Require(duel.State() == Status::MatchOver && duel.Winner() == 1 && duel.Wins()[1] == 2,"Target wins decides match winner");
    Match clamp({Mode::Duel,999}); Require(clamp.Settings().targetWins == rules::maxTargetWins,"Round target is bounded");
}
void TestMovementAndRoundResolution() {
    BomberGame game(42); game.Reset({Mode::Duel,2});
    game.Update(0.2f,{{{Direction::Right,true},{}}});
    Require(game.Bombs().empty() && game.Actors()[0].position.x == 1.5f,"Ready input cannot move or plant");
    game.Start(); game.Update(1.0f/120,{{{Direction::None,true},{}}});
    Require(game.Bombs().size() == 1,"Plant creates one bomb");
    game.TogglePause(); const float fuse = game.Bombs()[0].remaining; const Point paused = game.Actors()[0].position;
    game.Update(0.2f,{{{Direction::Right,true},{}}});
    Require(game.Bombs()[0].remaining == fuse && game.Actors()[0].position.x == paused.x,"Pause freezes movement and fuses");
    game.TogglePause();
    Advance(game,2.0f/rules::playerSpeed,{{{Direction::Right,false},{}}});
    Advance(game,1.0f/rules::playerSpeed,{{{Direction::Down,false},{}}});
    Require(std::abs(game.Actors()[0].position.x - 3.5f) < 0.03f && std::abs(game.Actors()[0].position.y - 2.5f) < 0.03f,"Turns navigate the cleared spawn corridor");
    Advance(game,3);
    Require(game.Actors()[0].alive && game.State() == Status::Playing,"Every fresh spawn has time and space to escape its own bomb");
    game.Reset({Mode::Duel,2}); game.Start(); game.Update(1.0f/120,{{{Direction::None,true},{}}});
    Advance(game,0.4f,{{{Direction::Right,false},{}}}); Advance(game,0.4f,{{{Direction::Left,false},{}}});
    Require(game.Actors()[0].position.x >= 2 + rules::actorRadius - 0.01f,"After leaving a bomb the owner cannot walk through it again");
    game.Reset({Mode::Duel,2}); game.Start(); game.Update(1.0f/120,{{{Direction::None,true},{Direction::None,true}}});
    Advance(game,4);
    Require(game.State() == Status::RoundOver && game.Progress().Winner() == -1 && !game.Actors()[0].alive && !game.Actors()[1].alive,
            "Simultaneous self-destructs resolve as a draw");
    game.Start(); Require(game.Progress().Round() == 2 && game.Bombs().empty() && game.Flames().empty(),"Next round clears transient hazards");
    game.Reset({Mode::Duel,2});
    for (int round = 1; round <= 2; ++round) {
        game.Start(); game.Update(1.0f/120,{{{Direction::None,true},{}}}); Advance(game,4);
        Require(game.Progress().Wins()[1] == round && game.Progress().Winner() == 1,"Survivor receives the round win");
    }
    Require(game.State() == Status::MatchOver,"Duel game integration honors configured target wins");
    BomberGame slow(9), fast(9); slow.Reset({Mode::Duel,3}); fast.Reset({Mode::Duel,3}); slow.Start(); fast.Start();
    for (int f = 0; f < 30; ++f) slow.Update(1.0f/30,{{{Direction::Right,false},{}}});
    for (int f = 0; f < 240; ++f) fast.Update(1.0f/240,{{{Direction::Right,false},{}}});
    Require(std::abs(slow.Actors()[0].position.x - fast.Actors()[0].position.x) < 0.02f,"Movement stays consistent across FPS caps");
    slow.Update(std::numeric_limits<float>::quiet_NaN()); Require(std::isfinite(slow.Actors()[0].position.x),"Invalid frame times are ignored");
}
void TestAIAndUpgrades() {
    auto map = OpenArena(); std::vector<Bomb> bombs{{{3,5},0,4,2,0}};
    Actor enemy; enemy.human = false; enemy.position = {5.5f,5.5f}; enemy.speed = 2;
    std::uint32_t random = 5;
    const Direction escape = ChooseEnemyDirection(map,bombs,{},enemy,{8.5f,5.5f},random);
    const Cell offset = Offset(escape), cell{5 + offset.x,5 + offset.y};
    const auto danger = PredictDanger(map,bombs,{});
    Require(escape != Direction::None && !std::isfinite(danger[Arena::Index(cell)]),"Drone escapes bomb lanes instead of chasing into danger");
    map.tiles.fill(Tile::Wall); map.tiles[Arena::Index({5,5})] = Tile::Floor; map.tiles[Arena::Index({6,5})] = Tile::Floor;
    const Direction blocked = ChooseEnemyDirection(map,{},{{{6,5},0.5f}},enemy,{7.5f,5.5f},random);
    Require(blocked == Direction::None,"Drone never walks into active fire");
    Actor human;
    for (int i = 0; i < 20; ++i) for (Upgrade item : {Upgrade::Capacity,Upgrade::Range,Upgrade::Speed}) ApplyUpgrade(human,item);
    Require(human.capacity == rules::maxCapacity && human.range == rules::maxRange && std::abs(human.speed - rules::maxPlayerSpeed) < 0.001f,
            "Equipment upgrades respect gameplay caps");
    ApplyUpgrade(enemy,Upgrade::Range); Require(enemy.range == rules::initialRange,"Drones do not steal player equipment");
    for (int seed = 1; seed <= 10; ++seed) {
        BomberGame game(seed); game.Start();
        for (int frame = 0; frame < 1200 && game.State() == Status::Playing; ++frame) {
            const Direction move = static_cast<Direction>(NextRandom(random) % 5);
            game.Update(1.0f/120,{{{move,NextRandom(random) % 180 == 0},{}}});
            for (const auto& actor : game.Actors()) {
                Require(std::isfinite(actor.position.x) && std::isfinite(actor.position.y),"Actor coordinates remain finite");
                for (int y = 0; y < rules::rows; ++y) for (int x = 0; x < rules::columns; ++x)
                    Require(game.Map().At({x,y}) == Tile::Floor || !TouchesCell(actor.position,{x,y}),"Movement never penetrates walls or crates");
            }
        }
    }
}
void TestRecords(const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory); const auto path = directory / "records.ini";
    { std::ofstream old(path); old << "snake=120\nbreakout=500\n"; }
    app::HighScores scores; Require(app::LoadHighScores(scores,path) && scores.bomber == 0,"Existing records migrate without loss");
    scores.bomber = 1800;
    Require(app::SaveHighScores(scores,path),"Bomber record saves");
    app::HighScores loaded;
    Require(app::LoadHighScores(loaded,path) && loaded.bomber == 1800 && loaded.snake == 120 && loaded.breakout == 500,"New record survives reload alongside existing games");
    scores.bomber = -1; Require(!app::SaveHighScores(scores,path),"Negative record is rejected"); std::filesystem::remove(path);
}
}
int main(int argc, char** argv) {
    try {
        TestMaps(); TestExplosions(); TestMatch(); TestMovementAndRoundResolution(); TestAIAndUpgrades();
        TestRecords(argc > 1 ? argv[1] : "bomber-test-data");
        std::cout << "Bomber tests passed\n"; return 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
