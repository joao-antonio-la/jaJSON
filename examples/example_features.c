#include "jajson.h"

/**
 * This example demonstrates several core features of jaJSON:
 * - Accessors (getters and setters)
 * - Conversions between types
 * - Stringification and parsing
 * - Type checking
 * - Sizing of arrays and objects
 *
 * We'll simulate a small "system configuration" object that could belong
 * to an application (e.g., server settings, logging, and limits).
 */

int main(void) {
    printf("=== jaJSON Features Example ===\n\n");

    // Create a configuration object
    ja_val *config = ja_new_set_obj(
        4,
        "app_name", ja_new_str("jaJSON Demo"),
        "max_connections", ja_new_num(128),
        "debug_mode", ja_new_bool(true),
        "servers", ja_new_set_arr(
            3,
            ja_new_str("main.server.com"),
            ja_new_str("backup.server.com"),
            ja_new_str("dev.server.local")
        )
    );

    printf("Initial configuration:\n");
    ja_print(config);
    printf("\n");

    // === Accessing values ===
    printf("Accessing configuration fields:\n");
    printf("Application: %s\n", ja_get_str(ja_get_obj_at(config, "app_name")));
    printf("Debug mode: %s\n", ja_get_bool(ja_get_obj_at(config, "debug_mode")) ? "enabled" : "disabled");
    printf("Server count: %zu\n\n", ja_size_of(ja_get_obj_at(config, "servers")));

    // === Modifying existing data ===
    printf("Toggling debug mode and increasing max connections...\n");
    ja_set_obj_at(config, "debug_mode", ja_new_bool(false));
    ja_set_obj_at(config, "max_connections", ja_new_num(256));

    printf("Adding a new test server...\n");
    ja_arr_append(ja_get_obj_at(config, "servers"), ja_new_str("test.server.local"));
    printf("New server count: %zu\n\n", ja_size_of(ja_get_obj_at(config, "servers")));

    // === Type checking and conversions ===
    printf("Checking types and conversions:\n");
    ja_val *max_conn_val = ja_get_obj_at(config, "max_connections");
    printf("max_connections type: %s\n", ja_str_type_of(max_conn_val));

    // Convert to string (useful for serialization or logs)
    ja_val *conn_str_val = ja_convert_to(ja_copy(max_conn_val), JA_TYPE_STRING);
    printf("max_connections as string: %s\n\n", ja_get_str(conn_str_val));

    // === Stringify to JSON ===
    printf("Stringifying the configuration into JSON text:\n");
    char *json_str = ja_stringify(config);
    printf("%s\n\n", json_str);

    // === Parsing back to a ja_val ===
    printf("Parsing the stringified data back into a ja_val:\n");
    ja_val *parsed_config = ja_parse(json_str);
    ja_print(parsed_config);
    printf("\n");

    // === Cleaning up ===
    free(json_str);
    ja_free_val(&config);
    ja_free_val(&conn_str_val);
    ja_free_val(&parsed_config);

    printf("All memory released. Exiting cleanly.\n");
    return 0;
}
