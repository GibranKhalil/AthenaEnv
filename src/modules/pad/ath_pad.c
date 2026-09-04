#include <ath_env.h>
#include <libpad.h>
#include "pad_hw.h"
#include "ath_pad.h"

typedef struct {
    int port;
    int slot;
    u32 btns;
    u32 old_btns;
    int lx, ly;
    int rx, ry;
    int old_lx, old_ly;
    int old_rx, old_ry;
} JSPadObject;

static JSClassID js_pad_class_id;

static void js_pad_finalizer(JSRuntime *rt, JSValue val) {
    JSPadObject *pad = (JSPadObject *)JS_GetOpaque(val, js_pad_class_id);
    if (pad) {
        js_free_rt(rt, pad);
    }
}

static JSClassDef js_pad_class = {
    "Pad",
    .finalizer = js_pad_finalizer,
};

static JSValue athena_pad_update(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    JSPadObject *pad = (JSPadObject *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!pad) return JS_EXCEPTION;

    struct padButtonStatus buttons;
    memset(&buttons, 0, sizeof(buttons));
    buttons.ljoy_h = 127;
    buttons.ljoy_v = 127;
    buttons.rjoy_h = 127;
    buttons.rjoy_v = 127;

    pad->old_btns = pad->btns;
    pad->old_lx = pad->lx;
    pad->old_ly = pad->ly;
    pad->old_rx = pad->rx;
    pad->old_ry = pad->ry;

    int read_success = pad_hw_read(pad->port, pad->slot, &buttons);
    if (read_success != 0) {
        pad->btns = 0xffff ^ buttons.btns;
        pad->lx = buttons.ljoy_h - 127;
        pad->ly = buttons.ljoy_v - 127;
        pad->rx = buttons.rjoy_h - 127;
        pad->ry = buttons.rjoy_v - 127;
    } else {
        pad->btns = 0;
        pad->lx = 0;
        pad->ly = 0;
        pad->rx = 0;
        pad->ry = 0;
    }

    return JS_UNDEFINED;
}

static JSValue athena_pad_pressed(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "pressed(button) requires 1 argument");
    JSPadObject *pad = (JSPadObject *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!pad) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    return JS_NewBool(ctx, (pad->btns & button) != 0);
}

static JSValue athena_pad_just_pressed(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "justPressed(button) requires 1 argument");
    JSPadObject *pad = (JSPadObject *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!pad) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    bool is_just_pressed = ((pad->btns & button) != 0) && ((pad->old_btns & button) == 0);
    return JS_NewBool(ctx, is_just_pressed);
}

static JSValue js_pad_get_prop(JSContext *ctx, JSValueConst this_val, int magic) {
    JSPadObject *pad = (JSPadObject *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!pad) return JS_EXCEPTION;

    switch (magic) {
        case 0: return JS_NewUint32(ctx, pad->btns);
        case 1: return JS_NewInt32(ctx, pad->lx);
        case 2: return JS_NewInt32(ctx, pad->ly);
        case 3: return JS_NewInt32(ctx, pad->rx);
        case 4: return JS_NewInt32(ctx, pad->ry);
        case 5: return JS_NewUint32(ctx, pad->old_btns);
        case 6: return JS_NewInt32(ctx, pad->old_lx);
        case 7: return JS_NewInt32(ctx, pad->old_ly);
        case 8: return JS_NewInt32(ctx, pad->old_rx);
        case 9: return JS_NewInt32(ctx, pad->old_ry);
        case 10: return JS_NewInt32(ctx, pad->port);
        default: return JS_UNDEFINED;
    }
}

static const JSCFunctionListEntry js_pad_proto_funcs[] = {
    JS_CFUNC_DEF("update", 0, athena_pad_update),
    JS_CFUNC_DEF("pressed", 1, athena_pad_pressed),
    JS_CFUNC_DEF("justPressed", 1, athena_pad_just_pressed),
    JS_CGETSET_MAGIC_DEF("btns", js_pad_get_prop, NULL, 0),
    JS_CGETSET_MAGIC_DEF("lx",   js_pad_get_prop, NULL, 1),
    JS_CGETSET_MAGIC_DEF("ly",   js_pad_get_prop, NULL, 2),
    JS_CGETSET_MAGIC_DEF("rx",   js_pad_get_prop, NULL, 3),
    JS_CGETSET_MAGIC_DEF("ry",   js_pad_get_prop, NULL, 4),
    JS_CGETSET_MAGIC_DEF("old_btns", js_pad_get_prop, NULL, 5),
    JS_CGETSET_MAGIC_DEF("old_lx",   js_pad_get_prop, NULL, 6),
    JS_CGETSET_MAGIC_DEF("old_ly",   js_pad_get_prop, NULL, 7),
    JS_CGETSET_MAGIC_DEF("old_rx",   js_pad_get_prop, NULL, 8),
    JS_CGETSET_MAGIC_DEF("old_ry",   js_pad_get_prop, NULL, 9),
    JS_CGETSET_MAGIC_DEF("port",     js_pad_get_prop, NULL, 10),
};

