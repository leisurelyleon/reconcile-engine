#!/usr/bin/env bash
# Configure and build the project (Release).
set -euo pipefail

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
echo "Build complete. Binary at build/bin/reconcile-cli"
