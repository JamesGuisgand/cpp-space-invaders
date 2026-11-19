#pragma once
#include "Model.hpp"
#include <random>

namespace inv {
class WaveManager {
    int number_{};
    float direction_{1}, shotTimer_{};
    std::vector<Enemy> enemies_;
    std::mt19937 random_;
public:
    explicit WaveManager(unsigned seed) : random_(seed) { next(); }
    void next();
    std::optional<Projectile> update(float dt);
    int number() const { return number_; }
    float speed() const { return 35.f + 8.f*std::min(number_-1,25); }
    float interval() const { return std::max(.28f, 1.25f-.07f*(number_-1)); }
    const auto& enemies() const { return enemies_; }
    bool hit(Rect shot);
    bool empty() const { return enemies_.empty(); }
};
class World {
    Player player_;
    WaveManager waves_;
    std::vector<Projectile> shots_;
    int lives_{3}, score_{};
    float shield_{};
public:
    explicit World(unsigned seed) : waves_(seed) { shots_.reserve(64); }
    void step(Input input, float dt);
    const auto& player() const { return player_; }
    const auto& waves() const { return waves_; }
    const auto& shots() const { return shots_; }
    int lives() const { return lives_; }
    int score() const { return score_; }
    bool shielded() const { return shield_ > 0; }
};
}
