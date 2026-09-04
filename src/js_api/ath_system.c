#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <kernel.h>

#include <ath_env.h>
#include <memory.h>
#include <dbgprintf.h>

static JSValue athena_system_get_ticks(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    return JS_NewInt64(ctx, (int64_t)clock());
}

static JSValue athena_system_get_ms(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    clock_t c = clock();
    double ms = ((double)c / (double)CLOCKS_PER_SEC) * 1000.0;
    return JS_NewFloat64(ctx, ms);
}

static JSValue athena_system_sleep(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "System.sleep(milliseconds) requires 1 argument");
    }
    int32_t ms = 0;
    if (JS_ToInt32(ctx, &ms, argv[0])) {
        return JS_EXCEPTION;
    }
    if (ms > 0) {
        usleep(ms * 1000);
    }
    return JS_UNDEFINED;
}

static JSValue athena_system_get_used_memory(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    return JS_NewUint32(ctx, (uint32_t)get_used_memory());
}

static JSValue athena_system_get_free_memory(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    uint32_t total = GetMemorySize();
    uint32_t used = (uint32_t)get_used_memory();
    uint32_t free_mem = (total > used) ? (total - used) : 0;
    return JS_NewUint32(ctx, free_mem);
}

static JSValue athena_system_gc(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    JS_RunGC(JS_GetRuntime(ctx));
    return JS_UNDEFINED;
}

static JSValue athena_system_exit(JSContext *ctx, JSValue this_val, int argc, JSValueConst *argv) {
    dbgprintf("[AthenaCore] System.exit called\n");
    Exit(0);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry system_module_funcs[] = {
    JS_CFUNC_DEF("getTicks", 0, athena_system_get_ticks),
    JS_CFUNC_DEF("getMilliseconds", 0, athena_system_get_ms),
    JS_CFUNC_DEF("sleep", 1, athena_system_sleep),
    JS_CFUNC_DEF("getUsedMemory", 0, athena_system_get_used_memory),
    JS_CFUNC_DEF("getFreeMemory", 0, athena_system_get_free_memory),
    JS_CFUNC_DEF("gc", 0, athena_system_gc),
    JS_CFUNC_DEF("exit", 0, athena_system_exit),
    JS_PROP_STRING_DEF("bootPath", boot_path, JS_PROP_CONFIGURABLE),
};

static int athena_system_module_init(JSContext *ctx, JSModuleDef *m) {
    return JS_SetModuleExportList(ctx, m, system_module_funcs, countof(system_module_funcs));
}

JSModuleDef *athena_system_init(JSContext* ctx) {
    return athena_push_module(ctx, athena_system_module_init, system_module_funcs, countof(system_module_funcs), "System");
}
