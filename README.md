# Assignment 1

An OpenGL application that captures a live webcam feed with OpenCV, applies image filters, and renders the result as a background with a 3D cube on top.

Press `Space` to cycle through the filters.

## Requirements

- Git
- CMake
- C/C++ compiler

OpenCV, GLFW, and GLAD are included as submodules and built automatically.

## Build

```bash
git clone --recurse-submodules https://github.com/Daniel6702/Visual-Computing-Course.git

cd Visual-Computing-Course/Assignment1/

cmake -S . -B build
cmake --build build --parallel 4
```

## Run

Linux/macOS:

```bash
./build/Assignment1
```
Windows:

```bash
build\Debug\Assignment1.exe
```

### Stop

`Alt+F4`