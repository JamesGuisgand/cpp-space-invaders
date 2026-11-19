// Interactive first milestone. Each line advances the simulation by 0.1 s.
#include "Model.hpp"
#include <iostream>
#include <string>
int main() {
    using namespace inv;
    Player player;
    Enemy enemy{{302,110,32,24}};
    std::vector<Projectile> shots;
    std::cout << "q/d + Enter: move; space + Enter: fire; Enter: advance\n";
    for (std::string line; std::getline(std::cin,line);) {
        for (int i=0;i<12;++i) {
            auto shot = player.update({int(line=="d")-int(line=="q"),0,line==" "},1.f/120);
            if (shot) shots.push_back(*shot);
            enemy.move(.01f,0);
            if (enemy.body.x > width) enemy.body.x = 0;
            for (auto& p : shots) {
                p.move(1.f/120);
                if (CollisionSystem::overlaps(enemy.body,p.body)) {
                    std::cout << "HIT\n";
                    return 0;
                }
            }
            std::erase_if(shots,[](const Projectile& p){ return p.body.y+p.body.h < 0; });
        }
        std::cout << "Ship x=" << player.bounds().x << " target x=" << enemy.body.x;
        if (!shots.empty()) std::cout << " shot y=" << shots.back().body.y;
        std::cout << '\n';
    }
}
