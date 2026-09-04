#include "pad_hw.h"
#include <kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libpad.h>
#include <dbgprintf.h>

static char padBuf[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS][256] __attribute__((aligned(64)));
static char actAlign[6];

static ath_pad_state_t g_pad_states[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS];
static int pad_initialized[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS];
static int last_pad_state[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS];

static int pad_slot_opened[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS];

static int pad_hw_ensure_opened(int port, int slot) {
    if (pad_slot_opened[port][slot]) return 1;

    if (pad_open_retry_countdown[port][slot] > 0) {
        pad_open_retry_countdown[port][slot]--;
        return 0;
    }

    pad_open_retry_countdown[port][slot] = PAD_REOPEN_RETRY_INTERVAL;

    if (padPortOpen(port, slot, padBuf[port][slot]) != 0) {
        pad_slot_opened[port][slot] = 1;
        dbgprintf("[PadHW] Late-opened port %d slot %d\n", port, slot);
        return 1;
    }
    return 0;
}

int pad_hw_wait_ready(int port, int slot) {
    int state = padGetState(port, slot);
    int lastState = -1;
    char stateString[16];
    int timeout = 0;

    while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1) && (state != PAD_STATE_DISCONN)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            dbgprintf("[PadHW] Waiting: pad(%d,%d) is in state %s\n", port, slot, stateString);
        }
        lastState = state;
        state = padGetState(port, slot);
        timeout++;
        if (timeout > 1000) {
            dbgprintf("[PadHW] Pad timeout on port %d, slot %d\n", port, slot);
            return -1;
        }
    }
    return 0;
}

int pad_hw_initialize(int port, int slot) {
    if (pad_hw_wait_ready(port, slot) != 0) {
        dbgprintf("[PadHW] Pad not ready, skipping init for port %d, slot %d\n", port, slot);
        return 0;
    }

    int modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    dbgprintf("[PadHW] Port %d slot %d device modes: %d\n", port, slot, modes);

    if (modes == 0) {
        dbgprintf("[PadHW] Digital controller detected on port %d slot %d\n", port, slot);
        return 1;
    }

    int i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) {
            break;
        }
        i++;
    } while (i < modes);

    if (i >= modes) {
        dbgprintf("[PadHW] Not a DualShock controller on port %d slot %d\n", port, slot);
        return 1;
    }

    int ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        return 1;
    }

    dbgprintf("[PadHW] Enabling DualShock mode for port %d slot %d\n", port, slot);
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    pad_hw_wait_ready(port, slot);
    padEnterPressMode(port, slot);

    pad_hw_wait_ready(port, slot);
    int actuators = padInfoAct(port, slot, -1, 0);
    dbgprintf("[PadHW] Actuators found on port %d slot %d: %d\n", port, slot, actuators);

    if (actuators != 0) {
        actAlign[0] = 0;
        actAlign[1] = 1;
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        pad_hw_wait_ready(port, slot);
        padSetActAlign(port, slot, actAlign);
    }

    pad_hw_wait_ready(port, slot);
    return 1;
}

void pad_hw_check_reconnection(int port, int slot) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return;
    if (slot < 0 || slot >= ATH_PAD_MAX_SLOTS) return;

    int current_state = padGetState(port, slot);

    if (current_state == PAD_STATE_DISCONN) {
        pad_initialized[port][slot] = 0;
    }

    if ((current_state == PAD_STATE_STABLE || current_state == PAD_STATE_FINDCTP1) && !pad_initialized[port][slot]) {
        pad_hw_initialize(port, slot);
        pad_initialized[port][slot] = 1;
    }

    last_pad_state[port][slot] = current_state;
}

int pad_hw_read(int port, int slot, struct padButtonStatus *buttons) {
    if (!pad_hw_ensure_opened(port, slot)) return 0;

    pad_hw_check_reconnection(port, slot);

    int state = padGetState(port, slot);
    if (state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1) {
        return 0;
    }

    return padRead(port, slot, buttons);
}

int pad_hw_set_actuators(int port, int slot, bool small_on, u8 large_intensity) {
    if (!pad_hw_ensure_opened(port, slot)) return 0;

    pad_hw_check_reconnection(port, slot);

    int state = padGetState(port, slot);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
        char act[6] = { (char)(small_on ? 1 : 0), (char)large_intensity, 0, 0, 0, 0 };
        return padSetActDirect(port, slot, act);
    }
    return 0;
}

int pad_hw_get_state(int port, int slot) {
    return padGetState(port, slot);
}

int pad_hw_get_type(int port, int slot) {
    return padInfoMode(port, slot, PAD_MODECUREXID, 0);
}

int pad_hw_get_port_count(void) {
    int n = padGetPortMax();
    if (n > ATH_PAD_MAX_PORTS) n = ATH_PAD_MAX_PORTS;
    if (n < 1) n = 1;
    return n;
}

