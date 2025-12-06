#include "jajson.h"

ja_val *__ja_new_generic(void) {
    ja_val *jav = malloc(sizeof(ja_val));

    if (!jav) {
        JA_MEM_ERROR();
        return NULL;
    }

    memset(&jav->u, 0, sizeof(jav->u));
    return jav;
}

ja_val *ja_new_num(double number_value) {
    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_num");
        return NULL;
    }
    
    jav->u.number.as_int = (int)number_value;
    jav->u.number.as_double = number_value;

    int temp = number_value;

    jav->type = number_value - temp == 0 ? JA_TYPE_INT : JA_TYPE_DOUBLE;

    return jav;
}

ja_val *ja_new_str(const char *string) {
    if (!string) {
        JA_LOG_ERROR("Can't create ja_val from NULL string.");
        return NULL;
    }

    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_str");
        return NULL;
    }
    
    jav->type = JA_TYPE_STRING;
    jav->u.string = strdup(string);

    if (!jav->u.string) {
        free(jav);
        JA_MEM_ERROR();
        return NULL;
    }

    return jav;
}

ja_val *ja_new_bool(bool boolean_value) {
    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_bool");
        return NULL;
    }
    
    jav->type = JA_TYPE_BOOL;
    jav->u.boolean = boolean_value;

    return jav;
}

ja_val *ja_new_arr(void) {
    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_arr");
        return NULL;
    }
    
    jav->type = JA_TYPE_ARRAY;
    jav->u.array.items = NULL;
    jav->u.array.size = 0;

    return jav;
}

ja_val *ja_new_obj(void) {
    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_obj");
        return NULL;
    }
    
    jav->type = JA_TYPE_OBJECT;
    jav->u.object.pairs = NULL;
    jav->u.object.size = 0;

    return jav;
}

ja_val *ja_new_null(void) {
    ja_val* jav = __ja_new_generic();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_null");
        return NULL;
    }

    jav->type = JA_TYPE_NULL;

    return jav;
}

ja_val *ja_new_set_arr(size_t array_size, ...) {
    ja_val* jav = ja_new_arr();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_set_arr");
        return NULL;
    }

    jav->u.array.items = malloc(sizeof(ja_val*) * array_size);
    if (!jav->u.array.items) {
        JA_MEM_ERROR();
        ja_free_val(&jav);
        return NULL;
    }

    va_list array_list;
    va_start(array_list, array_size);

    for (size_t i = 0; i < array_size; i++) {
        ja_val* value_arg = va_arg(array_list, ja_val*);
        if (!value_arg) {
            JA_LOG_ERROR("Can't read ja_val at index %zu to create array.", i);
            ja_free_val(&jav);
            return NULL;
        }
        jav->u.array.items[i] = value_arg;
    }

    va_end(array_list);
    jav->u.array.size = array_size;

    return jav;
}

ja_val *ja_new_set_obj(size_t object_size, ...) {
    ja_val *jav = ja_new_obj();
    if (!jav) {
        JA_PROPAGATE_ERROR("ja_new_set_obj");
        return NULL;
    }

    jav->u.object.pairs = malloc(sizeof(ja_pair) * object_size);
    if (!jav->u.object.pairs) {
        JA_MEM_ERROR();
        ja_free_val(&jav);
        return NULL;
    }

    va_list object_list;
    va_start(object_list, object_size);

    for (size_t i = 0; i < object_size; i++) {
        char *key_arg = va_arg(object_list, char*);
        ja_val *value_arg = va_arg(object_list, ja_val*);

        if (!key_arg || !value_arg) {
            JA_LOG_ERROR("Invalid key or value at index %zu", i);

            for (size_t j = 0; j < i; j++) {
                free(jav->u.object.pairs[j].key);
                ja_free_val(&(jav->u.object.pairs[j].value_ptr));
            }
            free(jav->u.object.pairs);
            ja_free_val(&jav);
            va_end(object_list);
            return NULL;
        }

        jav->u.object.pairs[i].key = strdup(key_arg);
        if (!jav->u.object.pairs[i].key) {
            JA_MEM_ERROR();

            for (size_t j = 0; j < i; j++) {
                free(jav->u.object.pairs[j].key);
                ja_free_val(&(jav->u.object.pairs[j].value_ptr));
            }
            free(jav->u.object.pairs);
            ja_free_val(&jav);
            va_end(object_list);
            return NULL;
        }

        jav->u.object.pairs[i].value_ptr = value_arg;
    }

    va_end(object_list);
    jav->u.object.size = object_size;

    return jav;
}

ja_val *ja_copy(ja_val *original) {
    if (!original) {
        JA_LOG_ERROR("ja_copy() received NULL pointer");
        return NULL;
    }

    ja_val* copy = NULL;
    
    switch (original->type) {
    case JA_TYPE_INT: 
    case JA_TYPE_DOUBLE: {
        copy = ja_new_num(original->u.number.as_double);
        if (!copy) break;
        
        return copy;
    }
    case JA_TYPE_STRING: {
        copy = ja_new_str(original->u.string);
        if (!copy) break;
         
        return copy;
    }
    case JA_TYPE_BOOL: {
        copy = ja_new_bool(original->u.boolean);
        if (!copy) break;
        
        return copy;
    }
    case JA_TYPE_ARRAY: {
        copy = ja_new_arr();
        if (!copy) break;
        
        for (size_t i = 0; i < original->u.array.size; i++) {
            ja_val *item_copy = ja_copy(original->u.array.items[i]);
            if (!item_copy) {
                ja_free_val(&copy);
                JA_PROPAGATE_ERROR("ja_copy");
                return NULL;
            }
            ja_arr_append(copy, item_copy);
        }
        
        return copy;
    }
    case JA_TYPE_OBJECT: {
        copy = ja_new_obj();
        if (!copy) break;
        
        for (size_t i = 0; i < original->u.object.size; i++) {
            ja_val *inner_value_copy = ja_copy(original->u.object.pairs[i].value_ptr);
            if (!inner_value_copy) {
                ja_free_val(&copy);
                JA_PROPAGATE_ERROR("ja_copy");
                return NULL;
            }
            
            ja_set_obj_at(
                copy,
                original->u.object.pairs[i].key, // No need to use strdup since ja_set_obj_at() already does that
                inner_value_copy
            );   
        }
        return copy;
    }
    case JA_TYPE_NULL: {
        copy = ja_new_null();
        if (!copy) break;

        return copy;
    }
    default:
        JA_LOG_ERROR("Can't copy this type.");
        return NULL;
    }
    
    JA_PROPAGATE_ERROR("ja_copy");
    return NULL;
}

