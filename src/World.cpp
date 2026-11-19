#include "World.hpp"
#include <cstdint>
namespace inv {
void WaveManager::next() {
    ++number_;
    enemies_.clear();
    direction_ = 1;
    shotTimer_ = interval();
    const int rows = 2 + std::min(number_-1, 3);
    enemies_.reserve(8*rows);
    for (int row=0; row<rows; ++row)
        for (int col=0; col<8; ++col)
            enemies_.push_back({{64.f+col*60,100.f+row*40,30,22}});
}
bool WaveManager::hit(Rect shot) {
    auto it = std::ranges::find_if(enemies_, [&](const Enemy& e){
        return CollisionSystem::overlaps(e.body,shot);
    });
    if (it == enemies_.end()) return false;
    *it = enemies_.back();
    enemies_.pop_back();
    return true;
}
std::optional<Projectile> WaveManager::update(float dt) {
    if (empty()) return {};
    float dx = direction_*speed()*dt;
    const bool turn = std::ranges::any_of(enemies_, [dx](const Enemy& e){
        return e.body.x+dx < 8 || e.body.x+e.body.w+dx > width-8;
    });
    if (turn) { direction_ *= -1; dx = 0; }
    for (auto& e : enemies_) e.move(dx, turn ? 14.f : 0.f);
    shotTimer_ -= dt;
    if (shotTimer_ > 0) return {};
    shotTimer_ = interval();
    const auto index = (static_cast<std::uint64_t>(random_()) * enemies_.size()) >> 32;
    auto b = enemies_[index].body;
    return Projectile{{b.x+13,b.y+b.h,4,10},Side::enemy};
}
void World::step(Input input, float dt) {
    if (!lives_) return;
    shield_ -= dt;
    if (auto p = player_.update(input,dt)) shots_.push_back(*p);
    if (auto p = waves_.update(dt)) shots_.push_back(*p);
    std::erase_if(shots_, [&](Projectile& p){
        p.move(dt);
        if (p.side == Side::player && waves_.hit(p.body)) { score_ += 100; return true; }
        if (p.side == Side::enemy && CollisionSystem::overlaps(p.body,player_.bounds())) {
            if (shield_ <= 0) { --lives_; shield_ = 1.1f; }
            return true;
        }
        return p.body.y+p.body.h < 0 || p.body.y > height;
    });
    for (const auto& e : waves_.enemies())
        if (e.body.y+e.body.h >= height || CollisionSystem::overlaps(e.body,player_.bounds()))
            lives_ = 0;
    if (lives_ && waves_.empty()) { shots_.clear(); waves_.next(); }
}
}
