#include "World.hpp"
#include "ScoreManager.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
using namespace inv;
#define CHECK(expr) do { if (!(expr)) throw std::runtime_error(#expr); } while (false)
void collisions() {
    Rect a{0,0,10,10};
    CHECK(CollisionSystem::overlaps(a,{9,9,2,2}));
    CHECK(CollisionSystem::overlaps(a,{2,2,1,1}));
    CHECK(!CollisionSystem::overlaps(a,{10,0,2,2}));
    CHECK(!CollisionSystem::overlaps(a,{0,14,2,2}));
}
void scores() {
    namespace fs = std::filesystem;
    auto dir = fs::temp_directory_path()/("inv-tests-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(dir);
    struct Cleanup { fs::path path; ~Cleanup(){ std::error_code ec; fs::remove_all(path,ec); } } cleanup{dir};
    auto path = dir/"record";
    ScoreManager s(path);
    CHECK(s.best()==0);
    CHECK(s.record(200) && s.record(100) && s.record(400));
    CHECK(ScoreManager(path).best()==400);
    std::ofstream(path) << "500oops";
    CHECK(ScoreManager(path).best()==0);
    CHECK(!ScoreManager(path/"invalid").record(42));
}
void progression() {
    WaveManager wave(7);
    CHECK(wave.number()==1 && wave.enemies().size()==16);
    auto speed = wave.speed(), interval = wave.interval();
    while (!wave.empty()) CHECK(wave.hit(wave.enemies().front().body));
    wave.next();
    CHECK(wave.number()==2 && wave.enemies().size()==24);
    CHECK(wave.speed()>speed && wave.interval()<interval);
}
void simulation() {
    constexpr float dt = 1.f/120;
    World world(2);
    for (int i=0;i<120*150 && world.lives() && world.waves().number()==1;++i) {
        auto ship = world.player().bounds();
        auto target = world.waves().enemies().front().body;
        float difference = target.x-ship.x;
        world.step({(difference>2)-(difference< -2),1,true},dt);
    }
    CHECK(world.waves().number()==2 && world.score()==1600);
    World idle(7);
    for(int i=0;i<120*600 && idle.lives();++i) idle.step({},dt);
    CHECK(idle.lives()==0);
}
int main(int argc,char** argv) {
    try {
        if(argc!=2) return 2;
        std::string_view suite(argv[1]);
        if(suite=="collisions") collisions();
        else if(suite=="scores") scores();
        else if(suite=="progression") progression();
        else if(suite=="simulation") simulation();
        else return 2;
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
}
