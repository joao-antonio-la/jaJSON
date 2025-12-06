#ifndef JAJSON_H
#define JAJSON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#define JA_DEBUG  // Comment out or delete to disable debug

// Logging macros
#ifdef JA_DEBUG
    // Error logs, print to stderr.
    #define JA_LOG_ERROR(msg, ...) fprintf(stderr, "[jaJSON_ERROR] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    // Warning logs, print to stderr, but not as severe as errors
    #define JA_LOG_WARN(msg, ...)  fprintf(stderr, "[jaJSON_WARN] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    // Info logs, print to stdout for general information
    #define JA_LOG_INFO(msg, ...)  fprintf(stdout, "[jaJSON_INFO] %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    // Memory allocation error macro 
    #define JA_MEM_ERROR() JA_LOG_ERROR("Memory allocation failed")

    // Propagation macro for stack trace
    #define JA_PROPAGATE_ERROR(context) \
        JA_LOG_ERROR("Error propagated from: %s", context)
    
#else // When not in debug mode, these macros do nothing
    #define JA_LOG_ERROR(msg, ...)      ((void)0)
    #define JA_LOG_WARN(msg, ...)       ((void)0)
    #define JA_LOG_INFO(msg, ...)       ((void)0)
    #define JA_MEM_ERROR()              ((void)0)
    #define JA_PROPAGATE_ERROR(context) ((void)0)
#endif

// Helper types for numbers
typedef struct ja_num {
    int as_int;
    double as_double;
} ja_num;

typedef struct ja_val ja_val; // Forward declaration of ja_val struct to use it in ja_pair struct.

// Key-value pair structure for JSON objects
typedef struct ja_pair {
    char *key;
    ja_val *value_ptr;
} ja_pair;

// Enum for JSON value types
typedef enum {
    JA_TYPE_INT,
    JA_TYPE_DOUBLE,
    JA_TYPE_STRING,
    JA_TYPE_BOOL,
    JA_TYPE_ARRAY,
    JA_TYPE_OBJECT,
    JA_TYPE_NULL
} ja_type;

// Main JSON value structure
typedef struct ja_val {
    ja_type type;
    union {
        ja_num number;
        char *string;
        bool boolean;
        struct {
            struct ja_val** items;
            size_t size;
        } array;
        struct {
            ja_pair *pairs;
            size_t size;
        } object;
    } u;
} ja_val;

// Structure for handling JSON files
typedef struct ja_json {
    char *json_str;
    ja_val *content;
} ja_json;

/**
 * @brief Creates a new ja_val for a number.
 * 
 * Returns a new allocated ja_val with the value set for the number given.
 * 
 * @param number_value The number value to set (can be integer or floating-point).
 * 
 * @return ja_val* Pointer to the newly created ja_val with number type and value set,
 * or NULL on memory allocation failure.
 */
ja_val *ja_new_num(double number_value);

/**
 * @brief Creates a new ja_val for a string.
 * 
 * Returns a new allocated ja_val with the value set for the string given.
 * 
 * @param string The string to set (char*).
 * 
 * @return ja_val* Pointer to the newly created ja_val with string type and value set,
 * or NULL on memory allocation failure.
 * 
 * @note There is no need to pass a pre-allocated string, since the implementation uses a strdup() call.
 */
ja_val *ja_new_str(const char *string);

/**
 * @brief Creates a new ja_val for a boolean.
 * 
 * Returns a new allocated ja_val with the value set for the boolean given.
 * 
 * @param boolean_value The boolean value to set, can be true or false.
 * 
 * @return ja_val* Pointer to the newly created ja_val with boolean type and value set,
 * or NULL on memory allocation failure.
 */
ja_val *ja_new_bool(bool boolean_value);

/**
 * @brief Creates a new ja_val for an array.
 * 
 * Returns a new allocated ja_val with the value set for an array with no elements.
 * 
 * @return ja_val* Pointer to the newly created ja_val with array type set,
 * or NULL on memory allocation failure.
 */
