#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

namespace inv {
constexpr float width = 640, height = 720;
struct Rect { float x, y, w, h; };
struct CollisionSystem {
    static constexpr bool overlaps(Rect a, Rect b) {
        return a.x < b.x+b.w && b.x < a.x+a.w && a.y < b.y+b.h && b.y < a.y+a.h;
    }
};
enum class Side { player, enemy };
struct Projectile {
    Rect body;
    Side side;
    void move(float dt) { body.y += (side == Side::player ? -490.f : 230.f) * dt; }
};
struct Input { int x{}, y{}; bool fire{}; };
class Player {
    Rect body_{304,650,32,20};
    float cooldown_{};
public:
    Rect bounds() const { return body_; }
    std::optional<Projectile> update(Input input, float dt) {
        const float distance = 290.f * dt / (input.x && input.y ? std::sqrt(2.f) : 1.f);
        body_.x = std::clamp(body_.x + input.x*distance, 0.f, width-body_.w);
        body_.y = std::clamp(body_.y + input.y*distance, height*.6f, height-body_.h);
        cooldown_ -= dt;
        if (input.fire && cooldown_ <= 0) {
            cooldown_ = .19f;
            return Projectile{{body_.x+14,body_.y-10,4,10},Side::player};
        }
        return {};
    }
};
struct Enemy {
    Rect body;
    void move(float dx, float dy) { body.x += dx; body.y += dy; }
};
}
