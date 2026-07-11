#ifdef ATHENA_KEYBOARD

#include <athena/keyboard.h>
#include <libkbd.h>

int athena_keyboard_init(void)
{
    return PS2KbdInit();
}

int athena_keyboard_read(char *key)
{
    return PS2KbdRead(key);
}

int athena_keyboard_set_repeat_rate(unsigned int msec)
{
    return PS2KbdSetRepeatRate(msec);
}

int athena_keyboard_set_blocking_mode(unsigned int mode)
{
    return PS2KbdSetBlockingMode(mode);
}

int athena_keyboard_deinit(void)
{
    return PS2KbdClose();
}

#endif /* ATHENA_KEYBOARD */
