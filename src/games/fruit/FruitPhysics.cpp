#include "games/fruit/FruitPhysics.h"
#include <algorithm>
#include <cmath>
namespace games::fruit::physics {
namespace {
float Restitution(float speed) { return speed > rules::bounceThreshold ? rules::restitution : 0; }
void Hit(Fruit& fruit, float speed) { fruit.impact = std::max(fruit.impact, std::min(1.0f, speed / 400)); }
void Boundary(Fruit& fruit, Point normal, bool floor) {
    const float radius = rules::radii[fruit.level], mass = Mass(fruit), inertia = Inertia(fruit);
    const float normalSpeed = fruit.velocity.x * normal.x + fruit.velocity.y * normal.y;
    const float impulse = normalSpeed < 0 ? -(1 + Restitution(-normalSpeed)) * normalSpeed * mass : 0;
    fruit.velocity.x += impulse * normal.x / mass; fruit.velocity.y += impulse * normal.y / mass;
    const Point tangent{-normal.y, normal.x};
    const float slip = fruit.velocity.x * tangent.x + fruit.velocity.y * tangent.y - fruit.angularVelocity * radius;
    const float limit = rules::friction * (impulse + (floor ? mass * rules::gravity * rules::timeStep : 0));
    const float friction = std::clamp(-slip / (1 / mass + radius * radius / inertia), -limit, limit);
    fruit.velocity.x += friction * tangent.x / mass; fruit.velocity.y += friction * tangent.y / mass;
    fruit.angularVelocity -= friction * radius / inertia;
    Hit(fruit, -normalSpeed);
}
}
float Mass(const Fruit& fruit) { return static_cast<float>(1 << fruit.level); }
float Inertia(const Fruit& fruit) { const float radius = rules::radii[fruit.level]; return 0.5f * Mass(fruit) * radius * radius; }
void Constrain(Fruit& fruit, bool supportFriction) {
    const float radius = rules::radii[fruit.level];
    if (fruit.position.x <= radius) { fruit.position.x = radius; Boundary(fruit, {1, 0}, false); }
    if (fruit.position.x >= rules::width - radius) { fruit.position.x = rules::width - radius; Boundary(fruit, {-1, 0}, false); }
    if (fruit.position.y >= rules::height - radius) {
        fruit.position.y = rules::height - radius; Boundary(fruit, {0, -1}, supportFriction);
    }
}
void Resolve(Fruit& a, Fruit& b) {
    const float dx = b.position.x - a.position.x, dy = b.position.y - a.position.y;
    const float ra = rules::radii[a.level], rb = rules::radii[b.level];
    const float distanceSquared = dx * dx + dy * dy;
    if (distanceSquared >= (ra + rb) * (ra + rb)) return;
    const float distance = std::sqrt(distanceSquared);
    const Point normal{distance > 0.001f ? dx / distance : 1, distance > 0.001f ? dy / distance : 0};
    const float inverseA = 1 / Mass(a), inverseB = 1 / Mass(b), sum = inverseA + inverseB;
    const float overlap = ra + rb - distance;
    a.position.x -= normal.x * overlap * inverseA / sum; a.position.y -= normal.y * overlap * inverseA / sum;
    b.position.x += normal.x * overlap * inverseB / sum; b.position.y += normal.y * overlap * inverseB / sum;
    const float speed = (b.velocity.x - a.velocity.x) * normal.x + (b.velocity.y - a.velocity.y) * normal.y;
    if (speed >= 0) return;
    const float impulse = -(1 + Restitution(-speed)) * speed / sum;
    a.velocity.x -= impulse * inverseA * normal.x; a.velocity.y -= impulse * inverseA * normal.y;
    b.velocity.x += impulse * inverseB * normal.x; b.velocity.y += impulse * inverseB * normal.y;
    const Point tangent{-normal.y, normal.x};
    const float slip = (b.velocity.x - a.velocity.x) * tangent.x + (b.velocity.y - a.velocity.y) * tangent.y -
        a.angularVelocity * ra - b.angularVelocity * rb;
    const float angularA = 1 / Inertia(a), angularB = 1 / Inertia(b);
    const float friction = std::clamp(-slip / (sum + ra * ra * angularA + rb * rb * angularB),
        -rules::friction * impulse, rules::friction * impulse);
    a.velocity.x -= friction * inverseA * tangent.x; a.velocity.y -= friction * inverseA * tangent.y;
    b.velocity.x += friction * inverseB * tangent.x; b.velocity.y += friction * inverseB * tangent.y;
    a.angularVelocity -= friction * ra * angularA; b.angularVelocity -= friction * rb * angularB;
    Hit(a, -speed); Hit(b, -speed);
}
Fruit Combine(const Fruit& a, const Fruit& b) {
    Fruit result{a.level + 1, {(a.position.x + b.position.x) / 2, (a.position.y + b.position.y) / 2},
        {(a.velocity.x + b.velocity.x) / 2, (a.velocity.y + b.velocity.y) / 2}, std::min(a.age, b.age)};
    const auto orbital = [&](const Fruit& fruit) {
        return Mass(fruit) * ((fruit.position.x - result.position.x) * (fruit.velocity.y - result.velocity.y) -
                             (fruit.position.y - result.position.y) * (fruit.velocity.x - result.velocity.x));
    };
    result.angle = a.angle;
    result.angularVelocity = std::clamp((Inertia(a) * a.angularVelocity + Inertia(b) * b.angularVelocity +
        orbital(a) + orbital(b)) / Inertia(result), -rules::maxSpin, rules::maxSpin);
    result.impact = 1;
    Constrain(result, false); return result;
}
}  // namespace games::fruit::physics
