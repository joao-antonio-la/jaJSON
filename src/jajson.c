#include "jajson.h"

ja_val *__jaNewGeneric(void) {
    ja_val *jav = malloc(sizeof(ja_val));

    if (!jav) {
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }

    jav->u.number.li = 0;
    jav->u.number.ld = 0.0;
    jav->u.string = NULL;
    jav->u.boolean = false;
    jav->u.array.items = NULL;
    jav->u.array.size = 0;
    jav->u.object.pairs = NULL;
    jav->u.object.size = 0;
    jav->type = JA_NULL;

    return jav;
}

ja_val *jaNewNum(long double number_value) {
    ja_val* jav = __jaNewGeneric();
    if (!jav) return NULL;

    jav->u.number.li = (long int) number_value;
    jav->u.number.ld = number_value;

    long int temp = number_value;

    jav->type = number_value - temp == 0 ? JA_NUMBER_LI : JA_NUMBER_LD;

    return jav;
}

ja_val *jaNewStr(const char *string) {
    ja_val* jav = __jaNewGeneric();
    if (!jav) return NULL;
    
    jav->type = JA_STRING;
    jav->u.string = strdup(string);

    if (!jav->u.string) {
        free(jav);
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }

    return jav;
}

ja_val *jaNewBool(bool boolean_value) {
    ja_val* jav = __jaNewGeneric();
    if (!jav) return NULL;
    
    jav->type = JA_BOOL;
    jav->u.boolean = boolean_value;

    return jav;
}

ja_val *jaNewArr(void) {
    ja_val* jav = __jaNewGeneric();
    if (!jav) return NULL;
    
    jav->type = JA_ARRAY;
    jav->u.array.items = NULL;
    jav->u.array.size = 0;

    return jav;
}

ja_val *jaNewObj(void) {
    ja_val* jav = __jaNewGeneric();
    if (!jav) return NULL;
    
    jav->type = JA_OBJECT;
    jav->u.object.pairs = NULL;
    jav->u.object.size = 0;

    return jav;
}

ja_val *jaNewNull(void) {
    return __jaNewGeneric();
}

ja_val *jaNewSetArr(size_t array_size, ...) {
    ja_val* jav = jaNewArr();
    if (!jav) return NULL;

    jav->u.array.items = malloc(sizeof(ja_val*) * array_size);
    if (!jav->u.array.items) {
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }

    va_list array_list;
    va_start(array_list, array_size);

    for (size_t i = 0; i < array_size; i++) {
        jav->u.array.items[i] = va_arg(array_list, ja_val*);
    }

    va_end(array_list);
    jav->u.array.size = array_size;

    return jav;
}

ja_val *jaNewSetObj(size_t object_size, ...) {
    ja_val* jav = jaNewObj();
    if (!jav) return NULL;

    jav->u.object.pairs = malloc(sizeof(ja_pair) * object_size);
    if (!jav->u.object.pairs) {
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }

    va_list object_list;
    va_start(object_list, object_size);

    for (size_t i = 0; i < object_size; i++) {
        jav->u.object.pairs[i].key = strdup(va_arg(object_list, char*));
        jav->u.object.pairs[i].value_ptr = va_arg(object_list, ja_val*);
    }

    va_end(object_list);
    jav->u.object.size = object_size;

    return jav;
}

ja_val *jaCopy(ja_val *original) {
    switch (original->type) {
    case JA_NUMBER_LD:
    case JA_NUMBER_LI: return jaNewNum(original->u.number.ld);
    case JA_STRING: return jaNewStr(original->u.string);
    case JA_BOOL: return jaNewBool(original->u.boolean);
    case JA_ARRAY: {
        ja_val* copy = jaNewArr();
        for (size_t i = 0; i < original->u.array.size; i++) {
            jaArrAppend(copy, jaCopy(original->u.array.items[i]));
        }
        return copy;
    }
    case JA_OBJECT: {
        ja_val* copy = jaNewObj();
        for (size_t i = 0; i < original->u.object.size; i++) {
            jaSetObjAt(
                copy,
                original->u.object.pairs[i].key,
                jaCopy(original->u.object.pairs[i].value_ptr)
            );   
        }
        return copy;
    }
    case JA_NULL: return jaNewNull();
    default:
        fprintf(stderr, "Can't handle this type.\n");
        return NULL;
    }
}

