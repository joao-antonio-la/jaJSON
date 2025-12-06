#include "jajson.h"

/**
 * Example: File Input and Output with jaJSON
 *
 * This example demonstrates how to work with JSON files in three different cases:
 *
 * 1. Reading and analyzing existing JSON data.
 * 2. Reading, modifying, and writing data back to the file.
 * 3. Creating brand new JSON data and saving it to disk.
 *
 * To run a specific case, change the value of EXAMPLE_TO_RUN below.
 */

#define EXAMPLE_TO_RUN 3  // Can be 1, 2, or 3

int main(void) {

#if EXAMPLE_TO_RUN == 1  // === CASE 1: READ + ANALYZE ===

    printf("[Running Example %d]\n\n", EXAMPLE_TO_RUN);
    printf("=== Case 1: Reading and analyzing project data ===\n\n");

    const char *filename = "examples/data/IO_1.json";
    ja_json *project = ja_json_init();

    if (!ja_read_json(project, filename)) {
        printf("Failed to read file: %s\n", filename);
        ja_json_end(project);
        return 1;
    }

    printf("Contents of \"%s\":\n", filename);
    ja_print(project->content);
    printf("\n");

    // Access and analyze a specific field
    printf("Analyzing contributors list:\n");
    ja_val *contributors = ja_get_obj_at(project->content, "contributors");
    size_t count = ja_size_of(contributors);

    if (count == 0) {
        printf("No contributors in this project.\n");
    } else if (count == 1) {
        printf("This is a solo project by: %s\n", ja_get_str(ja_get_arr_at(contributors, 0)));
    } else {
        printf("There are %zu contributors:\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu. %s\n", i + 1, ja_get_str(ja_get_arr_at(contributors, i)));
        }
    }

    ja_json_end(project);

#elif EXAMPLE_TO_RUN == 2  // === CASE 2: READ + MODIFY + WRITE ===

    printf("[Running Example %d]\n\n", EXAMPLE_TO_RUN);
    printf("=== Case 2: Reading, modifying, and writing data back ===\n\n");

    const char *users_file = "examples/data/IO_2.json";
    ja_json *users_json = ja_json_init();

    if (!ja_read_json(users_json, users_file)) {
        printf("Failed to read file: %s\n", users_file);
        ja_json_end(users_json);
        return 1;
    }

    ja_val *users = users_json->content;

    int active_users = 0;
    int highest_score = 0;
    ja_val *top_user = NULL;

    for (size_t i = 0; i < ja_size_of(users); i++) {
        ja_val *user = ja_get_arr_at(users, i);

        if (ja_get_bool(ja_get_obj_at(user, "active")))
            active_users++;

        int score = ja_get_int(ja_get_obj_at(user, "highscore"));
        if (score > highest_score) {
            highest_score = score;
            top_user = user;
        }
    }

    printf("There are %d active users.\n", active_users);
    if (top_user) {
        printf(
            "%s has the highest score: %d\n",
            ja_get_str(ja_get_obj_at(top_user, "user_name")),
            ja_get_int(ja_get_obj_at(top_user, "highscore"))
        );
    } else {
        printf("No users found or missing score field.\n");
    }

    // Remove a user (e.g., id = 6)
    int id_to_remove = 6;
    size_t i = 0;
    while (i < ja_size_of(users) &&
           ja_get_int(ja_get_obj_at(ja_get_arr_at(users, i), "id")) != id_to_remove) {
        i++;
    }

    if (i < ja_size_of(users)) {
        ja_arr_remove_at(users, i);
        printf("Removed user with id=%d.\n", id_to_remove);
    } else {
        printf("User with id=%d not found.\n", id_to_remove);
    }

    // Write back
    const char *output_file = "build/data/IO_2_output.json";
    ja_write_json(users_json, output_file);
    ja_json_end(users_json);

#elif EXAMPLE_TO_RUN == 3  // === CASE 3: CREATE + WRITE ===

    printf("[Running Example %d]\n\n", EXAMPLE_TO_RUN);
    printf("=== Case 3: Creating a new JSON file ===\n\n");

    ja_json *groceries = ja_json_init();

    groceries->content = ja_new_set_obj(
        4,
        "banana",   ja_new_num(3),
        "cookies",  ja_new_num(5),
        "bacon",    ja_new_num(2),
        "apples",   ja_new_num(6)
    );

    printf("Generated grocery list:\n");
    ja_print(groceries->content);

    const char *out_file = "build/data/groceries_list.json";
    ja_write_json(groceries, out_file);

    printf("\nWrote grocery list to \"%s\" successfully.\n", out_file);
    ja_json_end(groceries);

#else
    printf("No example executed! Check the value of EXAMPLE_TO_RUN.\n");
#endif

    printf("\n=== Example completed ===\n");
    return 0;
}
