#include <ath_env.h>
#include <athena/keyboard.h>

static JSValue js_keyboard_init(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    return JS_NewInt32(ctx, athena_keyboard_open());
}

static JSValue js_keyboard_get(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    char key = 0;
    athena_keyboard_read(&key);
    return JS_NewInt32(ctx, key);
}

static JSValue js_keyboard_setrepeatrate(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    uint32_t msec;
    JS_ToUint32(ctx, &msec, argv[0]);
    return JS_NewInt32(ctx, athena_keyboard_set_repeat_rate(msec));
}

static JSValue js_keyboard_setblockingmode(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    uint32_t mode;
    JS_ToUint32(ctx, &mode, argv[0]);
    return JS_NewInt32(ctx, athena_keyboard_set_blocking_mode(mode));
}

static JSValue js_keyboard_deinit(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv)
{
    return JS_NewInt32(ctx, athena_keyboard_deinit());
}

static const JSCFunctionListEntry module_funcs[] = {
    JS_CFUNC_DEF("init", 0, js_keyboard_init),
    JS_CFUNC_DEF("get", 0, js_keyboard_get),
    JS_CFUNC_DEF("setRepeatRate", 1, js_keyboard_setrepeatrate),
    JS_CFUNC_DEF("setBlockingMode", 1, js_keyboard_setblockingmode),
    JS_CFUNC_DEF("deinit", 0, js_keyboard_deinit),
};

static int module_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, module_funcs, countof(module_funcs));
}

#ifdef DYNAMIC_ATHENA_KEYBOARD
char *erl_dependancies[] = {
    "libkbd",
    0
};

JSModuleDef *js_init_module(JSContext *ctx)
#else
JSModuleDef *athena_keyboard_init(JSContext *ctx)
#endif
{
    return athena_push_module(ctx, module_init, module_funcs, countof(module_funcs), "Keyboard");
}
