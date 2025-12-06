# jaJSON

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![Language: C](https://img.shields.io/badge/language-C-blue)
![Platform: Cross](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey)
---
**jaJSON** is a mid/high-level JSON library for C that abstracts parsing, printing, and memory management into a simple API, while still giving users low-level control when needed.
Its goal is to enable clean and easy-to-read code without sacrificing control over the underlying data.

## Quick Start

Here is a short, working example of how to start using the library:

```c
#include "jajson.h"

int main(void) {
    ja_val* data = ja_new_set_obj(
        2,
        "name", ja_new_str("Joao"),
        "age", ja_new_num(19)
    );
    ja_print(data);
    // Output:
    // {"name":"Joao","age":19}

    ja_free_val(&data);
    return 0;
}
```

---

## Features Overview

### File Handling

jaJSON offers functions to easily read, modify, and write JSON files. It parses files into allocated objects and provides full control through a straightforward API.

```c
ja_json *j = ja_json_init();            // -> Allocates memory
ja_read_json(j, "path/to/your/file.json");  // -> Reads file and parses contents
ja_print(j->content);
ja_json_end(j);                         // -> Frees both the file content and parsed data
```

---


### Data Manipulation and Analysis

The library provides convenient methods to modify and query data within JSON objects and arrays. It aims to simplify code through setters and getters.

```c
ja_val *student_obj = ja_new_set_obj( // -> creating an object with preset values.
    3, 
    "name", ja_new_str("John Doe"),
    "age", ja_new_num(20),
    "classes", ja_new_set_arr(
        2,
        ja_new_str("Data Structures"),
        ja_new_str("Web Development")
    )
);
// -> {"name":"John Doe","age":20,"classes":["Data Structures","Web Development"]}

// Appending a new class to the array inside the object.
ja_arr_append(ja_get_obj_at(student_obj, "classes"), ja_new_str("Project Management"));
// -> {"name":"John Doe","age":20,"classes":["Data Structures","Web Development","Project Management"]}
```

---

## Architecture Overview

jaJSON is built around a set of custom types designed to manage JSON data and memory. It balances high-level abstraction with low-level control, allowing the user to choose the desired level of complexity.

---

### Custom Types

#### 1. `ja_val`

This is the core type of the library. Everything is built around it.

```c
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
```

It stores a type discriminator and uses a union of helper fields to simplify access and management.

#### 2. `ja_num`

Helper type storing a synchronized integer and double representation of the same value.

```c
typedef struct ja_num {
    int as_int;
    double as_double;
} ja_num;
```

#### 3. `ja_pair`

Represents a key–value pair used in JSON objects.

```c
typedef struct ja_pair {
    char *key;
    ja_val *value_ptr;
} ja_pair;
```

#### 4. `ja_json`

A high-level structure for handling file I/O.

```c
typedef struct ja_json {
    char *json_str;
    ja_val *content;
} ja_json;
```

---

### jaNew* Functions

The `jaNew*` family contains constructors for `ja_val` objects.
They allocate memory and return fully initialized values:

```c
ja_val *my_number = ja_new_num(3.14);
ja_val *my_string = ja_new_str("This is my string");
ja_val *my_boolean = ja_new_bool(true);
ja_val *my_null = ja_new_null();
```

Arrays and objects can be created in two ways:

1. **Empty container, then insert values later**
   (best when you don’t know the number of elements beforehand)

```c
// Creation:
ja_val *students = ja_new_arr();
ja_val *hours_per_course = ja_new_obj();

// Insertion:
ja_arr_append(students, ja_new_str("Mary"));
ja_arr_append(students, ja_new_str("John"));
ja_arr_append(students, ja_new_str("Clare"));
ja_arr_append(students, ja_new_str("Matt"));
ja_arr_append(students, ja_new_str("Henry"));
ja_arr_append(students, ja_new_str("Lucy"));
// students -> ["Mary", "John", "Clare", "Matt", "Henry", "Lucy"]

ja_set_obj_at(hours_per_course, "Data Structures", ja_new_num(4));
ja_set_obj_at(hours_per_course, "Web Development", ja_new_num(3));
ja_set_obj_at(hours_per_course, "Databases", ja_new_num(3));
ja_set_obj_at(hours_per_course, "Software Engineering", ja_new_num(2));
// hours_per_course -> {"Data Structures":4,"Web Development":3,"Databases":3,"Software Engineering":2}
```

2. **Container with preset values**
   (best when the size is known)
```c
// Creation + Insertion:
ja_val *students = ja_new_set_arr(
    6, // -> length of array
    ja_new_str("Mary"),
    ja_new_str("John"),
    ja_new_str("Clare"),
    ja_new_str("Matt"),
    ja_new_str("Henry"),
    ja_new_str("Lucy")
);
// students -> ["Mary", "John", "Clare", "Matt", "Henry", "Lucy"]

ja_val *hours_per_course = ja_new_set_obj(
    4, // -> length of object
    "Data Structures", ja_new_num(4),
    "Web Development", ja_new_num(3),
    "Databases", ja_new_num(3),
    "Software Engineering", ja_new_num(2)
);
// hours_per_course -> {"Data Structures":4,"Web Development":3,"Databases":3,"Software Engineering":2}
```

---
### Setters and Getters

Setters modify values stored in a `ja_val`.
Getters retrieve them.

#### Setters

These functions will set the value inside a `ja_val` container.

```c
ja_val *my_number = ja_new_num(3.14);
ja_set_num(my_number, 2.71);           // now my_number holds 2.71
ja_set_str(my_number, "was a number"); // now my_number holds "was a number"
```

For arrays and objects, you can set values by indicating the index/key and the value to insert.

```c
ja_val *guest_list = ja_new_set_arr(
    3,
    ja_new_str("Philip"),
    ja_new_str("Alice"),
    ja_new_str("Bob")
); // guest_list -> ["Philip","Alice","Bob"]
ja_set_arr_at(guest_list, 2, ja_new_str("Charlie")); // guest_list -> ["Philip","Alice","Charlie"]    

ja_val *groceries = ja_new_set_obj(
    3,
    "Apples", ja_new_num(5),
    "Bananas", ja_new_num(3),
    "Oranges", ja_new_num(4)
); // groceries -> {"Apples":5,"Bananas":3,"Oranges":4}
ja_set_obj_at(groceries, "Bananas", ja_new_num(6)); // groceries -> {"Apples":5,"Bananas":6,"Oranges":4}
```

#### Getters

These functions will retrieve the value of a `ja_val`.

```c
ja_val *my_string = ja_new_str("This is my string");
char *str_value = ja_get_str(my_string); // str_value -> "This is my string"
```

For arrays and objects, you can get values by indicating the index/key.

```c
ja_val *guest_list = ja_new_set_arr(
    3,
    ja_new_str("Philip"),
    ja_new_str("Alice"),
    ja_new_str("Bob")
);
// guest_list -> ["Philip", "Alice", "Bob"]

ja_print(ja_get_arr_at(guest_list, 2)); // -> Will print "Bob"

ja_val *groceries = ja_new_set_obj(
    3,
    "Apples", ja_new_num(5),
    "Bananas", ja_new_num(3),
    "Oranges", ja_new_num(4)
);
// groceries -> {"Apples":5,"Bananas":3,"Oranges":4

ja_print(ja_get_obj_at(groceries, "Bananas")); // -> Will print 3
```

> ***Important note!***
- When using `ja_get_arr_at()` and `ja_get_obj_at()`, the returned value is a pointer to the internal `ja_val` inside the container. This means that:
    - If you free the original container, the pointer will become invalid. Be careful when using these functions. This is done to avoid unnecessary memory allocations and copying of data, improving performance.
    - You need to call `jaSet*` functions for those as well, not just assign values directly.

---

### Features and Other Functions

- The library offers a wide range of functions to manipulate and manage JSON data.
- It also includes mechanisms to make error logging easier to enable/disable.

> If you want to check practical uses of all functions, check the examples files at [examples/](examples/).

---

#### Array and Object Manipulation

Even though we already showcased the use of setters and getters, there are more functions to manipulate arrays and objects.

##### SetAt and GetAt

These functions allow you to set or get values at specific indices (for arrays) or keys (for objects).

```c
void ja_set_arr_at(ja_val *target, size_t index, ja_val *value);
void ja_set_obj_at(ja_val *target, const char* key, ja_val *value);

ja_val *ja_get_arr_at(ja_val *origin, size_t index);
ja_val *ja_get_obj_at(ja_val *origin, const char *key);
```

> `SetAt` functions will always free the previous value, so be careful. If there is a need for the previous value later, use `ja_copy()` beforehand.

##### Append and Remove

These functions allow you to append new values to arrays or remove values from arrays and objects.

```c
void ja_arr_append(ja_val *target, ja_val *content_to_add);
void ja_arr_remove_at(ja_val *target, size_t index);
void ja_obj_remove_at(ja_val *target, const char *key);
```

Example:
```c
ja_val* arr = ja_new_set_arr(3, ja_new_num(10), ja_new_num(20), ja_new_num(30)); // -> [10,20,30]
ja_arr_append(arr, ja_new_num(40));                                         // -> [10,20,30,40]
ja_arr_remove_at(arr, 0);                                                  // -> [20,30,40]

ja_val* obj = ja_new_set_obj(
    3,
    "a", ja_new_str("x"),
    "b", ja_new_str("y"),
    "c", ja_new_str("z")
);                       // -> {"a":"x","b":"y","c":"z"}
ja_obj_remove_at(obj, "c"); // -> {"a":"x","b":"y"}
```

---

#### Type Conversion

You can convert between types using the `ja_convert_to()` function. This function will change the type of the `ja_val` container and convert the value accordingly.

```c
ja_val* ja_convert_to(ja_val *target, ja_type new_type);
```

Example:
```c
ja_val *port = ja_new_str("8080"); // -> "8080"
ja_convert_to(port, JA_TYPE_INT);  // -> 8080
```

***Special Conversion Rules***
Some conversions have special behavior, here's a description of them:
1. **Number**
    - Arrays/objects will translate to their lengths.
    - Strings will try to be parsed.
2. **Boolean**
    - Empty arrays/object return false.
3. **Array**
    - Numbers and booleans produces an array of length 1, where the only element is the original value.
    - Strings get split at every char and form an array where each element is a char of that string.
    - Nulls result in an empty array.
    - Objects produce an array of its values.
4. **Object**
    - Numbers, strings and booleans, turns into an object where the key is the type, and the value is the previous value (e.g.: 100 -> {"Number": 100}).
    - Null produces an empty object
    - Arrays result in an object of same length, the indexes become string keys for each value.

---

#### Stringify

You can generate a JSON string representation of a `ja_val` container using the `ja_stringify()` function. This function returns an allocated string that you need to free after use.

```c
char* ja_stringify(ja_val* value);
```

---

#### Printing

You can print the JSON representation of a `ja_val` container directly to the terminal using the `ja_print()` function. This function prints directly to stdout and frees the temporary string buffer automatically. It also appends a newline after printing (`\n`).

```c
void ja_print(ja_val* value);
```

---

#### Type Checking and Size

It is possible to check the type of a `ja_val` container using either two options:

1. `ja_enum_type_of()`
```c
int ja_enum_type_of(ja_val *value);
```

2. `ja_str_type_of()`
```c
const char *ja_str_type_of(ja_val *value);
```

Example:
```c
ja_val *str = ja_new_str("testing");
printf("%d\n", ja_enum_type_of(str)); // -> 2
printf("%s\n", ja_str_type_of(str));  // -> String
```

You can also get the size of a string, array, or object using the `ja_size_of()` function.

```c
size_t ja_size_of(ja_val *ja_val);
```

Example: 
```c
ja_val *str = ja_new_str("testing");
ja_val *arr = ja_new_set_arr(3, ja_new_null(), ja_new_null(), ja_new_null());
ja_val *obj = ja_new_set_obj(2, "0", ja_new_null(), "1", ja_new_null());

printf("> str size: %d\n", ja_size_of(str)); // -> 7
printf("> arr size: %d\n", ja_size_of(arr)); // -> 3
printf("> obj size: %d\n", ja_size_of(obj)); // -> 2
```

---

#### Memory Management

Memory management is crucial in C, and jaJSON provides functions to help you free memory allocated for `ja_val` containers.

```c
void ja_free_val(ja_val **ja_val_container_ptr);
```

This function will free the memory allocated for the `ja_val` container and set the pointer to NULL to avoid dangling pointers.

> An important note is that this function will also free all nested values inside arrays and objects, so you don't need to free them separately. As long as you use `ja_free_val()` to free the top-level container, all memory will be properly deallocated.

Example:
```c
ja_val *user = ja_new_set_obj(
    3,
    "id", ja_new_num(2432453245),
    "name", ja_new_str("Bob"),
    "email", ja_new_str("bob@mail.com")
);

ja_free_val(&user); // -> Also frees inner contents as well (id, name, email).
```

##### Note
> ⚠️ **Never share the same `ja_val` instance between multiple containers.**
> Doing so may cause double-free errors. Always use `ja_copy()` for duplication.

❌ **DO NOT:**
```c
ja_val *shared_str = ja_new_str("this is a shared string");
ja_val *arr1 = ja_new_set_arr(2, ja_new_str("First array!"), shared_str);
ja_val *arr2 = ja_new_set_arr(2, ja_new_str("Second array!"), shared_str);
```

✅ **Instead, do:**
```c
ja_val *shared_str = ja_new_str("this is a shared string");
ja_val *arr1 = ja_new_set_arr(2, ja_new_str("First array!"), shared_str);
ja_val *arr2 = ja_new_set_arr(2, ja_new_str("Second array!"), ja_copy(shared_str));
```

---

#### File I/O

You can read and write JSON files using the `ja_json` type and its associated functions.

```c
ja_json* ja_json_init();
int ja_read_json(ja_json *ja_json_object, const char *filename);  // Reads JSON files
void ja_sync_json(ja_json *ja_json_object);                       // Updates the string in the ja_json object to match the current values
int ja_write_json(ja_json *ja_json_object, const char *filename); // Writes JSON files (internally calls ja_sync_json())
void ja_json_end(ja_json *ja_json_object);                        // Finish the ja_json object and frees memory
```

**Example:** 
```c
ja_json *file = ja_json_init();
ja_read_json(file, "config.json");
ja_print(file->content);
ja_write_json(file, "output.json");
ja_json_end(file);
```

---

#### Logging

The library includes a simple logging mechanism that can be enabled or disabled at compile time. This is useful for debugging and tracking errors without cluttering the output in production.

```c
#define JA_DEBUG  // Comment out or delete to disable debug
```

> When `JA_DEBUG` is defined, the library will print debug messages, such as errors, warnings, and information to the terminal. If it's not defined, these messages will be suppressed.

| Macro          | Description             | Output   |
| -------------- | ----------------------- | -------- |
| JA_LOG_ERROR   | Critical error messages | stderr   |
| JA_LOG_WARN    | Non-fatal warnings      | stderr   |
| JA_LOG_INFO    | Informational messages  | stdout   |

### Limitations

> jaJSON keeps things simple and portable.
> For now, it prioritizes correctness and readability over performance and formatting.

#### 1. Number formatting.
- This library has a limit of characters when representing numbers.
- It may have inaccuracies when dealing with big values.

#### 2. No pretty printing.
- The library does not offer pretty printing of JSON values.
- Values are printed in a single line without indentation or line breaks.

#### 3. No hashing for objects.
- Objects are implemented as arrays of key-value pairs.
- This means that accessing values by key has a linear time complexity O(n).

## Notes

### 1. Dependencies

This library uses the following C header files:
```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
```

### 2. Compiling

> All scripts have `.sh` or `.bat` extensions, and are intended to be run from the project root directory (`jaJSON`).

The scripts provided by this library use the following line to compile the code:

```sh
gcc -Wall -Wextra -Iinclude -Isrc src\jajson.c "%SRC%" -o "%OUT%" -lm -D__USE_MINGW_ANSI_STDIO
```
- `gcc`                     : Invokes the GNU Compiler Collection (GCC), used to compile C code.                               
- `-Wall`                   : Enables **all common warnings**, helping detect potential issues early.                          
- `-Wextra`                 : Enables **additional, more specific warnings** beyond `-Wall` for stricter code checking.        
- `-Iinclude`               : Adds the `include/` directory to the list of paths GCC searches for header files (`#include "..."`).
- `-Isrc`                   : Adds the `src/` directory to the include search path, allowing internal headers within the source folder to be found.
- `src\jajson.c`            : The **main source file** of the library — this is the core implementation being linked with the user’s example or test.
- `"%SRC%"`                 : The **user’s source file** (example or test) passed as argument to the script.                   
- `-o "%OUT%"`              : Defines the **output executable path and filename**, typically inside the `build/` directory.    
- `-lm`                     : Links the **math library** (`libm`), required for mathematical functions like `fabs`, `round`, etc.
- `-D__USE_MINGW_ANSI_STDIO`: Defines a macro that **enables ANSI-compliant format specifiers** (like `%zu`) when using MinGW on Windows. Prevents warnings related to size-specific specifiers.

> Feel free to modify the compilation line to suit your needs and environment.

**Quick example:**
```bash
# Example (Linux)
bash scripts/shell/run_example.sh examples/example_basics.c

# Example (Windows)
scripts\batch\run_example.bat examples\example_basics.c
```

### 3. Standard Recommendations

When using this library, it's recommended to follow these practices:

- Always check the return values of functions that can fail, such as memory allocation functions and file I/O functions.
- Always free memory allocated for `ja_val` containers using `ja_free_val()`, unless you are certain that the memory will be managed elsewhere (e.g.: `ja_json_end()`).
- When using `ja_get_arr_at()` and `ja_get_obj_at()`, be cautious about the lifetime of the returned pointers, as they point to internal values of the container.
- Use the logging mechanism to help debug and track errors during development.
- When creating new set arrays and objects, opt for the following code structure:

#### 1. Small Arrays and Objects
```c
ja_val *arr_val = ja_new_set_arr(3, ja_new_num(1.2345), ja_new_str("two"), ja_new_bool(true));
ja_val *obj_val = ja_new_set_obj(2, "name", ja_new_str("Alice"), "age", ja_new_num(30));
```

#### 2. Larger Arrays and Objects
```c
ja_val *arr_val = ja_new_set_arr(
    6,
    ja_new_num(1.2345),
    ja_new_str("two"),
    ja_new_bool(true),
    ja_new_null(),
    ja_new_num(3.14159),
    ja_new_str("end")
);
ja_val *obj_val = ja_new_set_obj(
    4,
    "name", ja_new_str("Alice"),
    "age", ja_new_num(30),
    "is_student", ja_new_bool(false),
    "courses", ja_new_set_arr(
        3,
        ja_new_str("Math"),
        ja_new_str("Science"),
        ja_new_str("History")
    )
);
```

- As mentioned earlier, refrain from shared ownership, as it may cause problems when automatically freeing memory.

## Planned Features

All of these will eventually come to the library in a matter of time:
- Functions Refactoring and Reviews
- Auto Updating
- Utility Functions
- Pretty printing
- Reference counting for shared values
- Object hashing (O(1) lookups)
- Extended type conversion utilities
- Better typing

---

## License & Author

Released under the [MIT License](LICENSE).  
Developed by **João Antônio** — [GitHub](https://github.com/joao-antonio-la)

---

⭐ If you find jaJSON useful, consider starring the repository or sharing feedback!
