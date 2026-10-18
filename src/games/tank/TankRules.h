#pragma once
namespace games::tank::rules {
inline constexpr int columns = 20, rows = 12, count = columns * rows, waves = 3;
inline constexpr float cell = 30, width = 600, height = 360, radius = 10, playerSpeed = 112,
                       enemySpeed = 58;
inline constexpr float bulletSpeed = 265, playerReload = .32f, enemyReload = 1.5f, step = 1.f / 240,
                       maxFrame = .25f;
inline constexpr int playerHealth = 3, baseHealth = 5, enemyPoints = 100, wavePoints = 300;
} // namespace games::tank::rules
