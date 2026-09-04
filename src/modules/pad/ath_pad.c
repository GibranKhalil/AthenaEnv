#include <ath_env.h>
#include <libpad.h>
#include "pad_hw.h"
#include "ath_pad.h"

typedef struct {
    int port;
    int slot;
} JSPadHandle;

static JSClassID js_pad_class_id;

static JSValue js_pad_singletons[ATH_PAD_MAX_PORTS][ATH_PAD_MAX_SLOTS];

static void js_pad_finalizer(JSRuntime *rt, JSValue val) {
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque(val, js_pad_class_id);
    if (h) {
        js_free_rt(rt, h);
    }
}

static JSClassDef js_pad_class = {
    "Pad",
    .finalizer = js_pad_finalizer,
};

static double normalize_axis(int raw) {
    double v = (double)raw / 127.0;
    if (v > 1.0) v = 1.0;
    if (v < -1.0) v = -1.0;
    return v;
}

static int validate_port_slot(JSContext *ctx, int port, int slot) {
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) {
        JS_ThrowRangeError(ctx, "Porta invalida (0-%d)", ATH_PAD_MAX_PORTS - 1);
        return 0;
    }
    if (slot < 0 || slot >= ATH_PAD_MAX_SLOTS) {
        JS_ThrowRangeError(ctx, "Slot invalido (0-%d)", ATH_PAD_MAX_SLOTS - 1);
        return 0;
    }
    return 1;
}

static int parse_port_slot(JSContext *ctx, int argc, JSValueConst *argv, int *port, int *slot) {
    *port = 0;
    *slot = 0;
    if (argc >= 1 && JS_ToInt32(ctx, port, argv[0])) return 0;
    if (argc >= 2 && JS_ToInt32(ctx, slot, argv[1])) return 0;
    return validate_port_slot(ctx, *port, *slot);
}


static JSValue athena_pad_update(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    pad_hw_poll(h->port, h->slot);
    return JS_UNDEFINED;
}

static JSValue athena_pad_pressed(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "pressed(button) requires 1 argument");
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    const ath_pad_state_t *st = pad_hw_get_pad_state(h->port, h->slot);
    if (!st) return JS_NewBool(ctx, 0);

    return JS_NewBool(ctx, (st->btns & button) != 0);
}

static JSValue athena_pad_just_pressed(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "justPressed(button) requires 1 argument");
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    const ath_pad_state_t *st = pad_hw_get_pad_state(h->port, h->slot);
    if (!st) return JS_NewBool(ctx, 0);

    bool is_just_pressed = ((st->btns & button) != 0) && ((st->old_btns & button) == 0);
    return JS_NewBool(ctx, is_just_pressed);
}

static JSValue athena_pad_just_released(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "justReleased(button) requires 1 argument");
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    const ath_pad_state_t *st = pad_hw_get_pad_state(h->port, h->slot);
    if (!st) return JS_NewBool(ctx, 0);

    bool is_just_released = ((st->btns & button) == 0) && ((st->old_btns & button) != 0);
    return JS_NewBool(ctx, is_just_released);
}

static JSValue athena_pad_pressure(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_ThrowTypeError(ctx, "pressure(button) requires 1 argument");
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    uint32_t button = 0;
    if (JS_ToUint32(ctx, &button, argv[0])) return JS_EXCEPTION;

    int pressure = pad_hw_get_pressure(h->port, h->slot, button);
    return JS_NewInt32(ctx, pressure);
}

static JSValue athena_pad_rumble(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 2) return JS_ThrowTypeError(ctx, "rumble(smallOn, largeIntensity) requires 2 arguments");
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    int small_on = JS_ToBool(ctx, argv[0]);
    int large = 0;
    if (JS_ToInt32(ctx, &large, argv[1])) return JS_EXCEPTION;
    if (large < 0) large = 0;
    if (large > 255) large = 255;

    pad_hw_set_actuators(h->port, h->slot, (bool)small_on, (u8)large);
    return JS_UNDEFINED;
}

