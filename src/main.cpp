#include "Game.hpp"
#include <iostream>
#include <string_view>
int main(int argc,char** argv) {
    try {
        auto assets = std::filesystem::absolute(argv[0]).parent_path()/"assets";
        if (!std::filesystem::exists(assets)) assets = "assets";
        inv::Game game(assets/"DejaVuSansMono.ttf");
        game.run(argc == 2 && std::string_view(argv[1]) == "--smoke");
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
