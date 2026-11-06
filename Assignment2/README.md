# Assignment 2

An OpenCV application that calibrates a webcam with a checkerboard, matches SIFT features between a reference image and the live camera, and tracks a target while projecting 3D coordinate axes onto it.

## Requirements

- Git
- CMake
- C/C++ compiler

OpenCV is included as a Git submodule in `external/opencv`. Its source code and compiled binaries are **not included** in the submission. Network access is required to download it.

## Get Source

Clone the repository:

```bash
git clone https://github.com/Daniel6702/Visual-Computing-Course.git
cd Visual-Computing-Course/
```

Download OpenCV:

```bash
git submodule update --init external/opencv
```

If the repository is already provided, only the submodule command is necessary.

## Build

From the repository root:

```bash
cmake -S Assignment2 -B Assignment2/build
cmake --build Assignment2/build --parallel 4
```

OpenCV is built automatically on the first build.

## Run

Linux/macOS:

```bash
cd Assignment2/build
./Assignment2
```

Windows:

```powershell
cd Assignment2/build
.\Debug\Assignment2.exe
```

## Usage

The application automatically starts tracking the target (`Assignment2/imgs/target.png`).

When the target is detected, its estimated pose is visualized by projecting a 3D coordinate system onto it.

Press any key to exit.