void ja_set_num(ja_val *target, double number_value) {
    if (!target) {
        JA_LOG_ERROR("Can't set value on NULL ja_val.");
        return;
    }

    double diff = fabs(number_value - (double)((int)number_value));

    if (target->type != JA_TYPE_DOUBLE && target->type != JA_TYPE_INT) {
        (diff < 1e-9) ?
            JA_LOG_INFO("Changing %s to Number (%d).", ja_str_type_of(target), (int)number_value)
        :
            JA_LOG_INFO("Changing %s to Number (%g).", ja_str_type_of(target), number_value);
    }

    __ja_free_val(target);
    memset(&target->u, 0, sizeof(target->u));
    
    target->u.number.as_int = (int)number_value;
    target->u.number.as_double = number_value;

    target->type = (diff < 1e-9) ? JA_TYPE_INT : JA_TYPE_DOUBLE;
}

void ja_set_str(ja_val *target, const char *string) {
    if (!target) {
        JA_LOG_ERROR("Can't set value on NULL ja_val.");
        return;
    }
    
    if (!string) {
       JA_LOG_ERROR("Can't set NULL string on ja_val.");
        return;
    }

    if (target->type != JA_TYPE_STRING) {
        JA_LOG_INFO("Changing %s to String (\"%s\").", ja_str_type_of(target), string);
    }
    
    __ja_free_val(target);
    memset(&target->u, 0, sizeof(target->u));

    target->type = JA_TYPE_STRING;
    target->u.string = strdup(string);
    if (!target->u.string) {
        JA_MEM_ERROR();
    }
}

void ja_set_bool(ja_val *target, bool boolean_value) {
    if (!target) {
        JA_LOG_ERROR("Can't set value on NULL ja_val.");
        return;
    }
    
    if (target->type != JA_TYPE_BOOL) {
        JA_LOG_INFO("Changing %s to Boolean (%s).", 
            ja_str_type_of(target), boolean_value ? "true" : "false");
    }

    __ja_free_val(target);
    memset(&target->u, 0, sizeof(target->u));
    
    target->u.boolean = boolean_value;
    target->type = JA_TYPE_BOOL;
}

void ja_set_null(ja_val *target) {
    if (!target) {
        JA_LOG_ERROR("Can't set value on NULL ja_val.");
        return;
    }
    
    if (target->type != JA_TYPE_NULL) {
        JA_LOG_INFO("Changing %s to Null", ja_str_type_of(target));
    }
    
    __ja_free_val(target);
    memset(&target->u, 0, sizeof(target->u));

    target->type = JA_TYPE_NULL;
}

void ja_set_arr_at(ja_val *target, size_t index, ja_val *value) {
    if (!target || !value) {
        JA_LOG_ERROR("NULL pointers passed to ja_set_arr_at().");
        return;
    }
    
    if (target->type != JA_TYPE_ARRAY) {
        JA_LOG_ERROR("Can't use jaSetArrayAt() on non-array value");
        return;
    }

    if (index >= target->u.array.size) {
        JA_LOG_ERROR("Index out of bounds (index=%zu, size=%zu).", index, target->u.array.size);
        return;
    }
    
    __ja_free_val(target->u.array.items[index]);
    target->u.array.items[index] = value;
}

void ja_set_obj_at(ja_val *target, const char *key, ja_val *value) {
    if (!target || !value) {
        JA_LOG_ERROR("NULL pointers passed to ja_set_arr_at().");
        return;
    }

    if (target->type != JA_TYPE_OBJECT) {
        JA_LOG_ERROR("Can't use ja_set_obj_at() on non-object value");
        return;
    }

    for (size_t i = 0; i < target->u.object.size; i++) {
        if (strcmp(target->u.object.pairs[i].key, key) == 0) {
            ja_free_val(&target->u.object.pairs[i].value_ptr);
            target->u.object.pairs[i].value_ptr = value;
            return;
        }
    }

    size_t new_size = target->u.object.size + 1;
    
    ja_pair *new_pairs = realloc(target->u.object.pairs, new_size * sizeof(ja_pair));
    if (!new_pairs) {
        JA_MEM_ERROR();
        return;
    }

    target->u.object.pairs = new_pairs;
    
    target->u.object.pairs[target->u.object.size].key = strdup(key);
    target->u.object.pairs[target->u.object.size].value_ptr = value;

    target->u.object.size = new_size;
}

int ja_get_int(ja_val *origin) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve number from NULL pointer.");
        return 0;
    }

    if (origin->type != JA_TYPE_DOUBLE && origin->type != JA_TYPE_INT) {
        JA_LOG_ERROR("Can't use ja_get_int() in non-number value.");
        return 0;
    }

    return origin->u.number.as_int;
}

