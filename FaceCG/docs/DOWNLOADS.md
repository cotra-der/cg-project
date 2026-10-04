# Download manifest

Downloaded on 4 October 2026 from upstream projects. SHA-256 hashes below record
the received files for reproducibility (not independent signature attestations).

| File | Upstream URL | SHA-256 |
|---|---|---|
| `models/face_landmarker.task` | https://storage.googleapis.com/mediapipe-models/face_landmarker/face_landmarker/float16/1/face_landmarker.task | `64184e229b263107bc2b804c6625db1341ff2bb731874b0bcc2fe6544e0bc9ff` |
| `dependencies/mediapipe-v0.10.21.zip` | https://github.com/google-ai-edge/mediapipe/archive/refs/tags/v0.10.21.zip | `c1dacf6fe7105ac4018322df526c3b547de880e9ba584a0bd72782295a4f262b` |
| `dependencies/bazelisk.exe` | https://github.com/bazelbuild/bazelisk/releases/download/v1.25.0/bazelisk-windows-amd64.exe | `641a3dfebd717703675f912917735c44b45cf6300bfdfb924537f3cfbffcdd92` |

MediaPipe source is extracted under `dependencies/mediapipe-0.10.21`; its original
LICENSE is retained. Bazelisk obtains Bazel 6.5.0 as specified by that source's
`.bazelversion`. Large dependencies and the model are ignored by Git, but are
present locally. GLFW, OpenCV, CMake, and Ninja are installed through the existing
MSYS2 UCRT64 package manager, with its normal dependency and signature checks.


## Selected prebuilt native integration

Official MediaPipe 1.0.1 Windows wheel (extracted, not installed as Python):
https://files.pythonhosted.org/packages/22/71/42365b0aec2a96dfbeb3441220fe8dccd9a833f36adfecf3aa9f211c449b/mediapipe-1.0.1-py3-none-win_amd64.whl

SHA-256: `96dc9de6bd04a6315ef424fda5c48e0929f2d78317295e75bc32c0bceeab517b`
This was checked against the official PyPI metadata before extraction.

The official header snapshot is source commit
`102f362f5255388a574de7e1eb4b5656611f8f6d`.
The original downloaded master ZIP has SHA-256
`40ee77664492959839d4d4be25912edee46e6d20faf0e43addc2b09d28298cdd`.
The reproduction script requests the immutable commit archive and retains the
upstream LICENSE with the headers. Original wheel license metadata is retained
under `dependencies/mediapipe-native`.