int pad_hw_get_slot_count(int port) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return 0;

    int count = 0;
    for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
        if (pad_slot_opened[port][slot]) count++;
    }
    if (count > 0) return count;

    int n = padGetSlotMax(port);
    if (n > ATH_PAD_MAX_SLOTS) n = ATH_PAD_MAX_SLOTS;
    if (n < 1) n = 1;
    return n;
}

const ath_pad_state_t *pad_hw_get_pad_state(int port, int slot) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return NULL;
    if (slot < 0 || slot >= ATH_PAD_MAX_SLOTS) return NULL;
    return &g_pad_states[port][slot];
}

int pad_hw_get_pressure(int port, int slot, u32 button_bit) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return 0;
    if (slot < 0 || slot >= ATH_PAD_MAX_SLOTS) return 0;

    const struct padButtonStatus *raw = &g_pad_states[port][slot].raw;

    switch (button_bit) {
        case PAD_UP:       return raw->up_p;
        case PAD_RIGHT:    return raw->right_p;
        case PAD_DOWN:     return raw->down_p;
        case PAD_LEFT:     return raw->left_p;
        case PAD_TRIANGLE: return raw->triangle_p;
        case PAD_CIRCLE:   return raw->circle_p;
        case PAD_CROSS:    return raw->cross_p;
        case PAD_SQUARE:   return raw->square_p;
        case PAD_L1:       return raw->l1_p;
        case PAD_R1:       return raw->r1_p;
        case PAD_L2:       return raw->l2_p;
        case PAD_R2:       return raw->r2_p;
        default:
            return 0;
    }
}

void pad_hw_poll(int port, int slot) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return;
    if (slot < 0 || slot >= ATH_PAD_MAX_SLOTS) return;

    if (!pad_hw_ensure_opened(port, slot)) {
        return;
    }

    ath_pad_state_t *st = &g_pad_states[port][slot];

    pad_hw_check_reconnection(port, slot);

    int state = padGetState(port, slot);
    st->connected = (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1);

    st->old_btns = st->btns;
    st->old_lx = st->lx;
    st->old_ly = st->ly;
    st->old_rx = st->rx;
    st->old_ry = st->ry;

    if (!st->connected) {
        st->btns = 0;
        st->lx = st->ly = st->rx = st->ry = 0;
        memset(&st->raw, 0, sizeof(st->raw));
        return;
    }

    struct padButtonStatus buttons;
    memset(&buttons, 0, sizeof(buttons));
    buttons.ljoy_h = 127;
    buttons.ljoy_v = 127;
    buttons.rjoy_h = 127;
    buttons.rjoy_v = 127;

    if (padRead(port, slot, &buttons)) {
        st->btns = 0xffff ^ buttons.btns;
        st->lx = buttons.ljoy_h - 127;
        st->ly = buttons.ljoy_v - 127;
        st->rx = buttons.rjoy_h - 127;
        st->ry = buttons.rjoy_v - 127;
        st->raw = buttons;
        st->type = pad_hw_get_type(port, slot);
    } else {
        st->btns = 0;
        st->lx = st->ly = st->rx = st->ry = 0;
    }
}

void pad_hw_poll_all(void) {
    int ports = pad_hw_get_port_count();
    for (int port = 0; port < ports; port++) {
        for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
            pad_hw_poll(port, slot);
        }
    }
}

static int g_pad_subsystem_initialized = 0;

void pad_hw_init(void) {
    if (g_pad_subsystem_initialized) return;

    dbgprintf("[PadHW] Initializing libpad...\n");
    padInit(0);

    int ports = pad_hw_get_port_count();
    for (int port = 0; port < ports; port++) {
        for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
            if (padPortOpen(port, slot, padBuf[port][slot]) == 0) {
                dbgprintf("[PadHW] padPortOpen(%d,%d) failed\n", port, slot);
                pad_slot_opened[port][slot] = 0;
                continue;
            }

            pad_slot_opened[port][slot] = 1;
            dbgprintf("[PadHW] Port %d slot %d opened%s\n", port, slot, slot > 0 ? " (multitap)" : "");

            if (pad_hw_initialize(port, slot)) {
                pad_initialized[port][slot] = 1;
            }
        }
    }

    g_pad_subsystem_initialized = 1;
}

void pad_hw_shutdown(void) {
    if (!g_pad_subsystem_initialized) return;

    int ports = pad_hw_get_port_count();
    for (int port = 0; port < ports; port++) {
        for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
            if (!pad_slot_opened[port][slot]) continue;
            padPortClose(port, slot);
            pad_slot_opened[port][slot] = 0;
            pad_initialized[port][slot] = 0;
            last_pad_state[port][slot] = 0;
            memset(&g_pad_states[port][slot], 0, sizeof(g_pad_states[port][slot]));
        }
    }

    g_pad_subsystem_initialized = 0;
    dbgprintf("[PadHW] Shutdown complete\n");
}