ja_val *ja_new_arr(void); 

/**
 * @brief Creates a new ja_val for an object.
 * 
 * Returns a new allocated ja_val with the value set for an object with no key-value pairs.
 * 
 * @return ja_val* Pointer to the newly created ja_val with object type set,
 * or NULL on memory allocation failure.
 */
ja_val *ja_new_obj(void);

/**
 * @brief Creates a new ja_val for a null value.
 * 
 * Returns a new allocated ja_val with the value set for null.
 * 
 * @return ja_val* Pointer to the newly created ja_val with null type set,
 * or NULL on memory allocation failure.
 */
ja_val *ja_new_null(void);

/**
 * @brief Creates a new ja_val for an array with initial elements.
 * 
 * Returns a new allocated ja_val with the value set for an array containing the provided elements.
 * 
 * @return ja_val* Pointer to the newly created ja_val with array type and elements set,
 * or NULL on memory allocation failure.
 * 
 * @param array_size The number of elements to initialize the array with.
 * @param ... A variable number of ja_val* arguments to be added to the array.
 * 
 * @note All provided ja_val* arguments should be valid and properly allocated.
 * @note The function takes ownership of the provided ja_val* arguments.
 * @note It is the caller's responsibility to ensure that the number of arguments matches array_size.
 */
ja_val *ja_new_set_arr(size_t array_size, ...);

/**
 * @brief Creates a new ja_val for an object with initial key-value pairs.
 * 
 * Returns a new allocated ja_val with the value set for an object containing the provided key-value pairs.
 * 
 * @return ja_val* Pointer to the newly created ja_val with object type and key-value pairs set,
 * or NULL on memory allocation failure.
 * 
 * @param object_size The number of key-value pairs to initialize the object with.
 * @param ... A variable number of arguments, alternating between const char* (keys) and ja_val* (values).
 * 
 * @note All provided keys should be valid null-terminated strings.
 * @note All provided ja_val* arguments should be valid and properly allocated.
 * @note The function takes ownership of the provided ja_val* arguments.
 * @note It is the caller's responsibility to ensure that the number of arguments matches object_size * 2.
 */
ja_val *ja_new_set_obj(size_t object_size, ...);

/**
 * @brief Creates a copy of a ja_val.
 * 
 * @return a new ja_val with the same values and children.
 * 
 * @param original Value to be copied.
 * 
 * @note This function also copies the inner values of arrays and objects.
 */
ja_val *ja_copy(ja_val *original);

/**
 * @brief Sets the value inside of the target to be the provide number.
 * 
 * @param target ja_val container that will have the value written.
 * @param number_value value that will be set inside the container (can be integer or floating-point)
 * 
 * @note This function automatically decides between registering as double or int.
 * @note As every setter, this function will override the previous value.
 */
void ja_set_num(ja_val *target, double number_value);

/**
 * @brief Sets the value inside of the target to be the provide string.
 * 
 * @param target ja_val container that will have the string written.
 * @param string char pointer that will be set inside the container
 * 
 * @note This function allocates memory for the string provided, so there is no need to allocate beforehand.
 * @note As every setter, this function will override the previous value.
 */
void ja_set_str(ja_val *target, const char *string);

/**
 * @brief Sets the value inside of the target to be the provide bool.
 * 
 * @param target ja_val container that will have the value written.
 * @param boolean_value boolean value that will be set inside the container
 * 
 * @note As every setter, this function will override the previous value.
 */
void ja_set_bool(ja_val *target, bool boolean_value);

/**
 * @brief Sets the value inside of the target to be null.
 * 
 * @param target ja_val container that will have its value set to null.
 * 
 * @note As every setter, this function will override the previous value.
 */
void ja_set_null(ja_val *target);

