#include "jajson.h"

/** 
 * This is an example file of all the basic functionalities present in the library.
 * 
 * In this file we'll applying the base core of jaJSON. 
 */

int main(void) {
    printf("=== jaJSON Basics Example ===\n\n");

    // Create person attributes
    ja_val *name = ja_new_str("Tommy");
    ja_val *age = ja_new_num(24);
    ja_val *is_registered = ja_new_bool(false);
    ja_val *interests = ja_new_set_arr(
        3,
        ja_new_str("Technology"),
        ja_new_str("Programming"),
        ja_new_str("Engineering")
    );
    ja_val *contacts = ja_new_obj();

    // Pack attributes in a single object
    ja_val *Tommy = ja_new_set_obj(
        5,
        "name", name,
        "age", age,
        "is_registered", is_registered,
        "interests", interests,
        "contacts", contacts
    );

    // === Checking data ===
    printf("Before adding contacts:\n");
    ja_print(Tommy);

    printf("\nAdding contact information for Tommy...\n\n");

    // === Inserting contacts info ===
    ja_set_obj_at(contacts, "phone_number", ja_new_str("+1 (123) 567-78901"));
    ja_set_obj_at(ja_get_obj_at(Tommy, "contacts"), "email", ja_new_str("tommy.dev@somemail.com"));

    // === Checking data ===
    printf("After adding contacts:\n");
    ja_print(Tommy);

    // === Cleaning up ===
    ja_free_val(&Tommy); // Frees all nested values recursively — no need to free name, age, etc.
    
    printf("All memory released. Exiting cleanly.\n");
    return 0;
}