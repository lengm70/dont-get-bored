#include "config/ProgressRecords.h"
#include "games/tank/TankGame.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
void Check(bool b, const char *m) {
    if (!b)
        throw std::runtime_error(m);
}
int main(int argc, char **argv) {
    try {
        using namespace games::tank;
        TankGame g;
        g.Update(.1f, {{{1, true}, {}}});
        Check(g.Bullets().empty() && g.Players()[0].x == 255, "Ready must freeze controls");
        g.Start();
        g.Update(.1f, {{{1, true}, {}}});
        Check(g.Players()[0].x > 255 && !g.Bullets().empty(), "Movement and fire");
        g.TogglePause();
        float x = g.Players()[0].x;
        g.Update(.2f, {{{1, true}, {}}});
        Check(g.Players()[0].x == x, "Pause");
        g.Reset(true);
        Check(g.Players()[1].health == 3, "Co-op second player");
        g.Start();
        for (int i = 0; i < 15000 && g.State() == Status::Playing; ++i) {
            g.Update(1.f / 120, {{{i / 200 % 4, true}, {(i / 250 + 2) % 4, true}}});
            for (auto &p : g.Players())
                Check(std::isfinite(p.x) && std::isfinite(p.y), "Finite coordinates");
        }
        TankGame a, b;
        a.Start();
        b.Start();
        for (int i = 0; i < 30; ++i)
            a.Update(1.f / 30, {{{1, false}, {}}});
        for (int i = 0; i < 240; ++i)
            b.Update(1.f / 240, {{{1, false}, {}}});
        Check(std::abs(a.Players()[0].x - b.Players()[0].x) < .1f, "FPS consistency");
        app::ProgressRecords records;
        Check(records.Best("tank.best", 100) && !records.Best("tank.best", 50), "Best direction");
        Check(records.Best("moves.1", 30, true) && records.Best("moves.1", 20, true),
              "Lower is better");
        Check(!records.Best("bad\nkey", 30), "Key validation");
        auto path = argc > 1 ? argv[1] : "tank-test.ini";
        Check(records.Save(path), "Save");
        app::ProgressRecords restored;
        Check(restored.Load(path) && restored.Get("tank.best") == 100 &&
                  restored.Get("moves.1") == 20,
              "Record roundtrip");
        std::filesystem::remove(path);
        std::cout << "Tank and progress tests passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