void jaSetNum(ja_val *target, long double number_value) {
    if (target->type != JA_NUMBER_LD && target->type != JA_NUMBER_LI) {
        fprintf(stderr, "Can't set number on non-number value.\n");
        return;
    }
    
    target->u.number.li = (long int) number_value;
    target->u.number.ld = number_value;

    long int temp = number_value;

    target->type = number_value - temp == 0 ? JA_NUMBER_LI : JA_NUMBER_LD;
}

void jaSetStr(ja_val *target, const char *string) {
    if (target->type != JA_STRING) {
        fprintf(stderr, "Can't set string on non-string value.\n");
        return;
    }
    free(target->u.string);
    target->u.string = strdup(string);
}

void jaSetBool(ja_val *target, bool boolean_value) {
    if (target->type != JA_BOOL) {
        fprintf(stderr, "Can't set bool on non-boolean value.\n");
        return;
    }
    target->u.boolean = boolean_value;
}

void jaSetNull(ja_val *target) {
    __jaFreeVal(target);
    target->type = JA_NULL;
}

void jaSetArrAt(ja_val *target, size_t index, ja_val *value) {
    if (target->type != JA_ARRAY) {
        fprintf(stderr, "Can't use jaSetArrayAt() on non-array value");
        return;
    }
    if (index >= target->u.array.size) {
        fprintf(stderr, "Index out of bounds.\n");
        return;
    }
    __jaFreeVal(target->u.array.items[index]);
    target->u.array.items[index] = value;
}

void jaSetObjAt(ja_val *target, const char *key, ja_val *value) {
    if (target->type != JA_OBJECT) {
        fprintf(stderr, "Can't use jaSetObjAt() on non-object value");
        return;
    }

    for (size_t i = 0; i < target->u.object.size; i++) {
        if (strcmp(target->u.object.pairs[i].key, key) == 0) {
            __jaFreeVal(target->u.object.pairs[i].value_ptr);
            target->u.object.pairs[i].value_ptr = value;
            return;
        }
    }

    size_t new_size = target->u.object.size + 1;
    
    ja_pair *new_pairs = realloc(target->u.object.pairs, new_size * sizeof(ja_pair));
    if (!new_pairs) {
        fprintf(stderr, "Error while allocating memory.\n");
        return;
    }

    target->u.object.pairs = new_pairs;
    
    target->u.object.pairs[target->u.object.size].key = strdup(key);
    target->u.object.pairs[target->u.object.size].value_ptr = value;

    target->u.object.size = new_size;
}

long int jaGetNumLI(ja_val *origin) {
    if (origin->type != JA_NUMBER_LD && origin->type != JA_NUMBER_LI) {
        fprintf(stderr, "Can't use jaGetNumLI() in non-number value.\n");
        return 0;
    }
    return origin->u.number.li;
}

long double jaGetNumLD(ja_val *origin) {
    if (origin->type != JA_NUMBER_LD && origin->type != JA_NUMBER_LI) {
        fprintf(stderr, "Can't use jaGetNumLD() in non-number value.\n");
        return 0;
    }
    return origin->u.number.ld;
}

char* jaGetStr(ja_val *origin) {
    if (origin->type != JA_STRING) {
        fprintf(stderr, "Can't use jaGetStr() in non-string value.\n");
        return "(null)";
    }
    return origin->u.string;
}

bool jaGetBool(ja_val *origin) {
    if (origin->type != JA_BOOL) {
        fprintf(stderr, "Can't use jaGetBool() in non-boolean value.\n");
        return 0;
    }
    return origin->u.boolean;
}

bool jaIsNull(ja_val *origin) {
    return origin->type == JA_NULL;
}

ja_val *jaGetArrAt(ja_val *origin, size_t index) {
    if (origin->type != JA_ARRAY) {
        fprintf(stderr, "Can't use jaGetArrAt() in non-array value");
        return NULL;
    }
    if (index >= origin->u.array.size) {
        fprintf(stderr, "Index out of bounds.\n");
        return NULL;
    }
    return origin->u.array.items[index];
}

ja_val *jaGetObjAt(ja_val *origin, const char *key) {
    if (origin->type != JA_OBJECT) {
        fprintf(stderr, "Can't use jaGetObjAt() in non-object value.\n");
        return NULL;
    }
    for (size_t i = 0; i < origin->u.object.size; i++) {
        if (strcmp(origin->u.object.pairs[i].key, key) == 0) {
            return origin->u.object.pairs[i].value_ptr;
        }
    }
    fprintf(stderr, "Key not found.\n");
    return NULL;
}

