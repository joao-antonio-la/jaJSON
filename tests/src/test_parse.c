#include "jajson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This file tests the core JSON parsing function `ja_parse()`.
 * 
 * Each test feeds a JSON string directly to the parser and validates:
 *  - ✅ Correct parsing of valid JSON literals and structures.
 *  - ❌ Proper rejection of malformed or incomplete JSON strings.
 * 
 * The test runner compares both success expectations and type correctness.
 * Results are summarized at the end.
 */

#define TEST_OK   "\x1b[32mOK\x1b[0m"
#define TEST_FAIL "\x1b[31mFAIL\x1b[0m"

static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief Executes a single in-memory JSON parsing test.
 * 
 * @param json_str       The JSON string to parse.
 * @param expect_success Whether parsing is expected to succeed.
 * @param expect_type    Expected root type (-1 for any type).
 */
static void run_test(const char *json_str, int expect_success, int expect_type) {
    printf("\n> Input: %s\n", json_str);
    printf("  Expecting success: %s | Expected type: %d (%s)\n",
           expect_success ? "yes" : "no", expect_type, __ja_type_enum_to_str(expect_type));

    ja_val *json = ja_parse(json_str);
    if (!json) {
        if (expect_success) {
            printf("  %s  Failed to parse valid JSON.\n", TEST_FAIL);
            tests_failed++;
        } else {
            printf("  %s  Parsing failed as expected.\n", TEST_OK);
            tests_passed++;
        }
        return;
    }

    // Display parsed content
    char *stringified = ja_stringify(json);
    printf("  Parsed value: %s\n", stringified ? stringified : "(null)");
    free(stringified);

    if (expect_success) {
        if (expect_type != -1 && (int)json->type != expect_type) {
            printf("  %s  Expected type %d (%s), got %d (%s).\n",
                TEST_FAIL, expect_type, __ja_type_enum_to_str(expect_type), json->type, __ja_type_enum_to_str(json->type));
            tests_failed++;
        } else {
            printf("  %s  Parsed successfully.\n", TEST_OK);
            tests_passed++;
        }
    } else {
        printf("  %s  Unexpectedly parsed invalid JSON.\n", TEST_FAIL);
        tests_failed++;
    }

    ja_free_val(&json);
}

/**
 * @brief Entry point for the jaJSON parser test suite.
 */
int main(void) {
    printf("\n==== jaJSON In-Memory Parse Tests ====\n");

    // ✅ Valid JSON cases
    run_test("{}",                                     1, JA_TYPE_OBJECT);
    run_test("[]",                                     1, JA_TYPE_ARRAY);
    run_test("42",                                     1, JA_TYPE_INT);
    run_test("3.14",                                   1, JA_TYPE_DOUBLE);
    run_test("\"Hello, World!\"",                      1, JA_TYPE_STRING);
    run_test("true",                                   1, JA_TYPE_BOOL);
    run_test("false",                                  1, JA_TYPE_BOOL);
    run_test("null",                                   1, JA_TYPE_NULL);
    run_test("[1,2,3,4]",                              1, JA_TYPE_ARRAY);
    run_test("{\"key\": \"value\", \"num\": 123}",     1, JA_TYPE_OBJECT);
    run_test("{\"a\": \"b\", \"nested\": {\"x\": 1}}", 1, JA_TYPE_OBJECT);

    // ❌ Invalid JSON cases
    run_test("{",                   0, -1);
    run_test("[",                   0, -1);
    run_test("{\"key\": \"value\"", 0, -1);
    run_test("[1, 2, 3",            0, -1);
    run_test("tru",                 0, -1);
    run_test("nul",                 0, -1);
    run_test("12.34.56",            0, -1);
    run_test("\"Unclosed string",   0, -1);

    // 📊 Summary
    printf("\n=================================\n");
    printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=================================\n\n");

    return tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
