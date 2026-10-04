$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
& cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DFACECG_ENABLE_LIVE=OFF
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
& cmake --build build
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
& .\build\FaceCG.exe --static
