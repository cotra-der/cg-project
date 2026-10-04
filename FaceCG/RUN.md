# FaceCG Windows and macOS run guide

FaceCG is a C++17 desktop application. Static mode needs CMake, a C++ compiler,
OpenGL, and GLFW. Live mode additionally needs OpenCV and the native MediaPipe
Face Landmarker library. Keep `models/face_landmarker.task` in place.

## Windows

Install Visual Studio 2022 C++ tools or MSYS2 UCRT64, then use CMake, Ninja,
GLFW, and OpenCV. With the supplied MSYS2 installation:

```powershell
C:\msys64\usr\bin\pacman.exe -Syu
C:\msys64\usr\bin\pacman.exe -S --needed mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-opencv
```

Build and run static mode:

```powershell
cd "C:\Users\Oreo\Desktop\CG project\FaceCG"
powershell -ExecutionPolicy Bypass -File .\run-static.ps1
```

Build and run live mode:

```powershell
cd "C:\Users\Oreo\Desktop\CG project\FaceCG"
powershell -ExecutionPolicy Bypass -File .\run-live.ps1
```

Manual live build:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH
cmake -S . -B build-live -G Ninja -DCMAKE_BUILD_TYPE=Release -DFACECG_ENABLE_LIVE=ON -DFACECG_BUILD_CAMERA_PROBE=ON
cmake --build build-live
.\build-live\FaceCG.exe --live --model models\face_landmarker.task --camera 0
```

Independent checks:

```powershell
.\build-live\facecg_camera_probe.exe
.\build-live\facecg_mediapipe_probe.exe models\face_landmarker.task
```

If Windows reports a missing MediaPipe DLL, copy
`dependencies\mediapipe-native\mediapipe\tasks\c\libmediapipe.dll` beside the
executable as `mediapipe_source.dll`.

## macOS

```bash
brew install cmake ninja glfw opencv
cd "/path/to/CG project/FaceCG"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/FaceCG --static
```

The supplied Windows DLL cannot run on macOS. For live mode, provide a matching
macOS MediaPipe C library and headers:

```bash
cmake -S . -B build-live -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DFACECG_ENABLE_LIVE=ON \
  -DFACECG_MEDIAPIPE_INCLUDE=/path/to/mediapipe-c-sdk \
  -DFACECG_MEDIAPIPE_LIBRARY=/path/to/libmediapipe.dylib
cmake --build build-live
./build-live/FaceCG --live --model models/face_landmarker.task --camera 0
```

The macOS library must expose the C API used by
`src/face/LandmarkDetector.cpp`. A Python-only MediaPipe installation is not
sufficient for the C++ live target.

## Tests and controls

```bash
ctest --test-dir build --output-on-failure
```

Controls: `1` DDA, `2` Bresenham, `3` Bézier, `4` Catmull-Rom, `5` scan-line
fill, `6` full face, `F` toggle fills, `M` mirror, `S` static, `C` camera,
and `Esc` exit.
