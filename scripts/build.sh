#!/usr/bin/env bash
set -euo pipefail

preset=$1

printf "[Build] Running pre-build commands\n"

printf "[Build] Configuring preset: %s\n" "$preset"
cmake --preset "$preset"

printf "[Build] Building preset: %s\n" "$preset"
cmake --build --preset "$preset"

printf "[Build] Done!\n"
