#pragma once
#include "games/fruit/FruitGame.h"
namespace games::fruit::physics {
float Mass(const Fruit& fruit);
float Inertia(const Fruit& fruit);
void Constrain(Fruit& fruit, bool supportFriction = true);
void Resolve(Fruit& a, Fruit& b);
Fruit Combine(const Fruit& a, const Fruit& b);
}  // namespace games::fruit::physics