static JSValue js_pad_get_prop(JSContext *ctx, JSValueConst this_val, int magic) {
    JSPadHandle *h = (JSPadHandle *)JS_GetOpaque2(ctx, this_val, js_pad_class_id);
    if (!h) return JS_EXCEPTION;

    const ath_pad_state_t *st = pad_hw_get_pad_state(h->port, h->slot);

    switch (magic) {
        case 0:  return JS_NewInt32(ctx, h->port);
        case 1:  return JS_NewInt32(ctx, h->slot);
        case 2:  return JS_NewBool(ctx, st ? st->connected : 0);
        case 3:  return JS_NewInt32(ctx, st ? st->type : 0);
        case 4:  return JS_NewUint32(ctx, st ? st->btns : 0);
        case 5:  return JS_NewUint32(ctx, st ? st->old_btns : 0);
        case 6:  return JS_NewFloat64(ctx, st ? normalize_axis(st->lx) : 0.0);
        case 7:  return JS_NewFloat64(ctx, st ? normalize_axis(st->ly) : 0.0);
        case 8:  return JS_NewFloat64(ctx, st ? normalize_axis(st->rx) : 0.0);
        case 9:  return JS_NewFloat64(ctx, st ? normalize_axis(st->ry) : 0.0);
        case 10: return JS_NewFloat64(ctx, st ? normalize_axis(st->old_lx) : 0.0);
        case 11: return JS_NewFloat64(ctx, st ? normalize_axis(st->old_ly) : 0.0);
        case 12: return JS_NewFloat64(ctx, st ? normalize_axis(st->old_rx) : 0.0);
        case 13: return JS_NewFloat64(ctx, st ? normalize_axis(st->old_ry) : 0.0);
        default: return JS_UNDEFINED;
    }
}

static const JSCFunctionListEntry js_pad_proto_funcs[] = {
    JS_CFUNC_DEF("update", 0, athena_pad_update),
    JS_CFUNC_DEF("pressed", 1, athena_pad_pressed),
    JS_CFUNC_DEF("justPressed", 1, athena_pad_just_pressed),
    JS_CFUNC_DEF("justReleased", 1, athena_pad_just_released),
    JS_CFUNC_DEF("pressure", 1, athena_pad_pressure),
    JS_CFUNC_DEF("rumble", 2, athena_pad_rumble),

    JS_CGETSET_MAGIC_DEF("port",      js_pad_get_prop, NULL, 0),
    JS_CGETSET_MAGIC_DEF("slot",      js_pad_get_prop, NULL, 1),
    JS_CGETSET_MAGIC_DEF("connected", js_pad_get_prop, NULL, 2),
    JS_CGETSET_MAGIC_DEF("type",      js_pad_get_prop, NULL, 3),
    JS_CGETSET_MAGIC_DEF("btns",      js_pad_get_prop, NULL, 4),
    JS_CGETSET_MAGIC_DEF("old_btns",  js_pad_get_prop, NULL, 5),
    JS_CGETSET_MAGIC_DEF("lx",        js_pad_get_prop, NULL, 6),
    JS_CGETSET_MAGIC_DEF("ly",        js_pad_get_prop, NULL, 7),
    JS_CGETSET_MAGIC_DEF("rx",        js_pad_get_prop, NULL, 8),
    JS_CGETSET_MAGIC_DEF("ry",        js_pad_get_prop, NULL, 9),
    JS_CGETSET_MAGIC_DEF("old_lx",    js_pad_get_prop, NULL, 10),
    JS_CGETSET_MAGIC_DEF("old_ly",    js_pad_get_prop, NULL, 11),
    JS_CGETSET_MAGIC_DEF("old_rx",    js_pad_get_prop, NULL, 12),
    JS_CGETSET_MAGIC_DEF("old_ry",    js_pad_get_prop, NULL, 13),
};

static JSValue athena_pads_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int port, slot;
    if (!parse_port_slot(ctx, argc, argv, &port, &slot)) return JS_EXCEPTION;

    if (!JS_IsUndefined(js_pad_singletons[port][slot])) {
        return JS_DupValue(ctx, js_pad_singletons[port][slot]);
    }

    JSValue obj = JS_NewObjectClass(ctx, js_pad_class_id);
    if (JS_IsException(obj)) return obj;

    JSPadHandle *h = (JSPadHandle *)js_mallocz(ctx, sizeof(JSPadHandle));
    if (!h) {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    h->port = port;
    h->slot = slot;

    JS_SetOpaque(obj, h);

    js_pad_singletons[port][slot] = JS_DupValue(ctx, obj);

    return obj;
}

static JSValue athena_pads_init_fn(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    pad_hw_init();
    return JS_UNDEFINED;
}

static JSValue athena_pads_update(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    pad_hw_poll_all();
    return JS_UNDEFINED;
}

static JSValue athena_pads_shutdown(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    pad_hw_shutdown();
    return JS_UNDEFINED;
}

static JSValue athena_pads_get_type(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port, slot;
    if (!parse_port_slot(ctx, argc, argv, &port, &slot)) return JS_EXCEPTION;
    return JS_NewInt32(ctx, pad_hw_get_type(port, slot));
}

static JSValue athena_pads_get_state(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port, slot;
    if (!parse_port_slot(ctx, argc, argv, &port, &slot)) return JS_EXCEPTION;
    return JS_NewInt32(ctx, pad_hw_get_state(port, slot));
}

