#include <ath_env.h>
#include <athena/mouse.h>

static JSValue js_mouse_init(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    return JS_NewInt32(ctx, athena_mouse_init());
}

static JSValue js_mouse_get(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    JSValue obj;
    AthenaMouseData data;

    athena_mouse_read(&data);

    obj = JS_NewObject(ctx);
    JS_DefinePropertyValueStr(ctx, obj, "x", JS_NewInt32(ctx, data.x), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, obj, "y", JS_NewInt32(ctx, data.y), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, obj, "wheel", JS_NewInt32(ctx, data.wheel), JS_PROP_C_W_E);
    JS_DefinePropertyValueStr(ctx, obj, "buttons", JS_NewUint32(ctx, data.buttons), JS_PROP_C_W_E);

    return obj;
}

static JSValue js_mouse_setboundary(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    int minx, maxx, miny, maxy;

    JS_ToInt32(ctx, &minx, argv[0]);
    JS_ToInt32(ctx, &maxx, argv[1]);
    JS_ToInt32(ctx, &miny, argv[2]);
    JS_ToInt32(ctx, &maxy, argv[3]);

    return JS_NewInt32(ctx, athena_mouse_set_boundary(minx, maxx, miny, maxy));
}

static JSValue js_mouse_getmode(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    return JS_NewUint32(ctx, athena_mouse_get_mode());
}

static JSValue js_mouse_setmode(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    uint32_t mode;
    JS_ToUint32(ctx, &mode, argv[0]);
    return JS_NewUint32(ctx, athena_mouse_set_mode(mode));
}

static JSValue js_mouse_getaccel(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    return JS_NewFloat32(ctx, athena_mouse_get_accel());
}

static JSValue js_mouse_setaccel(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    float accel;
    JS_ToFloat32(ctx, &accel, argv[0]);
    return JS_NewInt32(ctx, athena_mouse_set_accel(accel));
}

static JSValue js_mouse_setpos(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    int x, y;
    JS_ToInt32(ctx, &x, argv[0]);
    JS_ToInt32(ctx, &y, argv[1]);
    return JS_NewInt32(ctx, athena_mouse_set_position(x, y));
}

static const JSCFunctionListEntry module_funcs[] = {
    JS_CFUNC_DEF("init", 0, js_mouse_init),
    JS_CFUNC_DEF("get", 0, js_mouse_get),
    JS_CFUNC_DEF("setBoundary", 4, js_mouse_setboundary),
    JS_CFUNC_DEF("getMode", 0, js_mouse_getmode),
    JS_CFUNC_DEF("setMode", 1, js_mouse_setmode),
    JS_CFUNC_DEF("getAccel", 0, js_mouse_getaccel),
    JS_CFUNC_DEF("setAccel", 1, js_mouse_setaccel),
    JS_CFUNC_DEF("setPosition", 2, js_mouse_setpos),
};

static int module_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, module_funcs, countof(module_funcs));
}

#ifdef DYNAMIC_ATHENA_MOUSE
char *erl_dependancies[] = {
    "libmouse",
    0
};
JSModuleDef *js_init_module(JSContext *ctx)
#else
JSModuleDef *athena_mouse_init(JSContext *ctx)
#endif
{
    return athena_push_module(ctx, module_init, module_funcs, countof(module_funcs), "Mouse");
}
