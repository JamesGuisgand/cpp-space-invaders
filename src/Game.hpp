#pragma once
#include "ScoreManager.hpp"
#include "World.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace inv {
class Game {
    enum class Screen { menu, options, playing, over };
    sf::RenderWindow window_;
    sf::Font font_;
    sf::RectangleShape tile_;
    World world_{0};
    ScoreManager records_{ScoreManager::defaultPath()};
    Screen screen_{Screen::menu};
    int choice_{};
    bool stars_{true}, saveError_{};
    void start();
    void save();
    void key(sf::Keyboard::Key key);
    Input input() const;
    void box(Rect bounds, sf::Color color);
    void label(const std::string& text, float x, float y, unsigned size, sf::Color color);
    void draw();
public:
    explicit Game(const std::filesystem::path& font);
    void run(bool smoke);
};
}
