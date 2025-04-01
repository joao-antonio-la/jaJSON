#ifndef JAJSON_H
#define JAJSON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

typedef struct ja_num {
    long int li;
    long double ld;
} ja_num;

typedef struct ja_val ja_val; // Forward declaration of ja_val struct to use it in ja_pair struct.
typedef struct {
    char *key;
    ja_val *value_ptr;
} ja_pair;
enum ja_types { JA_NUMBER_LI, JA_NUMBER_LD, JA_STRING, JA_BOOL, JA_ARRAY, JA_OBJECT, JA_NULL };

typedef struct ja_val {
    enum ja_types type;
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

typedef struct {
    char *json_str;
    ja_val *content;
} jaJSON;

// Functions that return new allocated instances of specific type.
ja_val *__jaNewGeneric(void); // Only allocates for the returning pointer.
ja_val *jaNewNum(long double number_value);
ja_val *jaNewStr(const char *string);
ja_val *jaNewBool(bool boolean_value);
ja_val *jaNewArr(void); // This will initialize an empty array.
ja_val *jaNewObj(void); // This will initialize an empty object.
ja_val *jaNewNull(void);

// Variants for creating new arrays and objects.
ja_val *jaNewSetArr(size_t array_size, ...);
ja_val *jaNewSetObj(size_t object_size, ...);

// Function for copying a value
ja_val *jaCopy(ja_val *original);

// Functions that set values inside a ja_val
void jaSetNum(ja_val *target, long double number_value);
void jaSetStr(ja_val *target, const char *string);
void jaSetBool(ja_val *target, bool boolean_value);
void jaSetNull(ja_val *target);

// Modifying values in an already set index/key on and array/object
void jaSetArrAt(ja_val *target, size_t index, ja_val *value);
void jaSetObjAt(ja_val *target, const char* key, ja_val *value);

// Functions for retrieving values on ja_vals
long int jaGetNumLI(ja_val *origin);
long double jaGetNumLD(ja_val *origin);
char *jaGetStr(ja_val *origin);
bool jaGetBool(ja_val *origin);
bool jaIsNull(ja_val *origin);

// These retrieve the ja_val at specific index/key on arrays/objects
ja_val *jaGetArrAt(ja_val *origin, size_t index);
ja_val *jaGetObjAt(ja_val *origin, const char *key);

// Specific functions for mofifying values inside arrays and objects
void jaArrAppend(ja_val *target, ja_val *content_to_add);
void jaArrRemoveAt(ja_val *target, size_t index);
void jaObjRemoveAt(ja_val *target, const char *key);

// Functions for converting between types
void jaConvertTo(ja_val *target, enum ja_types new_type);
// Helpers
void __jaConvertToNum(ja_val *target);
void __jaConvertToStr(ja_val *target);
void __jaConvertToBool(ja_val *target);
void __jaConvertToArr(ja_val *target);
void __jaConvertToObj(ja_val *target);

// Returns an allocated string with a representation of a JSON value.
char* jaStrJson(ja_val* ja_val);
// (With 12 decimal places of precision for long double numbers)

// Prints a ja_val and frees the allocated memory
void jaPrint(ja_val* ja_val);

// Parsers for JSON strings
ja_val *jaParse(char **json_str);
ja_val *__jaParseNumber(char **json_str);
ja_val *__jaParseString(char **json_str);
ja_val *__jaParseBool(char **json_str);
ja_val *__jaParseArray(char **json_str);
ja_val *__jaParseObject(char **json_str);
ja_val *__jaParseNull(char **json_str);

void __jaJumpWhiteSpaces(char **char_ptr);

// Returns an allocated string with the type of a ja_val
char *jaTypeOf(ja_val *ja_val);
// Returns the size of the value, works for strings, arrays and objects
size_t jaSizeOf(ja_val *ja_val);

// Recursively frees the memory allocated in a ja_val.
void jaFreeVal(ja_val **ja_val_container_ptr);
void __jaFreeVal(ja_val *ja_val); // helper

// Functions for JSON files manipulation.
jaJSON* jaJsonInit();
int jaReadJson(jaJSON *jajson_object, const char *filename);
int jaWriteJson(jaJSON *jajson_object, const char *filename);
void jaJsonEnd(jaJSON *jajson_object);

// Will update the json_str inside the jaJSON object based on the content object.
void jaUpdateJson(jaJSON *jajson_object);

#endif //JAJSON_H