void jaArrAppend(ja_val *target, ja_val *content_to_add) {
    if (target->type != JA_ARRAY) {
        fprintf(stderr, "Can't use jaArrAppend() in non-array value.\n");
        return;
    }

    size_t new_size = target->u.array.size + 1;

    ja_val** new_items = realloc(target->u.array.items, sizeof(ja_val*) * new_size);
    if (!new_items) {
        fprintf(stderr, "Error while allocating memory.\n");
        return;   
    }

    target->u.array.items = new_items;
    target->u.array.items[target->u.array.size] = content_to_add;
    target->u.array.size = new_size;
}

void jaArrRemoveAt(ja_val *target, size_t index) {
    if (target->type != JA_ARRAY) {
        fprintf(stderr, "Can't use jaArrRemoveAt() in non-array value.\n");
        return;
    }
    if (index >= target->u.array.size) {
        fprintf(stderr, "Index out of bounds.\n");
        return;
    }
    
    __jaFreeVal(target->u.array.items[index]);

    for (size_t i = index; i < target->u.array.size - 1; i++) {
        target->u.array.items[i] = target->u.array.items[i + 1];
    }
    
    target->u.array.size--;

    ja_val **new_items = realloc(target->u.array.items, sizeof(ja_val*) * target->u.array.size);
    if (new_items || target->u.array.size == 0) {  
        target->u.array.items = new_items;
    }
}

void jaObjRemoveAt(ja_val *target, const char *key) {
    if (target->type != JA_OBJECT) {
        fprintf(stderr, "Can't use jaObjRemoveAt() in non-object value.\n");
        return;
    }

    size_t index = target->u.object.size;
    for (size_t i = 0; i < target->u.object.size; i++) {
        if (strcmp(target->u.object.pairs[i].key, key) == 0) {
            free(target->u.object.pairs[i].key);
            __jaFreeVal(target->u.object.pairs[i].value_ptr);
            index = i;
            break;
        }
    }

    if (index == target->u.object.size) {
        fprintf(stderr, "Key not found: %s\n", key);
        return;
    }
    
    for (size_t i = index; i < target->u.object.size - 1; i++) {
        target->u.object.pairs[i] = target->u.object.pairs[i + 1];
    }

    target->u.object.size--;

    ja_pair *new_pairs = realloc(target->u.object.pairs, sizeof(ja_pair) * target->u.object.size);
    if (new_pairs || target->u.object.size == 0) {  
        target->u.object.pairs = new_pairs;
    }
}


void jaConvertTo(ja_val *target, enum ja_types new_type) {
    if (!target) return;
    if (target->type == new_type) return;

    switch (new_type) { 
    case JA_NUMBER_LI:
    case JA_NUMBER_LD:
        __jaConvertToNum(target);
        break;
    case JA_STRING:
        __jaConvertToStr(target);
        break;
    case JA_BOOL:
        __jaConvertToBool(target);
        break;
    case JA_ARRAY:
        __jaConvertToArr(target);
        break;
    case JA_OBJECT:
        __jaConvertToObj(target);
        break;
    case JA_NULL:
        jaSetNull(target);
        break;
    default:
        fprintf(stderr, "Invalid type conversion.\n");
        break;
    }
}

void __jaConvertToNum(ja_val *target) {
    switch (target->type) {
    case JA_NUMBER_LI: 
    case JA_NUMBER_LD: 
        break;

    case JA_STRING: {
        char *end_ptr;
        long double number_value = strtold(target->u.string, &end_ptr);

        if (end_ptr == target->u.string) {
            fprintf(stderr, "Error while converting string to number.\n");
            return;
        }

        if (target->u.string) free(target->u.string);
        target->u.number.li = (long int) number_value;
        target->u.number.ld = number_value;

        long int temp = number_value;

        target->type = number_value - temp == 0 ? JA_NUMBER_LI : JA_NUMBER_LD;
        break;
    }

    case JA_BOOL: {
        target->u.number.li = target->u.boolean ? 1 : 0;
        target->u.number.ld = target->u.boolean ? 1.0 : 0.0;
        target->type = JA_NUMBER_LI;
        break;
    }

    case JA_ARRAY:
    case JA_OBJECT: {
        size_t temp = jaSizeOf(target);
        __jaFreeVal(target);
        target->u.number.li = (long int) temp;
        target->u.number.ld = (long double) temp;
        target->type = JA_NUMBER_LI;
        break;
    }
    
    case JA_NULL: {
        target->u.number.li = 0;
        target->u.number.ld = 0.0;
        target->type = JA_NUMBER_LI;
        break;
    }

    default: {
        fprintf(stderr, "Invalid type for conversion to number.\n");
        break;
    }
    }
}