double ja_get_double(ja_val *origin) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve number from NULL pointer.");
        return 0;
    }

    if (origin->type != JA_TYPE_DOUBLE && origin->type != JA_TYPE_INT) {
        JA_LOG_ERROR("Can't use ja_get_double() in non-number value.");
        return 0;
    }

    return origin->u.number.as_double;
}

char* ja_get_str(ja_val *origin) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve string from NULL pointer.");
        return NULL;
    }

    if (origin->type != JA_TYPE_STRING) {
        JA_LOG_ERROR("Can't use ja_get_str() in non-string value.");
        return NULL;
    }

    return origin->u.string;
}

bool ja_get_bool(ja_val *origin) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve boolean from NULL pointer.");
        return false;
    }
    
    if (origin->type != JA_TYPE_BOOL) {
        JA_LOG_ERROR("Can't use ja_get_bool() in non-boolean value.");
        return 0;
    }

    return origin->u.boolean;
}

bool ja_is_null(ja_val *origin) {
    if (!origin) {
        JA_LOG_WARN("True value due to null pointer, not actual ja_val NULL type.");
        return true;
    }
    return origin->type == JA_TYPE_NULL;
}

ja_val *ja_get_arr_at(ja_val *origin, size_t index) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve value from NULL pointer.");
        return NULL;
    }

    if (origin->type != JA_TYPE_ARRAY) {
        JA_LOG_ERROR("Can't use ja_get_arr_at() in non-array value");
        return NULL;
    }

    if (index >= origin->u.array.size) {
        JA_LOG_ERROR("Index out of bounds (index=%zu, size=%zu).", index, origin->u.array.size);
        return NULL;
    }

    return origin->u.array.items[index];
}

ja_val *ja_get_obj_at(ja_val *origin, const char *key) {
    if (!origin) {
        JA_LOG_ERROR("Can't retrieve value from NULL pointer.");
        return NULL;
    }

    if (origin->type != JA_TYPE_OBJECT) {
        JA_LOG_ERROR("Can't use ja_get_obj_at() in non-object value.");
        return NULL;
    }
    
    for (size_t i = 0; i < origin->u.object.size; i++) {
        if (strcmp(origin->u.object.pairs[i].key, key) == 0) {
            return origin->u.object.pairs[i].value_ptr;
        }
    }
    
    JA_LOG_ERROR("Key \"%s\" not found.", key);
    return NULL;
}

void ja_arr_append(ja_val *target, ja_val *content_to_add) {
    if (!target) {
        JA_LOG_ERROR("ja_arr_append() called with NULL target.");
        return;
    }
    
    if (!content_to_add) {
        JA_LOG_ERROR("ja_arr_append() called with NULL content_to_add.");
        return;
    }

    if (target->type != JA_TYPE_ARRAY) {
        JA_LOG_ERROR("Can't use ja_arr_append() on non-array value.");
        return;
    }

    size_t new_size = target->u.array.size + 1;

    ja_val **new_items = realloc(target->u.array.items, sizeof(ja_val*) * new_size);
    if (!new_items) {
        JA_MEM_ERROR();
        return;
    }

    target->u.array.items = new_items;
    target->u.array.items[target->u.array.size] = content_to_add;
    target->u.array.size = new_size;
}

void ja_arr_remove_at(ja_val *target, size_t index) {
    if (!target) {
        JA_LOG_ERROR("Can't use ja_arr_remove_at() on NULL pointer.");
        return;
    }

    if (target->type != JA_TYPE_ARRAY) {
        JA_LOG_ERROR("Can't use ja_arr_remove_at() on non-array value.");
        return;
    }

    if (index >= target->u.array.size) {
        JA_LOG_ERROR("Index out of bounds (index=%zu, size=%zu).", index, target->u.array.size);
        return;
    }

    ja_free_val(&target->u.array.items[index]);
    target->u.array.items[index] = NULL;

    if (index < target->u.array.size - 1) {
        memmove(&target->u.array.items[index],
                &target->u.array.items[index + 1],
                (target->u.array.size - index - 1) * sizeof(ja_val*));
    }

    size_t new_size = target->u.array.size - 1;

    if (new_size > 0) {
        ja_val **new_items = realloc(target->u.array.items, sizeof(ja_val*) * new_size);
        if (!new_items) {
            JA_MEM_ERROR();
            return;
        }
        target->u.array.items = new_items;
    } else {
        free(target->u.array.items);
        target->u.array.items = NULL;
    }

    target->u.array.size = new_size;
}

