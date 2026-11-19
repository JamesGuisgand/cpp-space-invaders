# cpp-space-invaders

[**Play in your browser →**](https://spaceinvader.guisgand-james.fr/)

A 2D arcade shooter built around a shared **C++20 game simulation**. The desktop edition renders the game with **SFML 3**. The browser edition runs the same C++ code as **WebAssembly** and draws each frame with **HTML Canvas**.

## Play

Move with the **arrow keys** or **ZQSD** and hold **Space** to fire. Each enemy is worth 100 points. You start with three lives. A projectile costs one life and briefly activates a shield. If an enemy touches your ship or reaches the bottom of the screen, the run ends immediately.

| | Browser | Desktop |
|---|---|---|
| Start | Select a visual style and click **Jouer** | Select **Jouer** with the arrow keys and press Enter |
| Escape | Pause or resume. Switch between **Classique** and **Graphique ++** while paused | End the run and return to the main menu |
| High score | Saved in browser storage | Saved to a local `best.txt` file |

Waves continue as long as you survive. The first formation has 16 enemies. Each new wave adds a row of eight enemies until the formation reaches 40. Later waves move faster and fire more frequently, up to fixed limits.

## How the engine works

The simulation advances in fixed steps of **1/120 second**. Each step reads movement and firing input, updates the formation and projectiles, resolves collisions, and checks whether the wave is complete. Rendering happens separately.

| File | Responsibility |
|---|---|
| `src/Model.hpp` | Player movement, firing cooldown, projectiles and rectangle collision checks |
| `src/World.hpp`, `src/World.cpp` | Simulation state, enemy formations, wave progression, lives and scoring |
| `src/Game.cpp` | Native SFML window, keyboard input, menus and rendering |
| `web/bridge.cpp` | Exposes the C++ simulation to JavaScript and provides the positions to draw |
| `web/app.js`, `web/render.js` | Browser input, pause controls and both Canvas rendering styles |
| `tests/core.cpp` | Exercises the simulation without opening a window |

One integration test steers the player and fires until the first wave is cleared. It checks that the game reaches **wave 2 with exactly 1,600 points**. The test runs the same `World` implementation used by both playable editions.

## Build the desktop game

Run the commands below from the repository root. The first CMake configuration downloads **SFML 3.0.1**. The build copies the `assets/` directory beside the executable. Keep them together when moving the game.

### Linux · Ubuntu 24.04

```sh
sudo apt update
sudo apt install -y git cmake g++ libxrandr-dev libxcursor-dev libxi-dev \
  libudev-dev libfreetype-dev libgl1-mesa-dev libegl1-mesa-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/space-invaders
```

### Windows · PowerShell

Install Git, CMake 3.24 or newer, and Visual Studio 2022 with the **Desktop development with C++** workload and a Windows SDK.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\space-invaders.exe
```

## Run the simulation tests

The core uses only the C++ standard library. To build and run its four test suites on Linux without a display or SFML:

```sh
cmake -S . -B build-core -DBUILD_GAME=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

The suites cover collision boundaries, high-score persistence, wave progression and complete gameplay scenarios. The native GitHub Actions workflow builds and tests on Ubuntu and Windows. On Ubuntu it also renders twelve frames under Xvfb.

## Run the browser edition locally

Docker builds the C++ engine with Emscripten and packages the WebAssembly module and web files in an Nginx image.

```sh
docker build -t cpp-space-invaders-web .
docker run --rm -p 127.0.0.1:8000:80 cpp-space-invaders-web
```

Open [localhost:8000](http://localhost:8000).

## Further reading

- [C++ architecture and implementation walkthrough (French)](docs/EXPLICATION_CPP_FR.md)
- [VPS deployment with Docker and Traefik (French)](docs/DEPLOIEMENT_WEB_FR.md)
- [Original turn-by-turn console prototype](examples/mvp.cpp)
- [Bundled font license](assets/FONT-LICENSE.txt)