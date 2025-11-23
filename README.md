# Pong
Very simple pong game written in C++20 and [Raylib](https://raylib.com).

## Build
Download raylib version 5.5 from [github releases](https://github.com/raysan5/raylib/releases/tag/5.5).
Then extract the archive and rename the resulting directory to `raylib`.
Then build with cmake:
```bash
mkdir -p build

cmake --fresh -B build -S .

cmake --build build
```

## Run
```bash
./build/pong
```
