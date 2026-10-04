$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
New-Item -ItemType Directory -Force -Path dependencies,models | Out-Null
$wheelName = 'mediapipe-1.0.1-py3-none-win_amd64.whl'
$wheelUrl = 'https://files.pythonhosted.org/packages/22/71/42365b0aec2a96dfbeb3441220fe8dccd9a833f36adfecf3aa9f211c449b/mediapipe-1.0.1-py3-none-win_amd64.whl'
Invoke-WebRequest -Uri $wheelUrl -OutFile "dependencies/$wheelName"
if ((Get-FileHash "dependencies/$wheelName" -Algorithm SHA256).Hash -ne '96DC9DE6BD04A6315EF424FDA5C48E0929F2D78317295E75BC32C0BCEEAB517B') { throw 'MediaPipe package hash mismatch' }
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory((Join-Path $PSScriptRoot "dependencies/$wheelName"),(Join-Path $PSScriptRoot 'dependencies/mediapipe-native'),$true)
$sourceRevision = '102f362f5255388a574de7e1eb4b5656611f8f6d'
Invoke-WebRequest -Uri "https://github.com/google-ai-edge/mediapipe/archive/$sourceRevision.zip" -OutFile dependencies/mediapipe-headers.zip
$archive = [System.IO.Compression.ZipFile]::OpenRead((Join-Path $PSScriptRoot 'dependencies/mediapipe-headers.zip'))
try {
    foreach ($entry in $archive.Entries) {
        if ($entry.FullName -match '^[^/]+/(mediapipe/tasks/c/.*\.h|LICENSE)$') {
            $target = Join-Path $PSScriptRoot ('dependencies/mediapipe-c-sdk/' + $Matches[1])
            New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null
            [System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry,$target,$true)
        }
    }
} finally { $archive.Dispose() }
Invoke-WebRequest -Uri 'https://storage.googleapis.com/mediapipe-models/face_landmarker/face_landmarker/float16/1/face_landmarker.task' -OutFile models/face_landmarker.task
if ((Get-FileHash models/face_landmarker.task -Algorithm SHA256).Hash -ne '64184E229B263107BC2B804C6625DB1341FF2BB731874B0BCC2FE6544E0BC9FF') { throw 'Model hash mismatch' }
Write-Host 'Native MediaPipe DLL, pinned C headers, and model are ready. No Python installation is needed.'
