# Assignment 1

An OpenGL application that captures a live webcam feed with OpenCV, applies image filters, and renders the result as a background with a 3D cube on top.

Press `Space` to cycle through the filters.

## Requirements

- Git
- CMake
- C/C++ compiler

OpenCV, GLFW, and GLAD are Git submodules in the shared `external/` directory. Their source code and compiled binaries are **not included** in the submission. Network access is required to download them.

## Get Source

Clone the repository:

```bash
git clone https://github.com/Daniel6702/Visual-Computing-Course.git
cd Visual-Computing-Course/
```

Download the required submodules:

```bash
git submodule update --init external/opencv external/glfw external/glad
```

If the repository is already provided, only the submodule command is necessary.

## Build

From the repository root:

```bash
cmake -S Assignment1 -B Assignment1/build
cmake --build Assignment1/build --parallel 4
```

OpenCV is built automatically on the first build.

## Run

Linux/macOS:

```bash
cd Assignment1/build
./Assignment1
```

Windows:

```powershell
cd Assignment1/build
.\Debug\Assignment1.exe
```

## Controls

- `Space` – Cycle through filters.
- `Alt+F4` – Close the application.