static JSValue athena_pads_is_active(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port, slot;
    if (!parse_port_slot(ctx, argc, argv, &port, &slot)) return JS_EXCEPTION;

    int state = pad_hw_get_state(port, slot);
    bool active = (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1);
    return JS_NewBool(ctx, active);
}

static JSValue athena_pads_get_connected(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    JSValue arr = JS_NewArray(ctx);
    int count = 0;

    int ports = pad_hw_get_port_count();
    for (int port = 0; port < ports; port++) {
        for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
            int state = pad_hw_get_state(port, slot);
            if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
                JSValue entry = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, entry, "port", JS_NewInt32(ctx, port));
                JS_SetPropertyStr(ctx, entry, "slot", JS_NewInt32(ctx, slot));
                JS_SetPropertyUint32(ctx, arr, count++, entry);
            }
        }
    }
    return arr;
}

static JSValue athena_pads_get_connected_count(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int count = 0;
    int ports = pad_hw_get_port_count();
    for (int port = 0; port < ports; port++) {
        for (int slot = 0; slot < ATH_PAD_MAX_SLOTS; slot++) {
            int state = pad_hw_get_state(port, slot);
            if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
                count++;
            }
        }
    }
    return JS_NewInt32(ctx, count);
}

static JSValue athena_pads_get_port_count(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    return JS_NewInt32(ctx, pad_hw_get_port_count());
}

static JSValue athena_pads_get_slot_count(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    int port = 0;
    if (argc >= 1 && JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
    if (port < 0 || port >= ATH_PAD_MAX_PORTS) return JS_ThrowRangeError(ctx, "Porta invalida");
    return JS_NewInt32(ctx, pad_hw_get_slot_count(port));
}

static JSValue athena_pads_rumble(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 4) return JS_ThrowTypeError(ctx, "rumble(port, slot, smallOn, largeIntensity) requires 4 arguments");

    int port = 0, slot = 0, large = 0;
    if (JS_ToInt32(ctx, &port, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &slot, argv[1])) return JS_EXCEPTION;
    if (!validate_port_slot(ctx, port, slot)) return JS_EXCEPTION;

    int small_on = JS_ToBool(ctx, argv[2]);
    if (JS_ToInt32(ctx, &large, argv[3])) return JS_EXCEPTION;
    if (large < 0) large = 0;
    if (large > 255) large = 255;

    pad_hw_set_actuators(port, slot, (bool)small_on, (u8)large);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry module_funcs[] = {
    JS_CFUNC_DEF("init", 0, athena_pads_init_fn),
    JS_CFUNC_DEF("update", 0, athena_pads_update),
    JS_CFUNC_DEF("shutdown", 0, athena_pads_shutdown),
    JS_CFUNC_DEF("get", 2, athena_pads_get),
    JS_CFUNC_DEF("getType", 2, athena_pads_get_type),
    JS_CFUNC_DEF("getState", 2, athena_pads_get_state),
    JS_CFUNC_DEF("isActive", 2, athena_pads_is_active),
    JS_CFUNC_DEF("getConnected", 0, athena_pads_get_connected),
    JS_CFUNC_DEF("getConnectedCount", 0, athena_pads_get_connected_count),
    JS_CFUNC_DEF("getPortCount", 0, athena_pads_get_port_count),
    JS_CFUNC_DEF("getSlotCount", 1, athena_pads_get_slot_count),
    JS_CFUNC_DEF("rumble", 4, athena_pads_rumble),

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
    JS_PROP_INT32_DEF("STATE_FINDCTP1", PAD_STATE_FINDCTP1, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("STATE_STABLE", PAD_STATE_STABLE, JS_PROP_CONFIGURABLE),
    JS_PROP_INT32_DEF("STATE_ERROR", PAD_STATE_ERROR, JS_PROP_CONFIGURABLE),
};

static int js_pads_module_init(JSContext *ctx, JSModuleDef *m) {
    JS_NewClassID(&js_pad_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_pad_class_id, &js_pad_class);

    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_pad_proto_funcs, countof(js_pad_proto_funcs));
    JS_SetClassProto(ctx, js_pad_class_id, proto);

    for (int p = 0; p < ATH_PAD_MAX_PORTS; p++) {
        for (int s = 0; s < ATH_PAD_MAX_SLOTS; s++) {
            js_pad_singletons[p][s] = JS_UNDEFINED;
        }
    }

    pad_hw_init();

    return JS_SetModuleExportList(ctx, m, module_funcs, countof(module_funcs));
}

JSModuleDef *athena_pads_init(JSContext *ctx) {
    return athena_push_module(ctx, js_pads_module_init, module_funcs, countof(module_funcs), "Pads");
}