void ja_obj_remove_at(ja_val *target, const char *key) {
    if (!target) {
        JA_LOG_ERROR("ja_obj_remove_at() called with NULL target.");
    }
    
    if (!key) {
        JA_LOG_ERROR("ja_obj_remove_at() called with NULL key.");
        return;
    }
    
    if (target->type != JA_TYPE_OBJECT) {
        JA_LOG_ERROR("Can't use ja_obj_remove_at() in non-object value.");
        return;
    }

    size_t index = target->u.object.size;
    for (size_t i = 0; i < target->u.object.size; i++) {
        if (strcmp(target->u.object.pairs[i].key, key) == 0) {
            free(target->u.object.pairs[i].key);
            __ja_free_val(target->u.object.pairs[i].value_ptr);
            index = i;
            break;
        }
    }

    if (index == target->u.object.size) {
        JA_LOG_ERROR("Key not found: %s", key);
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

ja_val* ja_convert_to(ja_val *target, ja_type new_type) {
    if (!target) {
        JA_LOG_ERROR("NULL pointer passed to ja_convert_to().");
        return target;
    }
    if (target->type != new_type)
        switch (new_type) { 
        case JA_TYPE_INT:
        case JA_TYPE_DOUBLE:
            __ja_convert_to_num(target);
            break;
        case JA_TYPE_STRING:
            __ja_convert_to_str(target);
            break;
        case JA_TYPE_BOOL:
            __ja_convert_to_bool(target);
            break;
        case JA_TYPE_ARRAY:
            __ja_convert_to_arr(target);
            break;
        case JA_TYPE_OBJECT:
            __ja_convert_to_obj(target);
            break;
        case JA_TYPE_NULL:
            __ja_convert_to_null(target);
            break;
        default:
            JA_LOG_ERROR("Invalid type conversion.");
            break;
        }
    else 
        JA_LOG_WARN("Convertion with no effect: (%s -> %s)", __ja_type_enum_to_str(target->type), __ja_type_enum_to_str(new_type));
    return target;
}

void __ja_convert_to_num(ja_val *target) {
    if (!target) return;

    switch (target->type) {
    case JA_TYPE_INT: 
    case JA_TYPE_DOUBLE: 
        break;

    case JA_TYPE_STRING: {
        char *end_ptr;
        double number_value = strtold(target->u.string, &end_ptr);

        if (end_ptr == target->u.string) {
            JA_LOG_ERROR("Invalid number string: \"%s\"", target->u.string);
            return;
        }

        while (isspace((unsigned char)*end_ptr)) end_ptr++;

        if (*end_ptr != '\0') {
            JA_LOG_ERROR("Partial conversion: \"%s\" is not a pure number.", target->u.string);
            return;
        }

        free(target->u.string);

        target->u.number.as_int = (int)number_value;
        target->u.number.as_double = number_value;

        double diff = fabs(number_value - (double)((int)number_value));
        target->type = (diff < 1e-9) ? JA_TYPE_INT : JA_TYPE_DOUBLE;

        break;
    }

    case JA_TYPE_BOOL: {
        target->u.number.as_int = target->u.boolean ? 1 : 0;
        target->u.number.as_double = target->u.boolean ? 1.0 : 0.0;
        target->type = JA_TYPE_INT;
        break;
    }

    case JA_TYPE_ARRAY:
    case JA_TYPE_OBJECT: {
        size_t temp = ja_size_of(target);
        __ja_free_val(target);
        target->u.number.as_int = (int) temp;
        target->u.number.as_double = (double) temp;
        target->type = JA_TYPE_INT;
        break;
    }
    
    case JA_TYPE_NULL: {
        target->u.number.as_int = 0;
        target->u.number.as_double = 0.0;
        target->type = JA_TYPE_INT;
        break;
    }

    default: {
        JA_LOG_ERROR("Invalid type for conversion to number.");
        break;
    }
    }
}

void __ja_convert_to_str(ja_val *target) {
    if (!target) return;

    ja_val temp = *target;
    char* str = NULL;

    switch (temp.type) {
    case JA_TYPE_INT:
    case JA_TYPE_DOUBLE:
        str = ja_stringify(&temp);
        break;
    case JA_TYPE_BOOL:
        str = strdup(temp.u.boolean ? "true" : "false");
        break;
    case JA_TYPE_ARRAY:
    case JA_TYPE_OBJECT:
        str = ja_stringify(&temp);
        break;
    case JA_TYPE_NULL:
        str = strdup("null");
        break;
    case JA_TYPE_STRING:
        return;
    default:
        JA_LOG_ERROR("Invalid type for conversion to string.");
        return;
    }

    if (!str) return;

    if (target->type == JA_TYPE_STRING && target->u.string)
        free(target->u.string);

    target->u.string = str;
    target->type = JA_TYPE_STRING;
}

void __ja_convert_to_bool(ja_val *target) {
    if (!target) return;

    switch (target->type) {
    case JA_TYPE_INT:
    case JA_TYPE_DOUBLE: {
        target->u.boolean = target->u.number.as_int != 0;
        target->type = JA_TYPE_BOOL;
        break;
    }

    case JA_TYPE_STRING: {
        bool temp = (bool)strlen(target->u.string) > 0;
        __ja_free_val(target);
        target->u.boolean = temp;
        target->type = JA_TYPE_BOOL;
        break;
    }

    case JA_TYPE_BOOL:
        break;

    case JA_TYPE_ARRAY:
    case JA_TYPE_OBJECT: {
        target->u.boolean = target->u.array.size > 0 || target->u.object.size > 0;
        target->type = JA_TYPE_BOOL;
        __ja_free_val(target);
        break;
    }

    case JA_TYPE_NULL: {
        target->u.boolean = false;
        target->type = JA_TYPE_BOOL;
        break;
    }

    default:
        JA_LOG_ERROR("Invalid type for conversion to boolean.");
        break;
    }
}

void __ja_convert_to_arr(ja_val *target) {
    if (!target) return;

    switch (target->type) {
    case JA_TYPE_INT:
    case JA_TYPE_DOUBLE: {
        ja_val *num_cpy = ja_copy(target);
        ja_val *new_arr = ja_new_arr();
        if (!new_arr || !num_cpy) {
            return;
        }

        __ja_free_val(target);
        ja_arr_append(new_arr, num_cpy);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_TYPE_ARRAY;
        free(new_arr);
        return;
    }

    case JA_TYPE_STRING: {
        ja_val *str_cpy = ja_copy(target);
        ja_val *new_arr = ja_new_arr();
        if (!new_arr || !str_cpy) {
            JA_MEM_ERROR();
            return;
        }

        size_t str_length = strlen(target->u.string);
        for (size_t i = 0; i < str_length; i++) {
            char sub_str[2] = {0};
            sub_str[0] = str_cpy->u.string[i];
            ja_arr_append(new_arr, ja_new_str(sub_str));
        }

        __ja_free_val(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_TYPE_ARRAY;
        free(new_arr);
        return;
    }

    case JA_TYPE_BOOL: {
        ja_val *bool_cpy = ja_copy(target);
        ja_val *new_arr = ja_new_arr();
        if (!new_arr || !bool_cpy) {
            return;
        }

        __ja_free_val(target);
        ja_arr_append(new_arr, bool_cpy);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_TYPE_ARRAY;
        free(new_arr);
        return;
    }

    case JA_TYPE_NULL: {
        ja_val *new_arr = ja_new_arr();
        if (!new_arr) {
            return;
        }

        __ja_free_val(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_TYPE_ARRAY;
        free(new_arr);
        return;
    }

    case JA_TYPE_ARRAY:
        break;

    case JA_TYPE_OBJECT: {
        ja_val *new_arr = ja_new_arr();
        if (!new_arr) {
            return;
        }

        for (size_t i = 0; i < target->u.object.size; i++) {
            ja_arr_append(new_arr, ja_copy(target->u.object.pairs[i].value_ptr));
        }

        __ja_free_val(target);
        target->u.array.items = new_arr->u.array.items;
        target->u.array.size = new_arr->u.array.size;
        target->type = JA_TYPE_ARRAY;
        free(new_arr);
        break;
    }

    default: {
        JA_LOG_ERROR("Invalid type for conversion to array.");
        break;
    }
    }
}

void __ja_convert_to_obj(ja_val *target) {
    if (!target) return;

    switch (target->type) {
    case JA_TYPE_INT:
    case JA_TYPE_DOUBLE:
    case JA_TYPE_STRING:
    case JA_TYPE_BOOL: {
        ja_val *new_obj = ja_new_obj();
        ja_val *var_cpy = ja_copy(target);
        if (!new_obj || !var_cpy) {
            return;
        }

        ja_set_obj_at(new_obj, ja_str_type_of(target), var_cpy);
        __ja_free_val(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_TYPE_OBJECT;
        free(new_obj);
        break;
    }

    case JA_TYPE_NULL: {
        ja_val *new_obj = ja_new_obj();
        if (!new_obj) {
            return;
        }

        __ja_free_val(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_TYPE_OBJECT;
        free(new_obj);
        break;
    }

    case JA_TYPE_ARRAY: {
        ja_val *new_obj = ja_new_obj();
        ja_val *arr_cpy = ja_copy(target);
        if (!new_obj || !arr_cpy) {
            return;
        }

        for (size_t i = 0; i < target->u.array.size; i++) {
            char idx_buffer[32];
            sprintf(idx_buffer, "%zu", i);
            ja_set_obj_at(new_obj, idx_buffer, ja_copy(ja_get_arr_at(target, i)));
        }
        
        __ja_free_val(target);
        target->u.object.pairs = new_obj->u.object.pairs;
        target->u.object.size = new_obj->u.object.size;
        target->type = JA_TYPE_OBJECT;
        free(new_obj);
        break;
    }

    case JA_TYPE_OBJECT:
        break;

    default: {
        JA_LOG_ERROR("Invalid type for conversion to array.");
        break;
    }
    }
}

void __ja_convert_to_null(ja_val *target) {
    if (!target) return;
        
    __ja_free_val(target);
    memset(&target->u, 0, sizeof(target->u));

    target->type = JA_TYPE_NULL;
}

char* ja_stringify(ja_val* value) {
    if (!value) {
        JA_LOG_ERROR("NULL value. Can't convert to string.");
        return NULL;
    }

    switch (value->type) {
    case JA_TYPE_INT: {
        int needed = snprintf(NULL, 0, "%d", value->u.number.as_int);
        char* str = malloc(needed + 1);
        if (!str) {
            JA_MEM_ERROR();
            return NULL;
        }
        snprintf(str, needed + 1, "%d", value->u.number.as_int);
        return str;
    }

    case JA_TYPE_DOUBLE: {
        int needed;
        char* str;
        double val = value->u.number.as_double;

        if (fabs(val - round(val)) < 1e-12) {
            needed = snprintf(NULL, 0, "%.0f", val);
            str = malloc(needed + 1);
            if (!str) { JA_MEM_ERROR(); return NULL; }
            snprintf(str, needed + 1, "%.0f", val);
        } else {
            needed = snprintf(NULL, 0, "%.15g", val);
            str = malloc(needed + 1);
            if (!str) { JA_MEM_ERROR(); return NULL; }
            snprintf(str, needed + 1, "%.15g", val);
        }
        return str;
    }

    case JA_TYPE_STRING: {
        if (!value->u.string) return strdup("\"\"");
        
        const char *src = value->u.string;
        size_t len = strlen(src);
        size_t total = len+3;

        char *out = malloc(total);
        if (!out) {
            JA_MEM_ERROR();
            return NULL;
        }

        out[0] = '"';
        memcpy(out + 1, src, len);
        out[len + 1] = '"';
        out[len + 2] = '\0';

        return out;
    }

    case JA_TYPE_BOOL: {
        return strdup(value->u.boolean ? "true" : "false");
    }

    case JA_TYPE_ARRAY: {
        if (value->u.array.size == 0) return strdup("[]");

        char** elements = malloc(value->u.array.size * sizeof(char*));
        if (!elements) {
            JA_MEM_ERROR();
            return NULL;
        }

        size_t total_length = 2;
        for (size_t i = 0; i < value->u.array.size; i++) {
            elements[i] = ja_stringify(value->u.array.items[i]);
            total_length += strlen(elements[i]) + 1;
        }

        char* str = malloc(total_length * sizeof(char));
        if (!str) {
            JA_MEM_ERROR();
            for (size_t i = 0; i < value->u.array.size; i++) free(elements[i]);
            free(elements); 
            return NULL;
        }

        char* ptr = str;
        *ptr++ = '[';

        for (size_t i = 0; i < value->u.array.size; i++) {
            size_t len = strlen(elements[i]);
            memcpy(ptr, elements[i], len);
            ptr += len;
            if (i < value->u.array.size - 1) {
                *ptr++ = ',';
            }
            free(elements[i]);
        }

        *ptr++ = ']';
        *ptr = '\0';
        
        free(elements);
        return str;
    }

    case JA_TYPE_OBJECT: {
        if (value->u.object.size == 0) return strdup("{}");

        char** pairs = malloc(value->u.object.size * sizeof(char*));
        if (!pairs) {
            JA_MEM_ERROR();
            return NULL;
        }

        size_t total_length = 2;
        for (size_t i = 0; i < value->u.object.size; i++) {
            char* value_json = ja_stringify(value->u.object.pairs[i].value_ptr);
            if (!value_json) {
                for (size_t k = 0; k < i; k++) free(pairs[k]);
                free(pairs);
                return NULL;
            }

            size_t key_len = strlen(value->u.object.pairs[i].key);
            size_t val_len = strlen(value_json);

            pairs[i] = malloc(key_len + val_len + 4);
            if (!pairs[i]) {
                JA_MEM_ERROR();
                free(value_json);
                for (size_t k = 0; k < i; k++) free(pairs[k]);
                free(pairs);
                return NULL;
            }

            sprintf(pairs[i], "\"%s\":%s", value->u.object.pairs[i].key, value_json);
            total_length += strlen(pairs[i]) + 1; // +1 for comma
            free(value_json);
        }

        char* str = malloc(total_length);
        if (!str) {
            JA_MEM_ERROR();
            for (size_t i = 0; i < value->u.object.size; i++) free(pairs[i]);
            free(pairs);
            return NULL;
        }

        char* ptr = str;
        *ptr++ = '{';

        for (size_t i = 0; i < value->u.object.size; i++) {
            size_t len = strlen(pairs[i]);
            memcpy(ptr, pairs[i], len);
            ptr += len;
            if (i < value->u.object.size - 1) {
                *ptr++ = ',';
            }
            free(pairs[i]);
        }

        *ptr++ = '}';
        *ptr = '\0';

        free(pairs);
        return str;
    }

    case JA_TYPE_NULL:
        return strdup("null");

    default:
        JA_LOG_ERROR("Can't retrieve string from this type.");
        return NULL;
    }
}

void ja_print(ja_val* value) {
    if (!value) {
        JA_LOG_ERROR("NULL value. Can't print.");
        return;
    }

    char* str = ja_stringify(value);
    if (!str) {
        JA_PROPAGATE_ERROR("ja_print");
        return;
    }

    fprintf(stdout, "%s\n", str);
    free(str);
}

ja_val *ja_parse(const char *json_str) {
    if (!json_str) {
        JA_LOG_ERROR("NULL string passed to ja_parse().");
        return NULL;
    }
    ja_val *value = __ja_parse(json_str, NULL);
    if (!value) {
        JA_PROPAGATE_ERROR("ja_parse");
        return NULL;
    }
    return value;
}

ja_val *__ja_parse(const char *json_str, int *chars_consumed) {
    if (!json_str) {
        JA_LOG_ERROR("Can't parse NULL string.");
        return NULL;
    }

    if (*json_str == '\0') {
        JA_LOG_ERROR("Empty string.");
        return NULL;
    }

    __ja_jump_whitespaces(&json_str, chars_consumed);

    switch (*json_str) {
    case '{':
        return __ja_parse_object(json_str, chars_consumed);
    case '[':
        return __ja_parse_array(json_str, chars_consumed);
    case '"':
        return __ja_parse_string(json_str, chars_consumed);
    case 't':
    case 'f':
        return __ja_parse_bool(json_str, chars_consumed);
    case 'n':
        return __ja_parse_null(json_str, chars_consumed);
    default:
        if (isdigit(*json_str) || *json_str == '-') {
            return __ja_parse_number(json_str, chars_consumed);
        } else {
            JA_LOG_ERROR("Invalid character: %c", *json_str);
            return NULL;
        }
    }
}

ja_val *__ja_parse_number(const char *json_str, int *chars_consumed) {
    const char *p = json_str;
    int dots_count = 0, e_count = 0;

    if (*p == '-' || *p == '+') {
        p++;
        if (chars_consumed) (*chars_consumed)++;
    }

    while (*p && (isdigit((unsigned char)*p) || *p == '.' || *p == 'e' || *p == 'E' || *p == '+' || *p == '-')) {
        if (*p == '.') {
            dots_count++;
            if (dots_count > 1) {
                JA_LOG_ERROR("Multiple decimal points in number.");
                return NULL;
            }
        } else if (*p == 'e' || *p == 'E') {
            e_count++;
            if (e_count > 1) {
                JA_LOG_ERROR("Multiple exponent indicators in number.");
                return NULL;
            }
        }
        p++;
        if (chars_consumed) (*chars_consumed)++;
    }

    char *end_ptr;
    double number_value = strtold(json_str, &end_ptr);

    if (end_ptr == json_str) {
        JA_LOG_ERROR("Error while parsing number.");
        return NULL;
    }

    return ja_new_num(number_value);
}

ja_val *__ja_parse_string(const char *json_str, int *chars_consumed) {
    const char *p = json_str;
    const char *start = ++p;
    size_t length = 0;
    int consumed = 1;

    while (*p && *p != '"') {
        if (*p == '\\') {
            p++;
            consumed++;
            if (*p == 'u') {
                p++;
                consumed++;
                for (int i = 0; i < 4 && *p; i++, p++, consumed++);
            } else if (*p) {
                p++;
                consumed++;
            }
        } else {
            p++;
            consumed++;
        }
    }

    if (*p != '"') {
        JA_LOG_ERROR("Unmatched quotes in string.");
        return NULL;
    }

    length = p - start;
    char temp_str[length + 1];

    strncpy(temp_str, start, length);
    temp_str[length] = '\0';
    ja_val *jav = ja_new_str(temp_str);
    if (!jav) {
        return NULL;
    }

    consumed++;
    if (chars_consumed) *chars_consumed += consumed;
    return jav;
}

ja_val *__ja_parse_bool(const char *json_str, int *chars_consumed) {
    if (strncmp(json_str, "true", 4) == 0) {
        if (chars_consumed) (*chars_consumed) += 4;
        return ja_new_bool(true);
    } else if (strncmp(json_str, "false", 5) == 0) {
        if (chars_consumed) (*chars_consumed) += 5;
        return ja_new_bool(false);
    }
    JA_LOG_ERROR("Invalid boolean value.");
    return NULL;
}

ja_val *__ja_parse_array(const char *json_str, int *chars_consumed) {
    ja_val *jav = ja_new_arr();
    if (!jav) return NULL;

    json_str++;
    if (chars_consumed) (*chars_consumed)++; // Count the '['

    while (*json_str) {
        __ja_jump_whitespaces(&json_str, chars_consumed);

        if (*json_str == ']') {
            json_str++;
            if (chars_consumed) (*chars_consumed)++;
            return jav;
        }

        int inner_chars_consumed = 0;
        ja_val *value = __ja_parse(json_str, &inner_chars_consumed);
        if (!value) {
            ja_free_val(&jav);
            return NULL;
        }

        ja_arr_append(jav, value);
        json_str += inner_chars_consumed;
        if (chars_consumed) (*chars_consumed) += inner_chars_consumed;

        __ja_jump_whitespaces(&json_str, chars_consumed);

        if (*json_str == ',') {
            json_str++;
            if (chars_consumed) (*chars_consumed)++; // Count the ','
        } else if (*json_str != ']') {
            JA_LOG_ERROR("Invalid character in array: %c", *json_str);
            ja_free_val(&jav);
            return NULL;
        }
    } 

    JA_LOG_ERROR("Unmatched brackets in array.");
    ja_free_val(&jav);
    return NULL;
}

ja_val *__ja_parse_object(const char *json_str, int *chars_consumed) {
    ja_val *jav = ja_new_obj();
    if (!jav) return NULL;

    ja_val *key_container = NULL;
    ja_val *value = NULL;

    json_str++; // Skip '{'
    if (chars_consumed) (*chars_consumed)++;

    while (*json_str) {
        key_container = NULL;
        value = NULL;
        __ja_jump_whitespaces(&json_str, chars_consumed);

        if (*json_str == '}') {
            json_str++;
            if (chars_consumed) (*chars_consumed)++;
            return jav;
        }

        int inner_chars_consumed = 0;
        if (*json_str != '"') {
            if (*json_str == '\0')
                JA_LOG_ERROR("Unexpected end of file.");
            else
                JA_LOG_ERROR("Invalid character in object key: '%c'", *json_str);
            ja_free_val(&key_container);
            ja_free_val(&value);
            ja_free_val(&jav);
            return NULL;
        }

        key_container = __ja_parse_string(json_str, &inner_chars_consumed);
        if (!key_container) {
            ja_free_val(&jav);
            return NULL;
        }
        char *key = key_container->u.string;

        json_str += inner_chars_consumed;
        if (chars_consumed) (*chars_consumed) += inner_chars_consumed;

        __ja_jump_whitespaces(&json_str, chars_consumed);

        if (*json_str != ':') {
            JA_LOG_ERROR("Missing colon after key: %s", key);
            ja_free_val(&jav);
            ja_free_val(&key_container);
            return NULL;
        }

        json_str++;
        if (chars_consumed) (*chars_consumed)++;
        __ja_jump_whitespaces(&json_str, chars_consumed);

        int value_chars_consumed = 0;
        value = __ja_parse(json_str, &value_chars_consumed);
        if (!value) {
            ja_free_val(&jav);
            ja_free_val(&key_container);
            return NULL;
        }

        json_str += value_chars_consumed;
        if (chars_consumed) (*chars_consumed) += value_chars_consumed;
        __ja_jump_whitespaces(&json_str, chars_consumed);

        if (*json_str == ',') {
            json_str++;
            if (chars_consumed) (*chars_consumed)++;
        } else if (*json_str != '}') {
            JA_LOG_ERROR("Invalid character in object: '%c'", *json_str);
            ja_free_val(&jav);
            ja_free_val(&key_container);
            ja_free_val(&value);
            return NULL;
        }

        ja_set_obj_at(jav, key, value);
        ja_free_val(&key_container);
        key_container = NULL;
        value = NULL;
    }

    JA_LOG_ERROR("Unmatched brackets in object.");
    ja_free_val(&key_container);
    ja_free_val(&value);
    ja_free_val(&jav);
    return NULL;
}

ja_val *__ja_parse_null(const char *json_str, int *chars_consumed) {
    if (strncmp(json_str, "null", 4) == 0) {
        if (chars_consumed) (*chars_consumed) += 4;
        return ja_new_null();
    }
    JA_LOG_ERROR("Invalid null value.");
    return NULL;
}

void __ja_jump_whitespaces(const char **str_ptr, int *chars_consumed) {
    while (**str_ptr && isspace(**str_ptr)) {
        (*str_ptr)++;
        if (chars_consumed) (*chars_consumed)++;
    }
}

int ja_enum_type_of(ja_val *value) {
    if (!value) {
        JA_LOG_ERROR("Can't retrieve type of a NULL pointer.");
        return 0;
    }

    switch (value->type) {
    case JA_TYPE_INT: 
    case JA_TYPE_DOUBLE: 
    case JA_TYPE_STRING: 
    case JA_TYPE_BOOL: 
    case JA_TYPE_ARRAY: 
    case JA_TYPE_OBJECT: 
    case JA_TYPE_NULL: 
        return (int)value->type;
    default:
        return -1;
    }
}

const char *ja_str_type_of(ja_val *value) {
    if (!value) {
        JA_LOG_ERROR("Can't retrieve type of a NULL pointer.");
        return NULL;
    }

    return __ja_type_enum_to_str(value->type);
}

const char *__ja_type_enum_to_str(ja_type type) {
    switch (type) {
    case JA_TYPE_INT: return "Number";
    case JA_TYPE_DOUBLE: return "Number";
    case JA_TYPE_STRING: return "String";
    case JA_TYPE_BOOL: return "Bool";
    case JA_TYPE_ARRAY: return "Array";
    case JA_TYPE_OBJECT: return "Object";
    case JA_TYPE_NULL: return "Null";
    default: return "Unknown";
    }
}

size_t ja_size_of(ja_val *value) {
    if (!value) {
        JA_LOG_ERROR("Can't access size of NULL value.");
        return 0;
    }

    switch (value->type) {
    case JA_TYPE_STRING: return strlen(value->u.string);
    case JA_TYPE_ARRAY: return value->u.array.size;
    case JA_TYPE_OBJECT: return value->u.object.size;
    default:
        JA_LOG_ERROR("Can't use ja_size_of() for this type (%s).", ja_str_type_of(value));
        return 0;
    }
}

void ja_free_val(ja_val **val_ptr) {
    if (!val_ptr || !(*val_ptr)) return;
    
    __ja_free_val(*val_ptr);
    free(*val_ptr);
    *val_ptr = NULL;
}

void __ja_free_val(ja_val *value) {
    if (!value) return;

    switch (value->type) {
        case JA_TYPE_STRING:
            free(value->u.string);
            value->u.string = NULL;
            break;
        case JA_TYPE_ARRAY:
            for (size_t i = 0; i < value->u.array.size; i++) {
                ja_free_val(&value->u.array.items[i]);
            }
            free(value->u.array.items);
            value->u.array.items = NULL;
            break;
        case JA_TYPE_OBJECT:
            for (size_t i = 0; i < value->u.object.size; i++) {
                free(value->u.object.pairs[i].key);
                value->u.object.pairs[i].key = NULL;
                ja_free_val(&value->u.object.pairs[i].value_ptr);
            }
            free(value->u.object.pairs);
            value->u.object.pairs = NULL;
            break;
        default:
            break;
    }
}

ja_json* ja_json_init() {
    ja_json *ja_json_object = malloc(sizeof(ja_json));
    if (!ja_json_object) {
        JA_MEM_ERROR();
        return NULL;
    }

    ja_json_object->json_str = NULL;
    ja_json_object->content = NULL;

    return ja_json_object;
}

bool ja_read_json(ja_json *ja_json_object, const char *filename) {
    if (!ja_json_object) {
        JA_LOG_ERROR("ja_read_json() received NULL ja_json.");
        return false;
    }

    if (!filename) {
        JA_LOG_ERROR("Filename not specified, can't open file");
        return false;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        JA_LOG_ERROR("Error while opening file: %s", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0) {
        JA_LOG_ERROR("File is empty or unreadable: %s", filename);
        fclose(file);
        return false;
    }

    char* buffer = malloc(file_size + 1);
    if (!buffer) {
        JA_MEM_ERROR();
        fclose(file);
        return false;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if ((long)bytes_read != file_size) {
        JA_LOG_ERROR("Expected to read %ld bytes but only read %zu.", file_size, bytes_read);
        free(buffer);
        return false;
    }

    buffer[file_size] = '\0';

    ja_val *parsed = ja_parse(buffer);
    if (!parsed) {
        JA_PROPAGATE_ERROR("ja_read_json");
        free(buffer);
        ja_json_object->json_str = NULL;
        ja_json_object->content = NULL;
        return false;
    }

    ja_json_object->json_str = buffer;
    ja_json_object->content = parsed;

    return true;
}

bool ja_write_json(ja_json *ja_json_object, const char *filename) {
    if (!ja_json_object) {
        JA_LOG_ERROR("ja_sync_json() called with NULL pointer.");
        return false;
    }

    if (!filename) {
        JA_LOG_ERROR("Can't save file with NULL filename.");
        return false;
    }

    ja_sync_json(ja_json_object);

    if (!ja_json_object->json_str) {
        JA_LOG_ERROR("No JSON string to write.");
        return false;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        JA_LOG_ERROR("Error opening file: %s", filename);
        return false;
    }

    fputs(ja_json_object->json_str, file);
    fclose(file);

    return true;
}

void ja_sync_json(ja_json *ja_json_object) {
    if (!ja_json_object) {
        JA_LOG_ERROR("ja_sync_json() called with NULL pointer.");
        return;
    }

    if (!ja_json_object->content) {
        JA_LOG_ERROR("No content to sync.");
        return;
    }

    char *json_str = ja_stringify(ja_json_object->content);
    if (!json_str) {
        JA_PROPAGATE_ERROR("ja_write_json");
        return;
    }

    free(ja_json_object->json_str);
    ja_json_object->json_str = json_str;
}

void ja_json_end(ja_json *ja_json_object) {
    if (!ja_json_object) {
        JA_LOG_ERROR("Can't end NULL ja_json.");
        return;
    }
    
    if (ja_json_object->json_str) {
        free(ja_json_object->json_str);
        ja_json_object->json_str = NULL;
    }

    if (ja_json_object->content) {
        ja_free_val(&ja_json_object->content);
        ja_json_object->content = NULL;
    }

    free(ja_json_object);
}
