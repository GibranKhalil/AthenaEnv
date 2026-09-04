#include "pad_hw.h"
#include <kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libpad.h>
#include <dbgprintf.h>

static char pad0Buf[256] __attribute__((aligned(64)));
static char pad1Buf[256] __attribute__((aligned(64)));
static char actAlign[6];

static int pad_initialized[2] = {0, 0};
static int last_pad_state[2] = {PAD_STATE_DISCONN, PAD_STATE_DISCONN};

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
        dbgprintf("[PadHW] Pad not ready, skipping init for port %d\n", port);
        return 0;
    }

    int modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    dbgprintf("[PadHW] Port %d device modes: %d\n", port, modes);

    if (modes == 0) {
        dbgprintf("[PadHW] Digital controller detected on port %d\n", port);
        return 1;
    }

    // Verify if Dual Shock is supported
    int i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) {
            break;
        }
        i++;
    } while (i < modes);

    if (i >= modes) {
        dbgprintf("[PadHW] Not a DualShock controller on port %d\n", port);
        return 1;
    }

    int ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        return 1;
    }

    dbgprintf("[PadHW] Enabling DualShock mode for port %d\n", port);
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    pad_hw_wait_ready(port, slot);
    padEnterPressMode(port, slot);

    pad_hw_wait_ready(port, slot);
    int actuators = padInfoAct(port, slot, -1, 0);
    dbgprintf("[PadHW] Actuators found on port %d: %d\n", port, actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Small engine
        actAlign[1] = 1;   // Big engine
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
    int current_state = padGetState(port, slot);

    if (current_state == PAD_STATE_DISCONN) {
        pad_initialized[port] = 0;
    }

    if ((current_state == PAD_STATE_STABLE || current_state == PAD_STATE_FINDCTP1) && !pad_initialized[port]) {
        pad_hw_initialize(port, slot);
        pad_initialized[port] = 1;
    }

    last_pad_state[port] = current_state;
}

int pad_hw_read(int port, int slot, struct padButtonStatus *buttons) {
    pad_hw_check_reconnection(port, slot);

    int state = padGetState(port, slot);
    if (state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1) {
        return 0;
    }

    return padRead(port, slot, buttons);
}

int pad_hw_set_actuators(int port, int slot, u8 small, u8 large) {
    pad_hw_check_reconnection(port, slot);

    int state = padGetState(port, slot);
    if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
        char act[6] = { (char)small, (char)large, 0, 0, 0, 0 };
        return padSetActDirect(port, slot, act);
    }
    return 0;
}

int pad_hw_get_state(int port, int slot) {
    return padGetState(port, slot);
}

int pad_hw_get_type(int port, int slot) {
    return padInfoMode(port, slot, PAD_MODETABLE, 0);
}

void pad_hw_init(void) {
    static int initialized = 0;
    if (initialized) return;

    dbgprintf("[PadHW] Initializing libpad...\n");
    padInit(0);

    if (padPortOpen(0, 0, pad0Buf) == 0) {
        dbgprintf("[PadHW] padPortOpen(0) failed\n");
    } else {
        if (pad_hw_initialize(0, 0)) {
            pad_initialized[0] = 1;
        }
    }

    if (padPortOpen(1, 0, pad1Buf) == 0) {
        dbgprintf("[PadHW] padPortOpen(1) failed\n");
    } else {
        if (pad_hw_initialize(1, 0)) {
            pad_initialized[1] = 1;
        }
    }

    initialized = 1;
}
