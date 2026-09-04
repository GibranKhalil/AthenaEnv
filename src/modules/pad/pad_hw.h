#ifndef PAD_HW_H
#define PAD_HW_H

#include <tamtypes.h>
#include <libpad.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void pad_hw_init(void);
int pad_hw_wait_ready(int port, int slot);
int pad_hw_initialize(int port, int slot);
void pad_hw_check_reconnection(int port, int slot);
int pad_hw_read(int port, int slot, struct padButtonStatus *buttons);
int pad_hw_set_actuators(int port, int slot, u8 small, u8 large);
int pad_hw_get_state(int port, int slot);
int pad_hw_get_type(int port, int slot);

#ifdef __cplusplus
}
#endif

#endif /* PAD_HW_H */
