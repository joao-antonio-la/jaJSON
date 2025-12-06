#!/bin/sh
# Usage: run_test.sh <test_file_name.c>

# === CHECKS USAGE ===
if [ -z "$1" ]; then
    echo "Usage: $0 <test_file_name.c>"
    exit 1
fi

# === VERIFING FOLDER STRUCTURE ===
__verify_build_folder__="./scripts/shell/__verify_build_folder__.sh"
if [ -f "$__verify_build_folder__" ]; then
    sh "$__verify_build_folder__"
else
    echo "Error: $__verify_build_folder__ not found!"
    exit 1
fi

# === CONFIGURATION ===
SRC="$1"
BASENAME=$(basename "$SRC" .c)
OUT="build/tests/$BASENAME.out"
if [ -t 1 ]; then
    GREEN=$(printf '\033[32m')
    RED=$(printf '\033[31m')
    RESET=$(printf '\033[0m')
else
    GREEN=''
    RED=''
    RESET=''
fi

# === COMPILING ===
echo "Compiling $SRC -> $OUT"
gcc -Wall -Wextra -Iinclude -Isrc src/jajson.c "$SRC" -o "$OUT" -lm -D__USE_MINGW_ANSI_STDIO

# === EXECUTING ===
if [ $? -eq 0 ]; then
    ./"$OUT"
    if [ $? -eq 0 ]; then
        printf 'Test batch completed successfully. %sOK%s\n' "$GREEN" "$RESET"
    else
        printf 'Test batch failed. %sFAIL%s\n' "$RED" "$RESET"
    fi
else
    echo "Compilation failed."
fi