/**
 * @brief Sets the value at a specific index of the target to be the provided ja_val.
 * 
 * This function takes ownership of `value` (no copy). After calling this function, the caller must not free `value`
 * 
 * @param target Value that will have its contents manipulated.
 * @param index Position of array to be accessed.
 * @param value ja_val that will be set in the position indicated by the index.
 * 
 * @note This function will raise an error if the provided index is invalid (when JA_DEBUG is enabled).
 */
void ja_set_arr_at(ja_val *target, size_t index, ja_val *value);

/**
 * @brief Sets the value at a specific key of the target to be the provided ja_val.
 * 
 * This function takes ownership of `value` (no copy). After calling this function, the caller must not free `value`
 * 
 * @param target Value that will have its contents manipulated.
 * @param key Position of object to be accessed.
 * @param value ja_val that will be set in the position indicated by the key.
 * 
 * @note This function will simply insert the value if the provided key doesn't exist yet.
 */
void ja_set_obj_at(ja_val *target, const char* key, ja_val *value);

/**
 * @brief Function to retrieve an integer from a ja_val.
 * 
 * @return Integer representation of the number value inside the container provided.
 * 
 * @param origin The container from which the value will be retrieved.
 * 
 * @note As every getter, this will not change the type of the container and raise an error if types don't match (when JA_DEBUG is enabled).
 */
int ja_get_int(ja_val *origin);

/**
 * @brief Function to retrieve a double from a ja_val.
 * 
 * @return Double representation of the number value inside the container provided.
 * 
 * @param origin The container from which the value will be retrieved.
 * 
 * @note As every getter, this will not change the type of the container and raise an error if types don't match (when JA_DEBUG is enabled).
 */
double ja_get_double(ja_val *origin);

/**
 * @brief Function to retrieve a string from a ja_val.
 * 
 * @return String (char*) inside the container provided.
 * 
 * @param origin The container from which the string will be retrieved.
 * 
 * @note As every getter, this will not change the type of the container and raise an error if types don't match (when JA_DEBUG is enabled).
 */
char *ja_get_str(ja_val *origin);

/**
 * @brief Function to retrieve a boolean from a ja_val.
 * 
 * @return Boolean value inside the container provided.
 * 
 * @param origin The container from which the boolean value will be retrieved.
 * 
 * @note As every getter, this will not change the type of the container and raise an error if types don't match (when JA_DEBUG is enabled).
 */
bool ja_get_bool(ja_val *origin);

/**
 * @brief Function to verify if the value of a ja_val is null.
 * 
 * @return Boolean value, true if the value is null, false if not.
 * 
 * @param origin The container from which the verification will be performed.
 * 
 * @note This function returns true if the pointer itself is also null, but will raise a warning (when JA_DEBUG is enabled).
 */
bool ja_is_null(ja_val *origin);

/**
 * @brief Retrieves the value at a specific index of the origin (array).
 * 
 * @return ja_val found in the position indicated by the index.
 * 
 * @param origin Value (array) that will have its contents accessed.
 * @param index Position of array to be accessed.
 * 
 * @note If the provided index is invalid, this function will return NULL, and throw a warning (when JA_DEBUG is enabled).
 */
ja_val *ja_get_arr_at(ja_val *origin, size_t index);

/**
 * @brief Retrieves the value at a specific key of the origin (object).
 * 
 * @return ja_val found in the position indicated by the key.
 * 
 * @param origin Value (object) that will have its contents accessed.
 * @param key Key to be accessed in the object.
 * 
 * @note If the provided key is invalid, this function will return NULL, and throw a warning (when JA_DEBUG is enabled).
 */
ja_val *ja_get_obj_at(ja_val *origin, const char *key);

/**
 * @brief Appends a new value at the end of the array and takes ownership of the passed pointer.
 * 
 * @param target Array from which content will be inserted.
 * @param content_to_add New value that'll be put at a the end of the array.
 */
