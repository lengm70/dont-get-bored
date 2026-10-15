#include "games/breakout/BreakoutLevels.h"
#include <algorithm>
#include <cmath>
namespace games::breakout {
BrickArray MakeLevel(int level) {
    BrickArray bricks{};
    level = std::clamp(level, 0, rules::levelCount - 1);
    for (int i = 0; i < rules::brickCount; ++i) {
        const int row = i / rules::brickColumns, col = i % rules::brickColumns;
        auto& brick = bricks[i];
        brick.health = 1;
        if (level >= 1 && row < 2) brick = {BrickKind::Armored, level == 4 ? 3 : 2};
        if (level >= 2 && row % 2 == 1 && (col == 2 || col == 7)) brick = {BrickKind::Explosive, 1};
        if (level >= 3 && row == 2) brick = {BrickKind::Moving, 1};
        if (level == 4 && row == 3 && (col == 3 || col == 6)) brick = {BrickKind::Obstacle, 1};
    }
    return bricks;
}
Bounds BaseBrickBounds(int index) {
    return {rules::brickMargin + (index % rules::brickColumns) * (rules::brickWidth + rules::brickGap),
            rules::brickTop + (index / rules::brickColumns) * (rules::brickHeight + rules::brickGap),
            rules::brickWidth, rules::brickHeight};
}
Bounds CurrentBrickBounds(const BrickArray& bricks, int index, float seconds) {
    Bounds bounds = BaseBrickBounds(index);
    if (bricks[index].kind == BrickKind::Moving)
        bounds.x += rules::movingAmplitude * std::sin(seconds * rules::movingFrequency);
    return bounds;
}
float BrickVelocity(const BrickArray& bricks, int index, float seconds) {
    return bricks[index].kind == BrickKind::Moving ?
        rules::movingAmplitude * rules::movingFrequency * std::cos(seconds * rules::movingFrequency) : 0;
}
BrickDamage DamageBricks(BrickArray& bricks, int index) {
    BrickDamage result;
    std::vector<int> pending{index};
    while (!pending.empty()) {
        const int hit = pending.back(); pending.pop_back();
        if (hit < 0 || hit >= rules::brickCount) continue;
        auto& brick = bricks[hit];
        if (!brick.Alive() || !brick.Destructible()) continue;
        result.damaged.push_back(hit);
        if (--brick.health > 0) continue;
        result.destroyed.push_back(hit);
        if (brick.kind != BrickKind::Explosive) continue;
        const int row = hit / rules::brickColumns, col = hit % rules::brickColumns;
        for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
            const int nextRow = row + dy, nextCol = col + dx;
            if ((dx || dy) && nextRow >= 0 && nextRow < rules::brickRows &&
                nextCol >= 0 && nextCol < rules::brickColumns)
                pending.push_back(nextRow * rules::brickColumns + nextCol);
        }
    }
    return result;
}
int RemainingBricks(const BrickArray& bricks) {
    return static_cast<int>(std::count_if(bricks.begin(), bricks.end(),
        [](const Brick& brick) { return brick.Alive() && brick.Destructible(); }));
}
} // namespace games::breakout
