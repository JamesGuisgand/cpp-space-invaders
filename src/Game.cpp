#include "Game.hpp"
#include <chrono>
namespace inv {
using Key = sf::Keyboard::Key;
namespace {
const sf::Color mint{105,250,196}, pink{255,114,163}, ink{12,18,37}, white{225,234,248};
}
Game::Game(const std::filesystem::path& font)
    : window_(sf::VideoMode({640,720}),"Space Invaders",sf::Style::Titlebar | sf::Style::Close),
      font_(font) {
    window_.setFramerateLimit(60);
    window_.setKeyRepeatEnabled(false);
}
void Game::save() { saveError_ = !records_.record(world_.score()); }
void Game::start() {
    world_ = World(std::random_device{}());
    screen_ = Screen::playing;
}
void Game::key(Key k) {
    if (screen_ == Screen::playing) {
        if (k == Key::Escape) { save(); screen_ = Screen::menu; choice_ = 0; }
        return;
    }
    if (screen_ == Screen::over) {
        if (k == Key::Enter) start();
        if (k == Key::Escape) { screen_ = Screen::menu; choice_ = 0; }
        return;
    }
    if (screen_ == Screen::options) {
        if (k == Key::Enter) stars_ = !stars_;
        if (k == Key::Escape) screen_ = Screen::menu;
        return;
    }
    if (k == Key::Up || k == Key::Z) choice_ = (choice_+2)%3;
    if (k == Key::Down || k == Key::S) choice_ = (choice_+1)%3;
    if (k == Key::Escape || (k == Key::Enter && choice_ == 2)) window_.close();
    else if (k == Key::Enter && choice_ == 0) start();
    else if (k == Key::Enter) screen_ = Screen::options;
}
Input Game::input() const {
    auto held = [](Key k){ return sf::Keyboard::isKeyPressed(k); };
    return {(held(Key::Right)||held(Key::D))-(held(Key::Left)||held(Key::Q)),
            (held(Key::Down)||held(Key::S))-(held(Key::Up)||held(Key::Z)), held(Key::Space)};
}
void Game::box(Rect b, sf::Color color) {
    tile_.setPosition({b.x,b.y});
    tile_.setSize({b.w,b.h});
    tile_.setFillColor(color);
    window_.draw(tile_);
}
void Game::label(const std::string& s,float x,float y,unsigned size,sf::Color color) {
    sf::Text t(font_,s,size);
    t.setPosition({x,y});
    t.setFillColor(color);
    window_.draw(t);
}
void Game::draw() {
    window_.clear(ink);
    if (stars_) for (int i=0;i<64;++i)
        box({float((i*173+41)%640),float((i*i*7+79*i)%720),2,2},{56,74,101});
    if (screen_ == Screen::playing) {
        auto b = world_.player().bounds();
        const auto color = world_.shielded() ? white : mint;
        box({b.x,b.y+12,b.w,8},color);
        box({b.x+8,b.y+7,16,8},color);
        box({b.x+13,b.y,6,8},color);
        for (const auto& e : world_.waves().enemies()) {
            auto r = e.body;
            box({r.x+5,r.y,20,5},pink);
            box({r.x,r.y+5,30,12},pink);
            box({r.x+4,r.y+17,5,5},pink);
            box({r.x+21,r.y+17,5,5},pink);
            box({r.x+7,r.y+8,4,4},ink);
            box({r.x+19,r.y+8,4,4},ink);
        }
        for (const auto& p : world_.shots()) box(p.body,p.side == Side::player ? mint : pink);
        label("SCORE "+std::to_string(world_.score()),20,15,19,mint);
        label("VAGUE "+std::to_string(world_.waves().number()),265,15,19,white);
        label("VIES "+std::to_string(world_.lives()),505,15,19,white);
    } else {
        label(screen_ == Screen::menu ? "SPACE INVADERS" : screen_ == Screen::over ? "GAME OVER" : "OPTIONS",60,145,45,mint);
        if (screen_ == Screen::menu) {
            const char* items[]{"Jouer","Options","Quitter"};
            for (int i=0;i<3;++i)
                label(std::string(i==choice_ ? "> " : "  ")+items[i],70,270.f+48*i,26,i==choice_ ? mint : white);
            label("Fleches / Entree",70,478,19,white);
        } else if (screen_ == Screen::options) {
            label(stars_ ? "Etoiles : oui" : "Etoiles : non",70,280,26,white);
            label("Entree : changer    Echap : retour",70,340,18,mint);
            label("ZQSD / fleches : bouger    Espace : tirer",70,430,18,white);
        } else {
            label("Score "+std::to_string(world_.score()),70,275,30,white);
            label("Entree : rejouer    Echap : menu",70,360,20,mint);
        }
        label("Record "+std::to_string(records_.best()),70,590,21,white);
        if (saveError_) label("Echec de sauvegarde",70,635,18,pink);
    }
    window_.display();
}
void Game::run(bool smoke) {
    using Clock = std::chrono::steady_clock;
    auto last = Clock::now();
    constexpr double step = 1./120;
    double pending = 0;
    if (smoke) start();
    int frames = 0;
    while (window_.isOpen()) {
        while (auto event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { if (screen_ == Screen::playing) save(); window_.close(); break; }
            if (auto pressed = event->getIf<sf::Event::KeyPressed>()) key(pressed->code);
            if (!window_.isOpen()) break;
        }
        if (!window_.isOpen()) break;
        auto now = Clock::now();
        const double elapsed = std::chrono::duration<double>(now-last).count();
        last = now;
        if (screen_ == Screen::playing && (smoke || window_.hasFocus())) {
            pending += std::min(.1,elapsed);
            const Input controls = smoke ? Input{1,0,true} : input();
            while (pending >= step && screen_ == Screen::playing) {
                world_.step(controls,static_cast<float>(step));
                pending -= step;
                if (!world_.lives()) { save(); screen_ = Screen::over; }
            }
        } else pending = 0;
        draw();
        if (smoke && ++frames == 12) window_.close();
    }
}
}
