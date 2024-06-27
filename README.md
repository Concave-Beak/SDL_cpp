# My SDL Game engine
Game engine, written in C++ with SDL

## About

This is my first somewhat big project, it allows for
[configuration](./doc/config.md) and for now, it is a simple 2D
platformer/shooter. It features enemies, projectiles and a physics engine all
written in pure C++.

Feel free to give me any feedback!

## Building

### Dependencies

- Linux
- SDL2
- g++
- CMake
- Make

### Build

```sh
git clone https://github.com/RyanQueirozS/SDL_cpp.git
cd SDL_cpp

# If you want to play the development version:
# git checkout develop 

cmake .
make
./bin/SoulBound # project name might change
```

## TODO

- Player and texture assets
- Create enemies and AI
- Better fps handling
  - FPS is being displayed incorrectly.
