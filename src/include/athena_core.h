#ifndef ATHENA_CORE_H
#define ATHENA_CORE_H

#include <stdbool.h>

/* Shared runtime state (no QuickJS dependency). */
extern char boot_path[255];
extern char default_script[128];
extern bool boot_logo;
extern bool dark_mode;

void init_bootlogo(void);
bool bootlogo_finished(void);
void set_default_script(const char *path);
const char *athena_get_default_script(void);

#endif /* ATHENA_CORE_H */
