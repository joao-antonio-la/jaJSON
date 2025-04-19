# jaJSON
Simple C library for dealing with JSON files.
This is a standalone C library, simply download the
`jajson.h` and `jajson.c` files and include them in your project.

## Important Details
### 1. Dependencies
This library uses the following C header files:
```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
```

### 2. %Lf
When displaying `long double` values, this library uses
the format `%Lf` which might not work depending on your
compiler settings. "If you experience issues displaying
`long double` values using `%Lf`, compile with the flag
`-D__USE_MINGW_ANSI_STDIO` when using MinGW and GCC."

### 3. examples.c
Be careful when compiling this file since it is located
on a different folder. If you move it to the same directory
as the `jajson.h` header file, then change the first line
from this:
```c
#include "../src/jajson.h" // when compiling reference the header file with "../src/jajson.h"
```
to this:
```c
#include "jajson.h"
```

### 4. Note
> I do plan on making a revamp of this library,
> bring more functionalities and make improvements.
> Don't know when that will happen though.

## Manual

**[examples.c](examples/examples.c) has multiple cases and shows**
**practical uses of the library functions**

### Types
```c
typedef struct ja_num {
    long int li;
    long double ld;
} ja_num;
```
This custom type is used to hold two values, one representing
an integer value with no decimal places (long int), and a second
value representing any value with decimal places.

```c
typedef struct ja_val {
    enum ja_types type;
    union { ... } u;
} ja_val;
```
Main type used to handle JSON values, uses an Union to encase
different types in one place.

```c
typedef struct {
    char *key;
    ja_val *value_ptr;
} ja_pair;
```
Auxiliar type to hold key-value pairs for JSON objects.

```c
typedef enum {
    JA_NUMBER_LI,
    JA_NUMBER_LD,
    JA_STRING,
    JA_BOOL,
    JA_ARRAY,
    JA_OBJECT,
    JA_NULL
} ja_types;
```
Another auxiliary type, used to set standard values to represent the types.
Also used when calling convert function.

```c
typedef struct {
    char *json_str;
    ja_val *content;
} jaJSON;
```
Type used to manage files.

### Functions

#### Creation functions
These functions will allocate memory for the related type.

```c
ja_val *__jaNewGeneric(void);
ja_val *jaNewNum(long double number_value);
ja_val *jaNewStr(const char *string);
ja_val *jaNewBool(bool boolean_value);
ja_val *jaNewArr(void);
ja_val *jaNewObj(void);
ja_val *jaNewNull(void);
```

These ones are used to create arrays and objects with already set items.
See usage in [examples.c](examples.c)
```c
ja_val *jaNewSetArr(size_t array_size, ...);
ja_val *jaNewSetObj(size_t object_size, ...);
```

This will create an allocated copy of the specified value.
```c
ja_val *jaCopy(ja_val *original);
```

#### Setters
These functions are used to set values inside the containers.
```c
void jaSetNum(ja_val *target, long double number_value);
void jaSetStr(ja_val *target, const char *string);
void jaSetBool(ja_val *target, bool boolean_value);
void jaSetNull(ja_val *target);
```

By indicating an index or key, you can set a value inside the
desired array/object.
```c
void jaSetArrAt(ja_val *target, size_t index, ja_val *value);
void jaSetObjAt(ja_val *target, const char* key, ja_val *value);
```

#### Getters
These functions are used to retrieve data inside the
containers for easier access
```c
long int jaGetNumLI(ja_val *origin);
long double jaGetNumLD(ja_val *origin);
char *jaGetStr(ja_val *origin);
bool jaGetBool(ja_val *origin);
bool jaIsNull(ja_val *origin);
```

The same as the setters, by indicating index or key,
you can access a container.
```c
ja_val *jaGetArrAt(ja_val *origin, size_t index);
ja_val *jaGetObjAt(ja_val *origin, const char *key);
```

#### Modifiers
These functions are used to modify the content inside arrays and objects
```c
void jaArrAppend(ja_val *target, ja_val *content_to_add);
void jaArrRemoveAt(ja_val *target, size_t index);
void jaObjRemoveAt(ja_val *target, const char *key);
```

#### Converters
These functions make it possible to convert between types.
By indicating the target of conversion and the desired type,
can convert the container type and value.
```c
void jaConvertTo(ja_val *target, enum ja_types new_type);
```

Helper functions, no need to call them, only use the one above.
```c
void __jaConvertToNum(ja_val *target);
void __jaConvertToStr(ja_val *target);
void __jaConvertToBool(ja_val *target);
void __jaConvertToArr(ja_val *target);
void __jaConvertToObj(ja_val *target);
```

#### Others
These can be used for additional functionalities like
iterating through arrays/objects or data visualization.

Returns an allocated string with the JSON representation of the value.
```c
char* jaStrJson(ja_val* ja_val);
```

Prints to the terminal the JSON representation of the value.
```c
void jaPrint(ja_val* ja_val);
```

Returns a string with the type of the value.
```c
char *jaTypeOf(ja_val *ja_val);
```

Returns the size of a string, array or object.
```c
size_t jaSizeOf(ja_val *ja_val);
```

#### Parser
Used to turn JSON strings into actual values.
```c
ja_val *jaParse(char **json_str);
```

These helper functions are internally used by `jaParse()`
and don't need to be called directly.
```c
ja_val *__jaParseNumber(char **json_str);
ja_val *__jaParseString(char **json_str);
ja_val *__jaParseBool(char **json_str);
ja_val *__jaParseArray(char **json_str);
ja_val *__jaParseObject(char **json_str);
ja_val *__jaParseNull(char **json_str);
void __jaJumpWhiteSpaces(char **char_ptr);
```

#### Memory Managment
These ones help free memory inside containers.
```c
void jaFreeVal(ja_val **ja_val_container_ptr);
```

`__jaFreeVal()` only deallocates the internal content of a ja_val object,
while `jaFreeVal()` fully deallocates the object.
```c
void __jaFreeVal(ja_val *ja_val);
```

#### Files
These ones can be used to read and write files.

Initializes the object with necessary memory allocations.
```c
jaJSON* jaJsonInit();
```

Can read a JSON file and set the values inside the content pointer.
```c
int jaReadJson(jaJSON *jajson_object, const char *filename);
```

Will write the current content inside the specified filepath.
```c
int jaWriteJson(jaJSON *jajson_object, const char *filename);
```

Frees the memory inside the object.
```c
void jaJsonEnd(jaJSON *jajson_object);
```

This will update the string to match the content.
`jaWriteJson()` internally calls this function.
```c
void jaUpdateJson(jaJSON *jajson_object);
```