void __jaConvertToStr(ja_val *target) {
    switch (target->type) {
    case JA_NUMBER_LI:
    case JA_NUMBER_LD: {
        char* str = jaStrJson(target);
        if (target->u.string) free(target->u.string);
        target->u.string = str;
        target->type = JA_STRING;
        break;
    }

    case JA_STRING:
        break;

    case JA_BOOL: {
        if (target->u.string) free(target->u.string);
        
        target->u.string = strdup(target->u.boolean ? "true" : "false");
        if (!target->u.string) {
            fprintf(stderr, "Error while allocating memory.\n");
            return;
        }
        target->type = JA_STRING;
        break;
    }

    case JA_ARRAY:
    case JA_OBJECT: {
        char* str = jaStrJson(target);
        if (!str) {
            fprintf(stderr, "Error while converting array/object to string.\n");
            return;
        }
        if (target->u.string) free(target->u.string);
        target->u.string = str;
        target->type = JA_STRING;
        break;
    }

    case JA_NULL: {
        if (target->u.string) free(target->u.string);
        target->u.string = strdup("null");
        target->type = JA_STRING;
        break;
    }

    default: {
        fprintf(stderr, "Invalid type for conversion to string.\n");
        break;
    }
    }
}

void __jaConvertToBool(ja_val *target) {
    switch (target->type) {
    case JA_NUMBER_LI:
    case JA_NUMBER_LD:
        target->u.boolean = target->u.number.li != 0;
        target->type = JA_BOOL;
        break;

    case JA_STRING: {
        target->u.boolean = strlen(target->u.string) > 0;
        target->type = JA_BOOL;
        if (target->u.string) free(target->u.string);
        break;
    }

    case JA_BOOL:
        break;

    case JA_ARRAY:
    case JA_OBJECT: {
        target->u.boolean = target->u.array.size > 0 || target->u.object.size > 0;
        target->type = JA_BOOL;
        __jaFreeVal(target);
        break;
    }

    case JA_NULL: {
        target->u.boolean = false;
        target->type = JA_BOOL;
        break;
    }

    default:
        fprintf(stderr, "Invalid type for conversion to boolean.\n");
        break;
    }
}

