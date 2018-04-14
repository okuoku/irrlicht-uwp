Irrlicht UWP
============

UWP port of Irrlicht game engine using ANGLE GLES2 + SDL2.

This repository is for build files.
See https://github.com/okuoku/irrlicht-generic-sdl for actual port.

Build
=====

Assumes CMake is installed under `PATH`

1. `git submodule --init --recursive`
2. `mkdir build`
3. `cd build`
4. `cmake -P ../do-uwp64-configure.cmake`
5. `devenv irr.sln` and build


