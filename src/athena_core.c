#include <string.h>

#include <athena_core.h>

char boot_path[255] = { 0 };
char default_script[128] = "main.js";
bool boot_logo = true;
bool dark_mode = true;

void set_default_script(const char *path) {
    strcpy(default_script, path);
    default_script[strlen(path)] = '\0';
}

const char *athena_get_default_script(void) {
    return default_script;
}
