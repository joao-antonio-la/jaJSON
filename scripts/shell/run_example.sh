#!/bin/sh
# Usage: run_test.sh <example_file_name.c>

# === CHECKS USAGE ===
if [ -z "$1" ]; then
    echo "Usage: $0 <example_file_name.c>"
    exit 1
fi

# === VERIFY FOLDER STRUCTURE ===
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
OUT="build/examples/$BASENAME.out"

# === COMPILING ===
echo "Compiling $SRC -> $OUT"
gcc -Wall -Wextra -Iinclude -Isrc src/jajson.c "$SRC" -o "$OUT" -lm -D__USE_MINGW_ANSI_STDIO

# === EXECUTING ===
if [ $? -eq 0 ]; then
    ./"$OUT"
else
    echo "Compilation failed."
fi