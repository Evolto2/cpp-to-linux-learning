#!/usr/bin/env bash
set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"
cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux --parallel 2
