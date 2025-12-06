#include "jajson.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @file test_big.c
 * @brief Tests jaJSON's ability to handle large and deeply nested JSON files.
 *
 * This test loads a complex structure from `tests/data/test_big.json`
 * and validates that specific nested fields are parsed and accessible correctly.
 *
 * That file can be generated with specific length using the python script at tests/aux/generate_big.py
 * Structure excerpt:
 * {
 *   "meta": { "title": "Big JSON Stress Test", "version": 1, "generated": true },
 *   "data": [
 *     {
 *       "id": 0,
 *       "active": true,
 *       "name": "User0",
 *       "scores": [0,1,2,3,4,5,6,7,8,9],
 *       "profile": {
 *         "email": "user0@example.com",
 *         "settings": {
 *           "theme": "dark",
 *           "language": "en",
 *           "notifications": { "email": false, "sms": false, "push": false }
 *         }
 *       }
 *     }
 *   ]
 * }
 */

#define TEST_OK   "\x1b[32mOK\x1b[0m"
#define TEST_FAIL "\x1b[31mFAIL\x1b[0m"

static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief Asserts a test condition and logs its result.
 */
static void assert_test(const char *description, int condition) {
    if (condition) {
        printf("  %s  %s\n", TEST_OK, description);
        tests_passed++;
    } else {
        printf("  %s  %s\n", TEST_FAIL, description);
        tests_failed++;
    }
}

/**
 * @brief Runs a set of validation checks against the loaded JSON structure.
 */
static void run_big_json_tests(ja_json *json) {
    ja_val *root = json->content;
    if (!root || root->type != JA_TYPE_OBJECT) {
        assert_test("Root object validity", 0);
        return;
    }

    // Access meta.title
    ja_val *title = ja_get_obj_at(ja_get_obj_at(root, "meta"), "title");
    assert_test("meta.title should be 'Big JSON Stress Test'",
        title && strcmp(ja_get_str(title), "Big JSON Stress Test") == 0
    );

    // Access data[0].profile.settings.theme
    ja_val *data_arr = ja_get_obj_at(root, "data");
    ja_val *user0 = ja_get_arr_at(data_arr, 0);
    ja_val *theme = ja_get_obj_at(ja_get_obj_at(ja_get_obj_at(user0, "profile"), "settings"), "theme");
    assert_test("data[0].profile.settings.theme should be 'dark'",
        theme && strcmp(ja_get_str(theme), "dark") == 0
    );

    // Access data[0].scores[5]
    ja_val *score5 = ja_get_arr_at(ja_get_obj_at(user0, "scores"), 5);
    assert_test("data[0].scores[5] should be 5",
        score5 && ja_get_int(score5) == 5
    );

    // Access data[0].profile.settings.notifications.email
    ja_val *email_notify = ja_get_obj_at(
        ja_get_obj_at(ja_get_obj_at(ja_get_obj_at(user0, "profile"), "settings"), "notifications"),
        "email"
    );
    assert_test("data[0].profile.settings.notifications.email should be false",
        email_notify && ja_get_bool(email_notify) == false
    );
}

/**
 * @brief Entry point for the jaJSON big data test.
 */
int main(void) {
    printf("\n=== jaJSON Big File Tests ===\n");

    ja_json *json = ja_json_init();
    if (!json) {
        printf("  %s  Failed to initialize JSON object.\n", TEST_FAIL);
        return EXIT_FAILURE;
    }

    if (!ja_read_json(json, "tests/data/test_big.json")) {
        printf("  %s  Failed to read test_big.json.\n", TEST_FAIL);
        ja_json_end(json);
        return EXIT_FAILURE;
    }

    run_big_json_tests(json);

    // 📊 Summary
    printf("\n=================================\n");
    printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=================================\n\n");

    ja_json_end(json);
    return tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
