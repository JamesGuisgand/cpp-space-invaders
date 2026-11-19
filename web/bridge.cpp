#include "World.hpp"
#include <cstdint>
#include <vector>

namespace {
inv::World world{0};
// Six floats per item: x, y, width, height, kind, reserved.
std::vector<float> frame;
void append(inv::Rect bounds, float kind, bool shield = false) {
    frame.insert(frame.end(), {bounds.x, bounds.y, bounds.w, bounds.h, kind, float(shield)});
}
}
extern "C" {
void inv_reset(std::uint32_t seed) { world = inv::World(seed); }
void inv_step(int x, int y, int fire) {
    world.step({std::clamp(x,-1,1), std::clamp(y,-1,1), fire != 0}, 1.f/120);
}
int inv_score() { return world.score(); }
int inv_lives() { return world.lives(); }
int inv_wave() { return world.waves().number(); }
int inv_frame() {
    frame.clear();
    append(world.player().bounds(), 0, world.shielded());
    for (const auto& enemy : world.waves().enemies()) append(enemy.body, 1);
    for (const auto& shot : world.shots()) append(shot.body, shot.side == inv::Side::player ? 2 : 3);
    return static_cast<int>(frame.size()/6);
}
const float* inv_frame_data() { return frame.data(); }
}
