#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "jajson.h"

/**
 * This file tests jaJSON's **serialization and file-writing features**.
 * 
 * It verifies:
 *  - ✅ Correct creation and internal structure of JSON values (`jaNew*` functions).
 *  - ✅ Accurate output via `ja_print()` and `ja_write_json()`.
 *  - ✅ File contents match expected JSON text.
 */

#define TEST_OK   "\x1b[32mOK\x1b[0m"
#define TEST_FAIL "\x1b[31mFAIL\x1b[0m"

// Constants used across tests
const int INT_VAL = 1234567890;
const double LD_VAL = 0.1;
const char *STR_VAL = "Hello, World!";
const bool BOOL_VAL = true;

static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief Logs a single test result.
 * @param test_name  Description of the test.
 * @param condition  Whether the test succeeded.
 */
static void log_test_result(const char *test_name, bool condition) {
    printf("%-35s %s\n", test_name, condition ? TEST_OK : TEST_FAIL);
    condition ? tests_passed++ : tests_failed++;
}

/**
 * @brief Entry point for jaJSON write tests.
 */
int main(void) {
    printf("\n=== jaJSON Writing Tests ===\n\n");

    // ✅ Create simple JSON values
    ja_val *number_int = ja_new_num(INT_VAL);
    ja_val *number_ld  = ja_new_num(LD_VAL);
    ja_val *string     = ja_new_str(STR_VAL);
    ja_val *boolean    = ja_new_bool(BOOL_VAL);
    ja_val *null_val   = ja_new_null();

    // ✅ Compose complex structures
    ja_val *array = ja_new_set_arr(3, number_int, boolean, null_val);
    ja_val *object = ja_new_set_obj(
        3,
        "number_ld", number_ld,
        "string", string,
        "array", array
    );

    bool result;

    // === Individual Value Checks ===
    ja_print(number_int); printf("> ");
    result = (
        number_int &&
        number_int->type == JA_TYPE_INT &&
        number_int->u.number.as_int == INT_VAL &&
        ja_get_int(number_int) == INT_VAL
    );
    log_test_result("Write number (long int)", result);

    ja_print(number_ld); printf("> ");
    result = (
        number_ld &&
        number_ld->type == JA_TYPE_DOUBLE &&
        number_ld->u.number.as_double == LD_VAL &&
        ja_get_double(number_ld) == LD_VAL
    );
    log_test_result("Write number (long double)", result);

    ja_print(string); printf("> ");
    result = (
        string &&
        string->type == JA_TYPE_STRING &&
        strcmp(string->u.string, STR_VAL) == 0 &&
        strcmp(ja_get_str(string), STR_VAL) == 0
    );
    log_test_result("Write string", result);

    ja_print(boolean); printf("> ");
    result = (
        boolean &&
        boolean->type == JA_TYPE_BOOL &&
        boolean->u.boolean == BOOL_VAL &&
        ja_get_bool(boolean) == BOOL_VAL
    );
    log_test_result("Write boolean", result);

    ja_print(null_val); printf("> ");
    result = (
        null_val &&
        null_val->type == JA_TYPE_NULL &&
        ja_is_null(null_val)
    );
    log_test_result("Write null", result);

    // === Structure Checks ===
    ja_print(array); printf("> ");
    result = (
        array &&
        array->type == JA_TYPE_ARRAY &&
        array->u.array.size == 3 &&
        ja_get_arr_at(array, 0) == number_int &&
        ja_get_arr_at(array, 1) == boolean &&
        ja_get_arr_at(array, 2) == null_val
    );
    log_test_result("Write array", result);

    ja_print(object); printf("> ");
    result = (
        object &&
        object->type == JA_TYPE_OBJECT &&
        object->u.object.size == 3 &&
        ja_get_obj_at(object, "number_ld") == number_ld &&
        ja_get_obj_at(object, "string") == string &&
        ja_get_obj_at(object, "array") == array
    );
    log_test_result("Write object", result);

    // === File Write Test ===
    ja_json *json = ja_json_init();
    json->content = object;
    ja_write_json(json, "build/data/test_write_output.json");

    FILE *file = fopen("build/data/test_write_output.json", "r");
    bool file_opened = (file != NULL);
    size_t file_size = 0;
    char *file_content = NULL;

    if (file_opened) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        file_content = malloc(file_size + 1);
        size_t bytes_read = fread(file_content, 1, file_size, file);
        file_content[bytes_read] = '\0';
        fclose(file);
    }

    const char *expected_content =
        "{\"number_ld\":0.1,\"string\":\"Hello, World!\",\"array\":[1234567890,true,null]}";

    printf("\nExpected: %s\n", expected_content);
    printf("Got:      %s\n", file_content ? file_content : "(null)");

    result = (
        file_opened &&
        file_content &&
        strcmp(file_content, expected_content) == 0
    );
    log_test_result("> File write and content check", result);

    if (file_content) free(file_content);
    ja_json_end(json);

    // === Summary ===
    printf("\n=================================\n");
    printf("Summary: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("=================================\n\n");

    return tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