void ja_arr_append(ja_val *target, ja_val *content_to_add);

/**
 * @brief Removes a value from a specific position of the array.
 * 
 * This function frees the value removed, making it unusable.
 * 
 * @param target Array from which content will be removed.
 * @param index Position from which the value will be removed
 * 
 * @note If the index provided is not a valid position on the array, it will raise an error (when JA_DEBUG is enabled).
 */
void ja_arr_remove_at(ja_val *target, size_t index);


/**
 * @brief Removes a value from a specific key of the object.
 * 
 * This function frees the value removed, making it unusable.
 * 
 * @param target Object from which content will be removed.
 * @param key Key from which the value will be removed
 * 
 * @note If the key provided is not valid for the object, it will raise an error (when JA_DEBUG is enabled).
 */
void ja_obj_remove_at(ja_val *target, const char *key);

/**
 * @brief Function to convert values between different types.
 * 
 * It will overwrite the previous value and change the container type.
 * 
 * @return Returns back the target for easier access or copy.
 * 
 * @param target The value which will be converted.
 * 
 * @note Not all conversions are supported. If a conversion in not supported, the code will throw a warning (JA_DEBUG).
 */
ja_val* ja_convert_to(ja_val *target, ja_type new_type);

/**
 * @brief Creates a string that has the representation of a JSON value.
 * 
 * @return JSON string with the values interpreted.
 * 
 * @param value ja_val that will have its string generated.
 * 
 * @note This returns an ALLOCATED string (char*), so it's the users responsibility to manage it.
 */
char* ja_stringify(ja_val* value);

/**
 * @brief Displays on the stdout the string representation of a JSON value.
 * 
 * @param value ja_val that will be displayed.
 * 
 * @note This function uses the ja_stringify internally, but also frees the memory, so it is automatically managed.
 */
void ja_print(ja_val* value);

/**
 * @brief Reads a string and generates a JSON value based on that string.
 * 
 * @return A ja_val constructed based of the JSON string.
 * 
 * @param json_str The contents which will be interpreted.
 * 
 * @note When interpreting a string, for example `"hello"`, the parser will only understand it is a string if it's presented with escaped double quotes, as the examples show.
 * 
 * @example
 * ja_val *parsed_val;
 * parsed_val = ja_parse("1234");
 * parsed_val = ja_parse("\"Hello\"");
 */
ja_val *ja_parse(const char *json_str);

/**
 * @brief Function to access the type of a value.
 * 
 * @return Integer representation of the type enum. -1 if the type is unknown.
 * 
 * @param value ja_val to have its type analyzed.
 */
int ja_enum_type_of(ja_val *value);

/**
 * @brief Function to access the type of a value.
 * 
 * @return Non allocated descriptive string of the type.
 * 
 * @param value ja_val to have its type analyzed.
 */
const char *ja_str_type_of(ja_val *value);

/**
 * @brief Function to access the type of a value.
 * 
 * @return Returns the type as string based on the provided enum value.
 * 
 * @param type Type as enum constant to be retrieved as string.
 */
const char *__ja_type_enum_to_str(ja_type type);

/**
 * @brief Function to access the size of a value.
 * 
 * @return Value of the length, the amount of inner elements. Returns 0 for numbers, booleans and null.
 * 
 * @param value ja_val to be analyzed
 * 
 * @note Only works for strings, arrays and objects.
 */
size_t ja_size_of(ja_val *value);

/**
 * @brief Frees the a ja_val and its contents and sets `*val_ptr = NULL`. Use this to free values you own.
 * 
 * @param val_ptr Pointer to a ja_val pointer (ja_val**) that will be freed.
 * 
 * @note This function recursively frees the memory inside this ja_val, including its children, so be careful.
 */
void ja_free_val(ja_val **val_ptr);

/**
 * @brief Function to initialize a ja_json object, used for files I/O.
 * 
 * @return Allocated pointer to a new ja_json object.
 */