void __jaConvertToArr(ja_val *target) {
    switch (target->type) {
    case JA_NUMBER_LI:
    case JA_NUMBER_LD: {
        ja_val *num_cpy = jaCopy(target);
        ja_val *new_arr = jaNewArr();
        if (!new_arr || !num_cpy) {
            return;
        }

        __jaFreeVal(target);
        jaArrAppend(new_arr, num_cpy);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_ARRAY;
        free(new_arr);
        return;
    }

    case JA_STRING: {
        ja_val *str_cpy = jaCopy(target);
        ja_val *new_arr = jaNewArr();
        if (!new_arr || !str_cpy) {
            return;
        }

        size_t str_length = strlen(target->u.string);

        char* start = target->u.string;
        char* space = strchr(target->u.string, ' ');

        if (space) { // has space, so separate it by space
            while (space) {
                char sub_str[str_length];
                snprintf(sub_str, space - start + 1, start);

                start = space + 1;
                space = strchr(start, ' ');

                jaArrAppend(new_arr, jaNewStr(sub_str));
            }
            jaArrAppend(new_arr, jaNewStr(start));

        } else { // does not have space, so separate it by char
            for (size_t i = 0; i < str_length; i++) {
                char sub_str[2];
                strncpy(sub_str, str_cpy->u.string + i, 1);
                jaArrAppend(new_arr, jaNewStr(sub_str));
            }
        }

        __jaFreeVal(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_ARRAY;
        free(new_arr);
        return;
    }

    case JA_BOOL: {
        ja_val *bool_cpy = jaCopy(target);
        ja_val *new_arr = jaNewArr();
        if (!new_arr || !bool_cpy) {
            return;
        }

        __jaFreeVal(target);
        jaArrAppend(new_arr, bool_cpy);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_ARRAY;
        free(new_arr);
        return;
    }

    case JA_NULL: {
        ja_val *new_arr = jaNewArr();
        if (!new_arr) {
            return;
        }

        __jaFreeVal(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_ARRAY;
        free(new_arr);
        return;
    }

    case JA_ARRAY:
        break;

    case JA_OBJECT: {
        ja_val *new_arr = jaNewArr();
        if (!new_arr) {
            return;
        }

        for (size_t i = 0; i < target->u.object.size; i++) {
            jaArrAppend(new_arr, jaCopy(target->u.object.pairs[i].value_ptr));
        }

        __jaFreeVal(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_ARRAY;
        free(new_arr);
        break;
    }

    default: {
        fprintf(stderr, "Invalid type for conversion to array.\n");
        break;
    }
    }
}

void __jaConvertToObj(ja_val *target) {
    switch (target->type) {
    case JA_NUMBER_LI:
    case JA_NUMBER_LD:
    case JA_STRING:
    case JA_BOOL: {
        ja_val *new_obj = jaNewObj();
        ja_val *var_cpy = jaCopy(target);
        if (!new_obj || !var_cpy) {
            return;
        }

        jaSetObjAt(new_obj, "key", var_cpy);
        __jaFreeVal(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_OBJECT;
        free(new_obj);
        break;
    }

    case JA_NULL: {
        ja_val *new_obj = jaNewObj();
        if (!new_obj) {
            return;
        }

        __jaFreeVal(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_OBJECT;
        free(new_obj);
        break;
    }

    case JA_ARRAY: {
        ja_val *new_obj = jaNewObj();
        ja_val *arr_cpy = jaCopy(target);
        if (!new_obj || !arr_cpy) {
            return;
        }

        for (size_t i = 0; i < target->u.array.size; i++) {
            char idx_buffer[32];
            sprintf(idx_buffer, "%zu", i);
            jaSetObjAt(new_obj, idx_buffer, jaCopy(jaGetArrAt(target, i)));
        }
        
        __jaFreeVal(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_OBJECT;
        free(new_obj);
        break;
    }

    case JA_OBJECT:
        break;

    default: {
        fprintf(stderr, "Invalid type for conversion to array.\n");
        break;
    }
    }
}

char* jaStrJson(ja_val* ja_val) {
    if (!ja_val) {
        fprintf(stderr, "NULL value.\n");
        return strdup("ERROR");
    }

    switch (ja_val->type) {
    case JA_NUMBER_LI: {
        char* str = malloc(21 * sizeof(char));
        if (!str) {
            fprintf(stderr, "Error while allocating memory.\n");
            return strdup("ERROR");
        }
        snprintf(str, 21, "%ld", ja_val->u.number.li);
        return str;
    }

    case JA_NUMBER_LD: {
        char* str = malloc(128 * sizeof(char));
        if (!str) {
            fprintf(stderr, "Error while allocating memory.\n");
            return strdup("ERROR");
        }

        int len = snprintf(str, 128, "%.12Lf", ja_val->u.number.ld);

        char* decimal_point = strchr(str, '.');
        if (!decimal_point)
            return str;

        char* end = str + len - 1;
        while (end > decimal_point && *end == '0') {
            *end = '\0';
            end--;
        }

        if (end == decimal_point)
            *end = '\0';

        return str;
    }

    case JA_STRING: {
        size_t len = strlen(ja_val->u.string);
        size_t extra = 0;

        for (size_t i = 0; i < len; i++) {
            if (ja_val->u.string[i] == '\"' || ja_val->u.string[i] == '\\') {
                extra++; 
            }
        }

        char* str = malloc((len + extra + 3) * sizeof(char));
        if (!str) {
            fprintf(stderr, "Error while allocating memory.\n");
            return strdup("ERROR");
        }

        char* ptr = str;
        *ptr++ = '"';

        for (size_t i = 0; i < len; i++) {
            char c = ja_val->u.string[i];
            if (c == '"' || c == '\\') {
                *ptr++ = '\\';
            }
            *ptr++ = c;
        }

        *ptr++ = '"';
        *ptr = '\0';

        return str;
    }

    case JA_BOOL:
        return strdup(ja_val->u.boolean ? "true" : "false");

    case JA_ARRAY: {
        if (ja_val->u.array.size == 0) return strdup("[]");

        char** elements = malloc(ja_val->u.array.size * sizeof(char*));
        if (!elements) {
            fprintf(stderr, "Error while allocating memory.\n");
            return strdup("ERROR");
        }

        size_t total_length = 2;
        for (size_t i = 0; i < ja_val->u.array.size; i++) {
            elements[i] = jaStrJson(ja_val->u.array.items[i]);
            total_length += strlen(elements[i]) + 2;
        }

        char* str = malloc(total_length * sizeof(char));
        if (!str) {
            fprintf(stderr, "Error while allocating memory.\n");
            for (size_t i = 0; i < ja_val->u.array.size; i++) free(elements[i]);
            free(elements); 
            return strdup("ERROR");
        }

        strcpy(str, "[");
        for (size_t i = 0; i < ja_val->u.array.size - 1; i++) {
            strcat(str, elements[i]);
            strcat(str, ",");
            free(elements[i]);
        }

        strcat(str, elements[ja_val->u.array.size - 1]);
        strcat(str, "]");
        free(elements[ja_val->u.array.size - 1]);
        free(elements);

        return str;
    }

    case JA_OBJECT: {
        if (ja_val->u.object.size == 0) return strdup("{}");

        char** pairs = malloc(ja_val->u.object.size * sizeof(char*));
        if (!pairs) {
            fprintf(stderr, "Error while allocating memory.\n");
            return strdup("ERROR");
        }

        size_t total_length = 2;
        for (size_t i = 0; i < ja_val->u.object.size; i++) {
            char* value_json = jaStrJson(ja_val->u.object.pairs[i].value_ptr);
            pairs[i] = malloc((strlen(ja_val->u.object.pairs[i].key) + 4 + strlen(value_json)) * sizeof(char));
            if (!pairs[i]) {
                fprintf(stderr, "Error while allocating memory.\n");
                free(value_json);
                return strdup("ERROR");
            }

            sprintf(pairs[i], "\"%s\":%s", ja_val->u.object.pairs[i].key, value_json);
            total_length += strlen(pairs[i]) + 2;
            free(value_json);
        }

        char* str = malloc(total_length * sizeof(char));
        if (!str) {
            fprintf(stderr, "Error while allocating memory.\n");
            free(pairs);
            return strdup("ERROR");
        }

        strcpy(str, "{");
        for (size_t i = 0; i < ja_val->u.object.size - 1; i++) {
            strcat(str, pairs[i]);
            strcat(str, ",");
            free(pairs[i]);
        }
        
        strcat(str, pairs[ja_val->u.object.size - 1]);
        strcat(str, "}");
        free(pairs[ja_val->u.object.size - 1]);
        free(pairs);

        return str;
    }

    case JA_NULL:
        return strdup("null");

    default:
        fprintf(stderr, "Can't retrieve string from this type.\n");
        return strdup("(unknown)");
    }
}

void jaPrint(ja_val* ja_val) {
    if (!ja_val) {
        fprintf(stderr, "NULL value.\n");
        return;
    }

    char* str = jaStrJson(ja_val);
    if (str) {
        printf("%s\n", str);
        free(str);
    } else {
        fprintf(stderr, "Error while converting to string.\n");
    }
}

ja_val *jaParse(char **json_str) {
    __jaJumpWhiteSpaces(json_str);

    if (**json_str == '\0') {
        fprintf(stderr, "Empty string.\n");
        return NULL;
    }

    switch (**json_str) {
    case '{':
        return __jaParseObject(json_str);
    case '[':
        return __jaParseArray(json_str);
    case '"':
        return __jaParseString(json_str);
    case 't':
    case 'f':
        return __jaParseBool(json_str);
    case 'n':
        return __jaParseNull(json_str);
    default:
        if (isdigit(**json_str) || **json_str == '-') {
            return __jaParseNumber(json_str);
        } else {
            fprintf(stderr, "Invalid character: %c\n", **json_str);
            return NULL;
        }
    }
}

ja_val *__jaParseNumber(char **json_str) {
    char *end_ptr = *json_str;
    long double number_value = strtold(*json_str, &end_ptr);

    if (end_ptr == *json_str) {
        fprintf(stderr, "Error while parsing number.\n");
        return NULL;
    }

    *json_str = end_ptr;
    return jaNewNum(number_value);
}

ja_val *__jaParseString(char **json_str) {
    char *start = ++(*json_str);
    char *end = NULL;
    size_t length = 0;

    while (**json_str && **json_str != '"') {
        if (**json_str == '\\') {
            (*json_str)++;
            if (**json_str == 'u') {
                (*json_str) += 4;
            } else {
                (*json_str)++;
            }
        } else {
            (*json_str)++;
        }
    }

    if (**json_str != '"') {
        fprintf(stderr, "Unmatched quotes in string.\n");
        return NULL;
    }

    length = *json_str - start;
    char *temp_str = malloc(length + 1);
    if (!temp_str) {
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }
    strncpy(temp_str, start, length);
    temp_str[length] = '\0';
    ja_val *jav = jaNewStr(temp_str);
    if (!jav) {
        free(temp_str);
        return NULL;
    }
    free(temp_str);
    (*json_str)++;

    return jav;
}

ja_val *__jaParseBool(char **json_str) {
    if (strncmp(*json_str, "true", 4) == 0) {
        *json_str += 4;
        return jaNewBool(true);
    } else if (strncmp(*json_str, "false", 5) == 0) {
        *json_str += 5;
        return jaNewBool(false);
    } else {
        fprintf(stderr, "Invalid boolean value.\n");
        return NULL;
    }
}

ja_val *__jaParseArray(char **json_str) {
    ja_val *jav = jaNewArr();
    if (!jav) return NULL;

    (*json_str)++;

    while (**json_str) {
        __jaJumpWhiteSpaces(json_str);

        if (**json_str == ']') {
            (*json_str)++;
            return jav;
        }

        ja_val *value = jaParse(json_str);
        if (!value) {
            jaFreeVal(&jav);
            return NULL;
        }

        jaArrAppend(jav, value);

        __jaJumpWhiteSpaces(json_str);

        if (**json_str == ',') {
            (*json_str)++;
        } else if (**json_str != ']') {
            fprintf(stderr, "Invalid character in array: %c\n", **json_str);
            jaFreeVal(&jav);
            return NULL;
        }
    }

    fprintf(stderr, "Unmatched brackets in array.\n");
    jaFreeVal(&jav);
    return NULL;
}

ja_val *__jaParseObject(char **json_str) {
    ja_val *jav = jaNewObj();
    if (!jav) return NULL;

    (*json_str)++;

    while (**json_str) {
        __jaJumpWhiteSpaces(json_str);

        if (**json_str == '}') {
            (*json_str)++;
            return jav;
        }

        if (**json_str != '"') {
            fprintf(stderr, "Invalid character in object key: %c\n", **json_str);
            jaFreeVal(&jav);
            return NULL;
        }

        char *key = __jaParseString(json_str)->u.string;
        if (!key) {
            jaFreeVal(&jav);
            return NULL;
        }

        __jaJumpWhiteSpaces(json_str);

        if (**json_str != ':') {
            fprintf(stderr, "Missing colon after key: %s\n", key);
            free(key);
            jaFreeVal(&jav);
            return NULL;
        }

        (*json_str)++;

        __jaJumpWhiteSpaces(json_str);

        ja_val *value = jaParse(json_str);
        if (!value) {
            free(key);
            jaFreeVal(&jav);
            return NULL;
        }

        jaSetObjAt(jav, key, value);
        free(key);

        __jaJumpWhiteSpaces(json_str);

        if (**json_str == ',') {
            (*json_str)++;
        } else if (**json_str != '}') {
            fprintf(stderr, "Invalid character in object: %c\n", **json_str);
            jaFreeVal(&jav);
            return NULL;
        }
    }

    fprintf(stderr, "Unmatched brackets in object.\n");
    jaFreeVal(&jav);
    return NULL;
}

ja_val *__jaParseNull(char **json_str) {
    if (strncmp(*json_str, "null", 4) == 0) {
        *json_str += 4;
        return jaNewNull();
    } else {
        fprintf(stderr, "Invalid null value.\n");
        return NULL;
    }
}

void __jaJumpWhiteSpaces(char **char_ptr) {
    while (**char_ptr && isspace(**char_ptr)) {
        (*char_ptr)++;
    }
}

char *jaTypeOf(ja_val *ja_val) {
    switch (ja_val->type) {
    case JA_NUMBER_LI: return "NUMBER (LONG INT)";
    case JA_NUMBER_LD: return "NUMBER (LONG DOUBLE)";
    case JA_STRING: return "STRING";
    case JA_BOOL: return "BOOL";
    case JA_ARRAY: return "ARRAY";
    case JA_OBJECT: return "OBJECT";
    case JA_NULL: return "NULL";
    default: return "UNKNOWN";
    }
}

size_t jaSizeOf(ja_val *ja_val) {
    switch (ja_val->type) {
    case JA_STRING: return strlen(ja_val->u.string);
    case JA_ARRAY: return ja_val->u.array.size;
    case JA_OBJECT: return ja_val->u.object.size;
    default:
        fprintf(stderr, "Can't use jaSizeOf() for this type (%s).\n", jaTypeOf(ja_val));
        return 0;
    }
}

void jaFreeVal(ja_val **ja_val_container_ptr) {
    if (!ja_val_container_ptr || !(*ja_val_container_ptr)) return;
    
    __jaFreeVal(*ja_val_container_ptr);
    free(*ja_val_container_ptr);
    *ja_val_container_ptr = NULL;
}

void __jaFreeVal(ja_val *ja_val) {
    if (!ja_val) return;

    switch (ja_val->type) {
        case JA_STRING:
            free(ja_val->u.string);
            ja_val->u.string = NULL;
            break;
        case JA_ARRAY:
            for (size_t i = 0; i < ja_val->u.array.size; i++) {
                jaFreeVal(&ja_val->u.array.items[i]);
            }
            free(ja_val->u.array.items);
            ja_val->u.array.items = NULL;
            break;
        case JA_OBJECT:
            for (size_t i = 0; i < ja_val->u.object.size; i++) {
                free(ja_val->u.object.pairs[i].key);
                ja_val->u.object.pairs[i].key = NULL;
                jaFreeVal(&ja_val->u.object.pairs[i].value_ptr);
            }
            free(ja_val->u.object.pairs);
            ja_val->u.object.pairs = NULL;
            break;
        default:
            break;
    }
}

jaJSON* jaJsonInit() {
    jaJSON *jajson_object = malloc(sizeof(jaJSON));
    if (!jajson_object) {
        fprintf(stderr, "Error while allocating memory.\n");
        return NULL;
    }

    jajson_object->json_str = NULL;
    jajson_object->content = NULL;

    return jajson_object;
}

int jaReadJson(jaJSON *jajson_object, const char *filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error while opening file.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Error while allocating memory.\n");
        fclose(file);
        return 1;
    }

    size_t bytesRead = fread(buffer, 1, file_size, file);
    if (bytesRead != file_size) {
        fprintf(stderr, "Error: Expected to read %ld bytes but only read %zu.\n", file_size, bytesRead);
        free(buffer);
        fclose(file);
        return 1;
    }
    
    buffer[file_size] = '\0';
    fclose(file);

    jajson_object->json_str = buffer;
    jajson_object->content = jaParse(&buffer);
    if (!jajson_object->content) {
        fprintf(stderr, "Error while parsing JSON.\n");
        free(buffer);
        return 1;
    }

    return 0;
}

int jaWriteJson(jaJSON *jajson_object, const char *filename) {
    jaUpdateJson(jajson_object);

    if (!jajson_object->json_str) {
        fprintf(stderr, "No JSON string to write.\n");
        return -1;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1;
    }

    fputs(jajson_object->json_str, file);
    fclose(file);

    return 0;
}

void jaJsonEnd(jaJSON *jajson_object) {
    if (jajson_object->json_str) {
        free(jajson_object->json_str);
        jajson_object->json_str = NULL;
    }

    if (jajson_object->content) {
        jaFreeVal(&jajson_object->content);
        jajson_object->content = NULL;
    }

    free(jajson_object);
}

void jaUpdateJson(jaJSON *jajson_object) {
    if (!jajson_object->content) {
        fprintf(stderr, "No content to update.\n");
        return;
    }

    char *json_str = jaStrJson(jajson_object->content);
    if (!json_str) {
        fprintf(stderr, "Error while converting content to JSON string.\n");
        return;
    }

    free(jajson_object->json_str);
    jajson_object->json_str = json_str;
}