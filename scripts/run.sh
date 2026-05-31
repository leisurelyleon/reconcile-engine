#!/usr/bin/env bash
# Build if needed, then run the CLI over the sample data.
set -euo pipefail

if [ ! -x build/bin/reconcile-cli ]; then
    echo "Binary not found; building first..."
    cmake -S . -B build
    cmake --build build
fi

./build/bin/reconcile-cli data/samples/system_a.csv data/samples/system_b.csv