ja_json* ja_json_init();

/**
 * @brief Reads a file and parse its contents.
 * 
 * @return Boolean value based on the success of the operation. true for success, false for failure (with error on JA_DEBUG).
 * 
 * @param ja_json_object Wrapper that will hold the contents. 
 * @param filename Name of the file to be open.
 * 
 * @note Must contain the file extension too (e.g.: "users_data.json").
 */
bool ja_read_json(ja_json *ja_json_object, const char *filename);

/**
 * @brief Writes JSON content in a file.
 * 
 * @return Boolean value based on the success of the operation. true for success, false for failure (with error on JA_DEBUG).
 * 
 * @param ja_json_object Wrapper that holds the contents.
 * @param filename Name of the file to be written.
 * 
 * @note Automatically syncs the inner string to match the content (using ja_sync_json()).
 * @note This will overwrite files that have the same name. Be careful.
 * @note Must contain the file extension too (e.g.: "users_data.json").
 */
bool ja_write_json(ja_json *ja_json_object, const char *filename);

/**
 * @brief Final operation for dealing with a ja_json object. Will free the memory.
 * 
 * @param ja_json_object Content to be handled.
 * 
 * @note This frees both the ja_val and string contained in the object.
 */
void ja_json_end(ja_json *ja_json_object);

/**
 * @brief Syncs the string inside the ja_json object to match the ja_val contained.
 * 
 * @param ja_json_object The instance which needs to be updated.
 * 
 * @note This is essential when writing back a modified file, if it is not called, then contents wont change.
 */
void ja_sync_json(ja_json *ja_json_object);

// === Internal Helper Functions (not for public use) ===

/**
 * @brief Internal function to create a new generic JSON value
 *
 * Creates a new ja_val with uninitialized content.
 *
 * @return ja_val* Pointer to the newly created ja_val with no type or value set,
 * or NULL on memory allocation failure.
 *
 * @note It is not recommended to use this function directly,
 * use the specific jaNew* functions instead.
 */
ja_val *__ja_new_generic(void);

/**
 * @brief Helper function to convert a value to number type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_num(ja_val *target);

/**
 * @brief Helper function to convert a value to string type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_str(ja_val *target);

/**
 * @brief Helper function to convert a value to boolean type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_bool(ja_val *target);

/**
 * @brief Helper function to convert a value to array type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_arr(ja_val *target);

/**
 * @brief Helper function to convert a value to array type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_obj(ja_val *target);

/**
 * @brief Helper function to convert a value to null type.
 * 
 * @param target Value to be converted.
 * 
 * @note Not recommended to use directly.
 */
void __ja_convert_to_null(ja_val *target);

/**
 * @brief Helper function to parse a string.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to number.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_number(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to string.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_string(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to boolean.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_bool(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to array.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_array(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to object.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_object(const char *json_str, int *chars_consumed);

/**
 * @brief Helper function to parse a string to null.
 * 
 * @param json_str String to be interpreted.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 */
ja_val *__ja_parse_null(const char *json_str, int *chars_consumed);

/**
 * @brief Skips the whitespace in a string.
 * 
 * @param str_ptr Pointer to the string (char**) that will be processed.
 * @param chars_consumed Pointer to integer that tracks read characters.
 * 
 * @note Not recommended to use directly.
 * @note It modifies the value of the passed pointer.
 */
void __ja_jump_whitespaces(const char **str_ptr, int *chars_consumed);

/**
 * @brief Helper for freeing a ja_val.
 * 
 * @param val_ptr Pointer to a ja_val (ja_val*) that will be freed.
 * 
 * @note This function recursively frees the memory inside this ja_val, including its children, so be careful.
 * @note It only frees the content inside the value, so it is not recommended to use it, unless you know what you are doing.
 * @note This function is used internally in other functions.
 */
void __ja_free_val(ja_val *value);

#endif //JAJSON_H 