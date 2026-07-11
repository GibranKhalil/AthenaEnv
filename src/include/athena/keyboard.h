#ifndef ATHENA_KEYBOARD_H
#define ATHENA_KEYBOARD_H

#ifdef ATHENA_KEYBOARD

int athena_keyboard_init(void);
int athena_keyboard_read(char *key);
int athena_keyboard_set_repeat_rate(unsigned int msec);
int athena_keyboard_set_blocking_mode(unsigned int mode);
int athena_keyboard_deinit(void);

#endif /* ATHENA_KEYBOARD */

#endif /* ATHENA_KEYBOARD_H */
