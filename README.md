# cpp-space-invaders

A keyboard arcade game built with C++20 and SFML 3.0.1. Clear eight-column formations, dodge incoming fire and beat your local high score. Enemy ships and projectiles are drawn from rectangles. The bundled DejaVu Sans Mono font has its license in assets/.

## Play

Choose **Jouer** with the arrow keys and Enter. Move with the arrows or ZQSD, hold Space to fire, and press Escape to end the run and return to the menu. Options toggles the star field. Enter restarts after game over.

Each enemy is worth 100 points. You start with three lives and briefly become invulnerable after a projectile hit. A ship hitting you or reaching the bottom ends the run immediately. Clearing a formation removes leftover shots and starts a faster wave. The best score is written at game over, on leaving a run, or when closing the window normally. Linux uses the XDG data directory (default ~/.local/share), Windows uses LocalAppData, under cpp-space-invaders/best.txt. Use one game instance per save directory.

## Build on Linux (Ubuntu 24.04)

Install Git, CMake 3.24+, a C++20 compiler and SFML's native development dependencies. The first configure downloads the pinned SFML release from GitHub.

```sh
sudo apt update
sudo apt install -y git cmake g++ libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev libgl1-mesa-dev libegl1-mesa-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/space-invaders
```

## Build on Windows (PowerShell)

Install Git, CMake 3.24+ and Visual Studio 2022 with **Desktop development with C++** and a Windows SDK. Run from the extracted project root.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\space-invaders.exe
```

The build copies assets beside the executable. Keep the directory with it when moving the game.

## Headless development

```sh
cmake -S . -B build-core -DBUILD_GAME=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

The model uses only the standard library. The four suites cover collision boundaries, score persistence, wave parameters, and full simulation scenarios including a cleared wave and a lost run. The build workflow also requests a twelve-frame rendering smoke test under Xvfb on Linux.

To play the original turn-by-turn console milestone with GCC:

```sh
g++ -std=c++20 -Isrc examples/mvp.cpp -o build-mvp
./build-mvp
```

## Code walkthrough

Start with [the French interview guide](docs/EXPLICATION_CPP_FR.md), then read Model.hpp, World.cpp and Game.cpp in that order.

## Publish from the extracted folder

Install GitHub CLI and Git, then run:

```sh
gh auth login
git init -b main
git add .
git commit -m "Add C++20 Space Invaders game, tests and CI"
gh repo create cpp-space-invaders --public --source=. --remote=origin --push
```

## Browser edition

The web build compiles the original C++20 game simulation to WebAssembly with Emscripten. Canvas renders two styles from the same simulation snapshot. Pause to switch style mid-run. Browser storage holds the selected style and personal high score. The native SFML build remains available. See [VPS deployment instructions in French](docs/DEPLOIEMENT_WEB_FR.md).
