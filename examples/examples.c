#include "../src/jajson.h" // when compiling reference the header file with "../src/jajson.h"

int main() {
    // Example usage of the JSON library

    // ====================================================================
    // First Example, reading a simple file
    // Create a new JSON object
    jaJSON *j = jaJsonInit();
    // Read JSON from a file
    jaReadJson(j, "example1.json");

    ja_val *c = j->content; // variable for easier access
    
    // Print the JSON content
    // printf("%s\n", j->json_str);
    // or
    printf("\nOriginal first JSON:\n");
    jaPrint(c);
    // or
    // printf("%s\n", jaStrJson(c));
    
    // Modify the JSON content
    jaSetObjAt(c, "Version", jaNewStr("1.0.1"));

    /*
    You can also modify the JSON content by retrieving the value at a specific key and modifying it:
    -> jaSetStr(jaGetObjAt(c, "Version"), "1.0.1");
    */

    // Print the modified JSON content
    printf("\nModified first JSON:\n");
    jaPrint(c);

    // Write the modified JSON to a file
    jaWriteJson(j, "example1_modified.json"); // will create the file if it doesn't exist or overwrite it if it does

    // ====================================================================
    // Second Example, reading another file with all types of values
    // Read JSON from another file
    jaReadJson(j, "example2.json"); // reusing the same jaJSON object
    // Note: This will free the previous content and load the new one.
    c = j->content; // variable for easier access

    // Print the JSON content
    printf("\n\nSecond JSON:\n");
    jaPrint(c);

    // Free the JSON object
    jaJsonEnd(j);
    // Note: This will free the content and the JSON string.
    
    
    // ====================================================================
    // Third Example, creating a JSON object from scratch
    // Create a new ja_val object
    ja_val *json_obj = jaNewSetObj(
        4,
        "name", jaNewStr("John Doe"),
        "age", jaNewNum(30),
        "is_student", jaNewBool(false),
        "courses", jaNewSetArr(
            2,
            jaNewStr("Math"),
            jaNewStr("Science")
        )
    );

    // Print the JSON object
    printf("\n\nThird JSON:\n");
    jaPrint(json_obj);

    // Free the JSON object
    jaFreeVal(&json_obj);
    // Note: jaFreeVal will also free the nested arrays and objects.

    return 0;
}
