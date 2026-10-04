$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
& cmake -S . -B build-live -G Ninja -DCMAKE_BUILD_TYPE=Release -DFACECG_ENABLE_LIVE=ON -DFACECG_BUILD_CAMERA_PROBE=ON
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
& cmake --build build-live
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
& .\build-live\FaceCG.exe --live --model models/face_landmarker.task
