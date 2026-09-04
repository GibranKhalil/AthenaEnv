#ifndef PAD_HW_H
#define PAD_HW_H

#include <tamtypes.h>
#include <libpad.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATH_PAD_MAX_PORTS 2
#define ATH_PAD_MAX_SLOTS 4

typedef struct {
    int connected;
    int type;
    u32 btns;
    u32 old_btns;
    int lx, ly, rx, ry;
    int old_lx, old_ly, old_rx, old_ry;
    struct padButtonStatus raw;
} ath_pad_state_t;

void pad_hw_init(void);
void pad_hw_shutdown(void);

void pad_hw_poll_all(void);
void pad_hw_poll(int port, int slot);

int pad_hw_wait_ready(int port, int slot);
int pad_hw_initialize(int port, int slot);
void pad_hw_check_reconnection(int port, int slot);
int pad_hw_read(int port, int slot, struct padButtonStatus *buttons);

int pad_hw_set_actuators(int port, int slot, bool small_on, u8 large_intensity);

int pad_hw_get_state(int port, int slot);
int pad_hw_get_type(int port, int slot);

int pad_hw_get_port_count(void);
int pad_hw_get_slot_count(int port);

const ath_pad_state_t *pad_hw_get_pad_state(int port, int slot);

int pad_hw_get_pressure(int port, int slot, u32 button_bit);

#ifdef __cplusplus
}
#endif

#endif /* PAD_HW_H */