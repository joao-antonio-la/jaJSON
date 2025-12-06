#include "jajson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This file runs a basic automated test suite for the jaJSON library.
 * 
 * Each test validates JSON parsing behavior across both valid and invalid inputs.
 * 
 * - ✅ Successful parsing: checks for correct structure and type.
 * - ❌ Expected failures: ensures invalid files are properly rejected.
 * 
 * The test runner reports each result individually and provides a summary at the end.
 */

#define TEST_OK   "\x1b[32mOK\x1b[0m"
#define TEST_FAIL "\x1b[31mFAIL\x1b[0m"

static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief Runs a single test case on a JSON file.
 * 
 * @param filename       Path to the test JSON file.
 * @param expect_success Whether parsing is expected to succeed.
 * @param expect_type    Expected root type (-1 for any type).
 */
static void run_test(const char *filename, int expect_success, int expect_type) {
    printf("\n> Testing: %s\n", filename);

    ja_json *json = ja_json_init();
    if (!json) {
        printf("  %s  Initialization failed.\n", TEST_FAIL);
        tests_failed++;
        return;
    }

    if (!ja_read_json(json, filename)) {
        if (expect_success) {
            printf("  %s  Failed to parse valid JSON.\n", TEST_FAIL);
            tests_failed++;
        } else {
            printf("  %s  Parsing failed as expected.\n", TEST_OK);
            tests_passed++;
        }
        ja_json_end(json);
        return;
    }

    // Success path
    if (expect_success) {
        if (!json->content) {
            printf("  %s  No content after successful parse.\n", TEST_FAIL);
            tests_failed++;
        } else if (expect_type != -1 && (int)json->content->type != expect_type) {
            printf("  %s  Expected type %d, got %d.\n", TEST_FAIL, expect_type, json->content->type);
            tests_failed++;
        } else {
            printf("  %s  Parsed successfully.\n", TEST_OK);
            tests_passed++;
        }
    } else {
        printf("  %s  Unexpectedly parsed invalid file.\n", TEST_FAIL);
        tests_failed++;
    }

    ja_json_end(json);
}

/**
 * @brief Entry point for the jaJSON test suite.
 */
int main(void) {
    printf("\n=== jaJSON Parse Tests ===\n");

    // ✅ Valid JSON cases
    run_test("tests/data/test_file/test_minimal.json", 1, -1);
    run_test("tests/data/test_file/test_numbers.json", 1, JA_TYPE_ARRAY);
    run_test("tests/data/test_file/test_strings.json", 1, JA_TYPE_ARRAY);
    run_test("tests/data/test_file/test_arrays.json",  1, JA_TYPE_ARRAY);
    run_test("tests/data/test_file/test_objects.json", 1, JA_TYPE_OBJECT);
    run_test("tests/data/test_file/test_mixed.json",   1, JA_TYPE_OBJECT);

    // ❌ Invalid JSON case
    run_test("tests/data/test_file/test_invalid.json", 0, -1);

    // 📊 Summary
    printf("\n=================================\n");
    printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=================================\n\n");

    return tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
