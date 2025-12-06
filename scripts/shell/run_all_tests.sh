#!/bin/sh
# ==========================================
#       jaJSON - Test Suite Runner
# ==========================================

echo "=========================================="
echo "       jaJSON - Test Suite Runner"
echo "=========================================="
echo

# === CONFIGURATION ===
TEST_DIR="tests/src"
SCRIPT_DIR="scripts/shell"
RUN_TEST="$SCRIPT_DIR/run_test.sh"
STOP_ON_FAIL=0   # 1 = stop after first failure, 0 = continue all
PASSED=0
FAILED=0
GREEN=$(printf '\033[32m')
RED=$(printf '\033[31m')
RESET=$(printf '\033[0m')

# === VERIFY run_test.sh EXISTS ===
if [ ! -f "$RUN_TEST" ]; then
    echo "Error: $RUN_TEST not found!"
    exit 1
fi

# === FIND ALL test_*.c FILES ===
for FILE in "$TEST_DIR"/test_*.c; do
    [ -f "$FILE" ] || continue

    echo "Running test: $(basename "$FILE")"
    "$RUN_TEST" "$FILE"
    STATUS=$?

    if [ $STATUS -eq 0 ]; then
        PASSED=$((PASSED + 1))
    else
        FAILED=$((FAILED + 1))
        if [ $STOP_ON_FAIL -eq 1 ]; then
            echo
            echo "Test failed. Stopping early due to STOP_ON_FAIL=1."
            break
        fi
    fi

    echo "------------------------------------------"
done


# === SUMMARY ===
echo
echo "=========================================="
echo "             Test Summary"
echo "=========================================="
printf '%sPassed: %d%s\n' "$GREEN" "$PASSED" "$RESET"
printf '%sFailed: %d%s\n' "$RED" "$FAILED" "$RESET"
echo "=========================================="
echo

# === EXIT CODE ===
if [ "$FAILED" -gt 0 ]; then
    exit 1
else
    exit 0
fi
