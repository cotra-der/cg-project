# Dependency proof and native integration

Initial checks on 4 October 2026 found GCC 14.2.0 but no CMake, GLFW,
OpenCV, MediaPipe SDK, or model. Those missing prerequisites were recorded before
full integration, as required by the implementation plan.

## Downloaded native MediaPipe

The application uses the official MediaPipe 1.0.1 `libmediapipe.dll`, called
**directly from C++ through MediaPipe's C ABI**. The DLL is distributed inside the
official Windows wheel; extraction does not install Python, and neither Python
nor a Python subprocess is used by the application. This is the actual MediaPipe
Face Landmarker, not a substitute detector. CG and rendering interfaces remain
independent of MediaPipe.

- DLL: `dependencies/mediapipe-native/mediapipe/tasks/c/libmediapipe.dll`
- Official headers: `dependencies/mediapipe-c-sdk`
- Header source revision: `102f362f5255388a574de7e1eb4b5656611f8f6d`
- Model: `models/face_landmarker.task`
- Redownload: `download-native-dependencies.ps1` (PowerShell 7).

The selected option/result layouts were compared with the ctypes declarations
shipped with version 1.0.1. C++ static assertions guard their x64 sizes.
The native DLL has successfully loaded and closed the model in a dependency
smoke check. C++ inference and camera results are tracked in `VALIDATION.md`.

On Windows, the provided build uses MSYS2 UCRT64 / MinGW, whose linker can consume
the DLL directly. For a different platform, supply a matching native MediaPipe
C library with `FACECG_MEDIAPIPE_LIBRARY` and its official headers through
`FACECG_MEDIAPIPE_INCLUDE`. An MSVC build needs the corresponding import library
and DLL deployment configuration; that alternative is not verified here.

## Source-build attempt (not required for the selected DLL integration)

MediaPipe 0.10.21 source and Bazelisk 1.25.0 were also downloaded. Bazel 6.5.0
rejected the project path because it contains a space. A retry in a temporary
space-free directory advanced, but failed fetching
`rules_python/releases/download/0.34.0/rules_python-0.34.0.tar.gz` with
`javax.net.ssl.SSLException: Connection reset`. No substitute was introduced.
The official prebuilt native DLL resolves this dependency without a Bazel build.

## System libraries

CMake, Ninja, GLFW, and OpenCV are being installed using MSYS2 UCRT64.
The original 2024 package index returned 404s for removed archives. A normal
MSYS2 core update followed by a consistent package update resolved that issue.
The installer retained its package signature and disk-space checks. A proposed
retry without the slow space check was aborted by its state guard when the
original installer advanced; no configuration change was applied.

## External acceptance commands

- `FaceCG --probe-gl --frames 2`: read back a white pixel at (100,100).
- `facecg_camera_probe`: capture 60 nonempty webcam frames without MediaPipe.
- `facecg_mediapipe_probe models/face_landmarker.task`: C++ model initialization
  and blank-frame inference.
- `FaceCG --probe-landmarks`: capture a moving face over 150 frames and verify
  changing selected coordinates.
- `FaceCG --live`: manually check head motion, mouth changes, blinking,
  mirroring, no-face handling, and measured FPS.

The official API source is
https://github.com/google-ai-edge/mediapipe/blob/102f362f5255388a574de7e1eb4b5656611f8f6d/mediapipe/tasks/c/vision/face_landmarker/face_landmarker.h
