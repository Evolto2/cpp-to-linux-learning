param([string]$Compiler = 'clang++', [string]$BuildDir = 'build/windows')
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location -LiteralPath $repoRoot
try {
    cmake -S . -B $BuildDir -G Ninja "-DCMAKE_CXX_COMPILER=$Compiler" -DCMAKE_BUILD_TYPE=Debug
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
    cmake --build $BuildDir --parallel 2
    if ($LASTEXITCODE -ne 0) { throw 'CMake build failed.' }
} finally { Pop-Location }