/* Module Functions */
static JSValue athena_pads_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int port = 0;
    if (argc >= 1) {
        if (JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
        if (port < 0 || port > 1) {
            return JS_ThrowRangeError(ctx, "Port must be 0 or 1");
        }
    }

    JSValue obj = JS_NewObjectClass(ctx, js_pad_class_id);
    if (JS_IsException(obj)) return obj;

    JSPadObject *pad = (JSPadObject *)js_mallocz(ctx, sizeof(JSPadObject));
    if (!pad) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }

    pad->port = port;
    pad->slot = 0;

    pad_hw_check_reconnection(pad->port, pad->slot);

    struct padButtonStatus buttons;
    memset(&buttons, 0, sizeof(buttons));
    buttons.ljoy_h = 127;
    buttons.ljoy_v = 127;
    buttons.rjoy_h = 127;
    buttons.rjoy_v = 127;

    if (pad_hw_read(pad->port, pad->slot, &buttons)) {
        pad->btns = 0xffff ^ buttons.btns;
        pad->lx = buttons.ljoy_h - 127;
        pad->ly = buttons.ljoy_v - 127;
        pad->rx = buttons.rjoy_h - 127;
        pad->ry = buttons.rjoy_v - 127;
    }

    JS_SetOpaque(obj, pad);
    return obj;
}

static JSValue athena_pads_init_fn(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    pad_hw_init();
    return JS_UNDEFINED;
}

static JSValue athena_pads_get_type(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port = 0;
    if (argc >= 1 && JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
    if (port < 0 || port > 1) return JS_ThrowRangeError(ctx, "Port must be 0 or 1");

    return JS_NewInt32(ctx, pad_hw_get_type(port, 0));
}

static JSValue athena_pads_get_state(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port = 0;
    if (argc >= 1 && JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
    if (port < 0 || port > 1) return JS_ThrowRangeError(ctx, "Port must be 0 or 1");

    return JS_NewInt32(ctx, pad_hw_get_state(port, 0));
}

static JSValue athena_pads_is_active(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port = 0;
    if (argc >= 1 && JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
    if (port < 0 || port > 1) return JS_ThrowRangeError(ctx, "Port must be 0 or 1");

    int state = pad_hw_get_state(port, 0);
    bool active = (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1);
    return JS_NewBool(ctx, active);
}

static JSValue athena_pads_get_connected(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    JSValue arr = JS_NewArray(ctx);
    int count = 0;

    for (int port = 0; port < 2; port++) {
        int state = pad_hw_get_state(port, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
            JS_SetPropertyUint32(ctx, arr, count++, JS_NewInt32(ctx, port));
        }
    }
    return arr;
}

static JSValue athena_pads_get_connected_count(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int count = 0;
    for (int port = 0; port < 2; port++) {
        int state = pad_hw_get_state(port, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
            count++;
        }
    }
    return JS_NewInt32(ctx, count);
}

static JSValue athena_pads_rumble(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 2) return JS_ThrowTypeError(ctx, "rumble requires at least (small, large) or (port, small, large)");

    int port = 0;
    int small = 0;
    int large = 0;

    if (argc >= 3) {
        JS_ToInt32(ctx, &port, argv[0]);
        JS_ToInt32(ctx, &small, argv[1]);
        JS_ToInt32(ctx, &large, argv[2]);
    } else {
        JS_ToInt32(ctx, &small, argv[0]);
        JS_ToInt32(ctx, &large, argv[1]);
    }

    if (port < 0 || port > 1) return JS_ThrowRangeError(ctx, "Port must be 0 or 1");

    pad_hw_set_actuators(port, 0, (u8)(small ? 1 : 0), (u8)large);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry module_funcs[] = {
    JS_CFUNC_DEF("init", 0, athena_pads_init_fn),
    JS_CFUNC_DEF("get", 1, athena_pads_get),
    JS_CFUNC_DEF("getType", 1, athena_pads_get_type),
    JS_CFUNC_DEF("getState", 1, athena_pads_get_state),
    JS_CFUNC_DEF("isActive", 1, athena_pads_is_active),
    JS_CFUNC_DEF("getConnected", 0, athena_pads_get_connected),
    JS_CFUNC_DEF("getConnectedCount", 0, athena_pads_get_connected_count),
    JS_CFUNC_DEF("rumble", 3, athena_pads_rumble),

    /* Controller buttons */
    JS_PROP_INT32_DEF("SELECT", PAD_SELECT, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("START", PAD_START, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("UP", PAD_UP, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("RIGHT", PAD_RIGHT, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("DOWN", PAD_DOWN, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("LEFT", PAD_LEFT, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("TRIANGLE", PAD_TRIANGLE, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("CIRCLE", PAD_CIRCLE, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("CROSS", PAD_CROSS, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("SQUARE", PAD_SQUARE, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("L1", PAD_L1, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("R1", PAD_R1, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("L2", PAD_L2, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("R2", PAD_R2, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("L3", PAD_L3, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("R3", PAD_R3, JS_PROP_CONFIGURABLE),

    /* Pad types */
    JS_PROP_INT32_DEF("TYPE_DIGITAL", PAD_TYPE_DIGITAL, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("TYPE_ANALOG", PAD_TYPE_ANALOG, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("TYPE_DUALSHOCK", PAD_TYPE_DUALSHOCK, JS_PROP_CONFIGURABLE),

    /* States */
    JS_PROP_INT32_DEF("STATE_DISCONN", PAD_STATE_DISCONN, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("STATE_STABLE", PAD_STATE_STABLE, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("STATE_ERROR", PAD_STATE_ERROR, JS_PROP_CONFIGURABLE),
};

static int js_pads_module_init(JSContext *ctx, JSModuleDef *m) {
    JS_NewClassID(&js_pad_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_pad_class_id, &js_pad_class);

    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_pad_proto_funcs, countof(js_pad_proto_funcs));
    JS_SetClassProto(ctx, js_pad_class_id, proto);

    pad_hw_init();

    return JS_SetModuleExportList(ctx, m, module_funcs, countof(module_funcs));
}

JSModuleDef *athena_pads_init(JSContext *ctx) {
    return athena_push_module(ctx, js_pads_module_init, module_funcs, countof(module_funcs), "Pads");
}
