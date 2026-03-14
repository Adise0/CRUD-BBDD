#!/usr/bin/env bash
set -euo pipefail

printf "Starting watcher...\n"

run_after=1
while getopts "n" opt; do
  case "$opt" in
  n) run_after=0 ;;
  esac
done
shift $((OPTIND - 1))

watchexec --shell=powershell -w src -e cpp,h,html,js,css -r --stop-signal SIGTERM \
  -E RUN_AFTER="$run_after" \
  -- bash ./scripts/dev.sh
