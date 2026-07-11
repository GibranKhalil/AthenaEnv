/**
 * AthenaEnv Native Compiler Test
 * 
 * This example demonstrates how to use Native.compile() to create
 * optimized native functions for performance-critical code.
 */

// Example 1: Simple addition function
console.log("\n=== Example 1: Simple Addition ===");

const nativeAdd = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => {
    return a + b;
});

// Test the native function
const result1 = nativeAdd(10, 20);
console.log(`nativeAdd(10, 20) = ${result1}`);

// Example 2: Vector addition using Float32Array
console.log("\n=== Example 2: Vector Addition ===");
const addVectors = Native.compile({
    args: ['Float32Array', 'Float32Array', 'Float32Array', 'int'],
    returns: 'void'
}, (a, b, result, len) => {
    for (let i = 0; i < len; i++) {
        result[i] = a[i] + b[i];
    }
});

// Test vector addition
const vecA = new Float32Array([1.0, 2.0, 3.0, 4.0]);
const vecB = new Float32Array([5.0, 6.0, 7.0, 8.0]);
const vecResult = new Float32Array([48.0, 48.0, 48.0, 48.0]);

addVectors(vecA, vecB, vecResult, 4);
console.log(`Vector A: [${vecA.join(', ')}]`);
console.log(`Vector B: [${vecB.join(', ')}]`);
console.log(`Result:   [${vecResult.join(', ')}]`);

// Example 3: Dot product
console.log("\n=== Example 3: Dot Product ===");

const dotProduct = Native.compile({
    args: ['Float32Array', 'Float32Array', 'int'],
    returns: 'float'
}, (a, b, len) => {
    let sum = 0.0;
    for (let i = 0; i < len; i++) {
        sum = sum + a[i] * b[i];
    }
    return sum;
});

const dot = dotProduct(vecA, vecB, 4);
console.log(`Dot product: ${dot}`);

// Example 4: Fibonacci (recursive simulation with loop)
console.log("\n=== Example 4: Fibonacci ===");

const fibonacci = Native.compile({
    args: ['int'],
    returns: 'int'
}, (n) => {
    if (n <= 1) return n;

    let a = 0;
    let b = 1;
    for (let i = 2; i <= n; i++) {
        let temp = a + b;
        a = b;
        b = temp;
    }
    return b;
});

console.log(`fibonacci(10) = ${fibonacci(10)}`);
console.log(`fibonacci(20) = ${fibonacci(20)}`);

// Example 5: Arithmetic Tests (MUL, DIV, MOD)
console.log("\n=== Example 5: Arithmetic Operations ===");

const nativeMul = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a * b);

const nativeDiv = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => (a / b) | 0);  // floor to int

const nativeMod = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a % b);

const nativeNeg = Native.compile({
    args: ['int'],
    returns: 'int'
}, (a) => -a);

console.log(`3 * 7 = ${nativeMul(3, 7)}`);           // Expected: 21
console.log(`100 / 7 = ${nativeDiv(100, 7)}`);       // Expected: 14
console.log(`100 % 7 = ${nativeMod(100, 7)}`);       // Expected: 2
console.log(`-42 = ${nativeNeg(42)}`);               // Expected: -42

// Strength reduction tests (should use SHL instead of MULT)
const mulBy8 = Native.compile({
    args: ['int'],
    returns: 'int'
}, (a) => a * 8);  // Should become a << 3

const divBy4 = Native.compile({
    args: ['int'],
    returns: 'int'
}, (a) => (a / 4) | 0);  // Should become a >> 2

console.log(`5 * 8 = ${mulBy8(5)}`);  // Expected: 40 (uses SHL $t0, $t0, 3)
console.log(`100 / 4 = ${divBy4(100)}`);  // Expected: 25 (uses SAR)

// Constant folding test (should compute at compile time)
const constExpr = Native.compile({
    args: [],
    returns: 'int'
}, () => 10 + 5 * 2);  // Should fold to 20 (single LI)

console.log(`10 + 5 * 2 = ${constExpr()}`);  // Expected: 20

// Example 6: Bitwise Operations
console.log("\n=== Example 6: Bitwise Operations ===");

const nativeAnd = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a & b);

const nativeOr = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a | b);

const nativeXor = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a ^ b);

const nativeShl = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a << b);

const nativeShr = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => a >> b);

console.log(`0xFF & 0x0F = ${nativeAnd(0xFF, 0x0F)}`);   // Expected: 15
console.log(`0xF0 | 0x0F = ${nativeOr(0xF0, 0x0F)}`);    // Expected: 255
console.log(`0xFF ^ 0x0F = ${nativeXor(0xFF, 0x0F)}`);   // Expected: 240
console.log(`1 << 4 = ${nativeShl(1, 4)}`);              // Expected: 16
console.log(`256 >> 4 = ${nativeShr(256, 4)}`);          // Expected: 16

// Example 7: Benchmark comparison
console.log("\n=== Example 7: Benchmark ===");

// JavaScript version
function jsAddLoop(arr, len, iterations) {
    let sum = 0;
    for (let iter = 0; iter < iterations; iter++) {
        for (let i = 0; i < len; i++) {
            sum += arr[i];
        }
    }
    return sum;
}

// Native version
const nativeAddLoop = Native.compile({
    args: ['Int32Array', 'int', 'int'],
    returns: 'int'
}, (arr, len, iterations) => {
    let sum = 0;
    for (let iter = 0; iter < iterations; iter++) {
        for (let i = 0; i < len; i++) {
            sum = sum + arr[i];
        }
    }
    return sum;
});

// Prepare test data
const testData = new Int32Array(100);
for (let i = 0; i < 100; i++) {
    testData[i] = i;
}

const ITERATIONS = 1000;

// Benchmark JavaScript version
const jsStart = Timer.new();
const jsResult = jsAddLoop(testData, 100, ITERATIONS);
const jsTime = Timer.getTime(jsStart);

// Benchmark Native version
const nativeStart = Timer.new();
const nativeResult = nativeAddLoop(testData, 100, ITERATIONS);
const nativeTime = Timer.getTime(nativeStart);

console.log(`JavaScript: ${jsResult} in ${jsTime}ms`);
console.log(`Native:     ${nativeResult} in ${nativeTime}ms`);

if (nativeTime > 0) {
    const speedup = jsTime / nativeTime;
    console.log(`Speedup: ${speedup.toFixed(2)}x`);
}

// Get info about compiled function
console.log("\n=== Compiled Function Info ===");
// Note: getInfo expects the internal handle, not the wrapper
// This is a simplified example showing the API

// Example 8: Math.sqrt (Native MIPS SQRT.S instruction)
console.log("\n=== Example 8: Math.sqrt ===");
const nativeSqrt = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.sqrt(x);
});

const sqrt4 = nativeSqrt(4.0);
const sqrt16 = nativeSqrt(16.0);
const sqrt2 = nativeSqrt(2.0);
console.log(`Math.sqrt(4) = ${sqrt4}`);
console.log(`Math.sqrt(16) = ${sqrt16}`);
console.log(`Math.sqrt(2) = ${sqrt2.toFixed(4)}`);

// Example 9: Math.abs (Native MIPS ABS.S instruction)
console.log("\n=== Example 9: Math.abs ===");
const nativeAbs = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.abs(x);
});

const abs5 = nativeAbs(-5.0);
const abs3 = nativeAbs(3.0);
const absPI = nativeAbs(-3.14159);
console.log(`Math.abs(-5) = ${abs5}`);
console.log(`Math.abs(3) = ${abs3}`);
console.log(`Math.abs(-3.14159) = ${absPI.toFixed(4)}`);

// Example 10: Math.sin (Via C function call)
console.log("\n=== Example 10: Math.sin ===");
const nativeSin = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.sin(x);
});

const sin0 = nativeSin(0.0);
const sinPI2 = nativeSin(1.5708); // ~PI/2
console.log(`Math.sin(0) = ${sin0.toFixed(4)}`);
console.log(`Math.sin(PI/2) = ${sinPI2.toFixed(4)}`);

// Example 11: Math.cos (Via C function call)
console.log("\n=== Example 11: Math.cos ===");
const nativeCos = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.cos(x);
});

const cos0 = nativeCos(0.0);
const cosPI = nativeCos(3.14159); // ~PI
console.log(`Math.cos(0) = ${cos0.toFixed(4)}`);
console.log(`Math.cos(PI) = ${cosPI.toFixed(4)}`);

// Example 11.5: Math.tan (Via C function call)
console.log("\n=== Example 11.5: Math.tan ===");
const nativeTan = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.tan(x);
});

const tan0 = nativeTan(0.0);
const tanPI4 = nativeTan(0.7854); // ~PI/4
console.log(`Math.tan(0) = ${tan0.toFixed(4)}`);
console.log(`Math.tan(PI/4) = ${tanPI4.toFixed(4)}`);

// Example 12: Math.floor (Via C function call)
console.log("\n=== Example 12: Math.floor ===");
const nativeFloor = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.floor(x);
});

const floor3_7 = nativeFloor(3.7);
const floorNeg2_3 = nativeFloor(-2.3);
console.log(`Math.floor(3.7) = ${floor3_7}`);
console.log(`Math.floor(-2.3) = ${floorNeg2_3}`);

// Example 13: Math.ceil (Via C function call)
console.log("\n=== Example 13: Math.ceil ===");
const nativeCeil = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => {
    return Math.ceil(x);
});

const ceil3_2 = nativeCeil(3.2);
const ceilNeg2_7 = nativeCeil(-2.7);
console.log(`Math.ceil(3.2) = ${ceil3_2}`);
console.log(`Math.ceil(-2.7) = ${ceilNeg2_7}`);

// Example 14: Math.exp and Math.log
console.log("\n=== Example 14: Math.exp & Math.log ===");
const nativeExp = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.exp(x));

const nativeLog = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.log(x));

console.log(`Math.exp(1) = ${nativeExp(1).toFixed(4)} (should be ~2.7183)`);
console.log(`Math.log(2.7183) = ${nativeLog(2.7183).toFixed(4)} (should be ~1.0)`);

// Example 15: Math.asin and Math.acos
console.log("\n=== Example 15: Math.asin & Math.acos ===");
const nativeAsin = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.asin(x));

const nativeAcos = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.acos(x));

console.log(`Math.asin(0.5) = ${nativeAsin(0.5).toFixed(4)} (should be ~0.5236)`);
console.log(`Math.acos(0.5) = ${nativeAcos(0.5).toFixed(4)} (should be ~1.0472)`);

// Example 16: Math.round and Math.trunc
console.log("\n=== Example 16: Math.round & Math.trunc ===");
const nativeRound = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.round(x));

const nativeTrunc = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.trunc(x));

console.log(`Math.round(3.7) = ${nativeRound(3.7)}`);
console.log(`Math.round(3.2) = ${nativeRound(3.2)}`);
console.log(`Math.trunc(3.7) = ${nativeTrunc(3.7)}`);
console.log(`Math.trunc(-3.7) = ${nativeTrunc(-3.7)}`);

// Example 17: Math.pow (Multi-arg!)
console.log("\n=== Example 17: Math.pow (2 args) ===");
const nativePow = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (base, exp) => Math.pow(base, exp));

console.log(`Math.pow(2, 3) = ${nativePow(2, 3)}`);
console.log(`Math.pow(5, 2) = ${nativePow(5, 2)}`);
console.log(`Math.pow(10, 0.5) = ${nativePow(10, 0.5).toFixed(4)} (should be ~3.1623)`);

// Example 18: Math.atan2 (Multi-arg!)
console.log("\n=== Example 18: Math.atan2 (2 args) ===");
const nativeAtan2 = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (y, x) => Math.atan2(y, x));

console.log(`Math.atan2(1, 1) = ${nativeAtan2(1, 1).toFixed(4)} (should be ~0.7854 = PI/4)`);
console.log(`Math.atan2(1, 0) = ${nativeAtan2(1, 0).toFixed(4)} (should be ~1.5708 = PI/2)`);

// Example 19: Math.hypot (Multi-arg!)
console.log("\n=== Example 19: Math.hypot (2 args) ===");
const nativeHypot = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (x, y) => Math.hypot(x, y));

console.log(`Math.hypot(3, 4) = ${nativeHypot(3, 4)} (should be 5)`);
console.log(`Math.hypot(5, 12) = ${nativeHypot(5, 12)} (should be 13)`);

// Example 20: Math.cbrt (cube root)
console.log("\n=== Example 20: Math.cbrt ===");
const nativeCbrt = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.cbrt(x));

console.log(`Math.cbrt(8) = ${nativeCbrt(8)}`);
console.log(`Math.cbrt(27) = ${nativeCbrt(27)}`);

// Example 21: Math.min (inline FPU comparison)
console.log("\n=== Example 21: Math.min (inline FPU) ===");

const nativeMin2 = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (a, b) => Math.min(a, b));

console.log(`Math.min(5, 3) = ${nativeMin2(5, 3)} (should be 3)`);
console.log(`Math.min(-2, 7) = ${nativeMin2(-2, 7)} (should be -2)`);
console.log(`Math.min(10.5, 3.2) = ${nativeMin2(10.5, 3.2)} (should be 3.2)`);

// Example 22: Math.max (inline FPU comparison)
console.log("\n=== Example 22: Math.max (inline FPU) ===");

const nativeMax2 = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (a, b) => Math.max(a, b));

console.log(`Math.max(5, 3) = ${nativeMax2(5, 3)} (should be 5)`);
console.log(`Math.max(-2, 7) = ${nativeMax2(-2, 7)} (should be 7)`);
console.log(`Math.max(-10.5, -3.2) = ${nativeMax2(-10.5, -3.2)} (should be -3.2)`);

// Example 23: Variadic Math.min (4 args)
console.log("\n=== Example 23: Variadic Math.min (4 args) ===");

const nativeMin4 = Native.compile({
    args: ['float', 'float', 'float', 'float'],
    returns: 'float'
}, (a, b, c, d) => Math.min(a, b, c, d));

console.log(`Math.min(5, 3, 8, 1) = ${nativeMin4(5, 3, 8, 1)} (should be 1)`);
console.log(`Math.min(10, 20, 5, 15) = ${nativeMin4(10, 20, 5, 15)} (should be 5)`);

// Example 24: Variadic Math.max (4 args)
console.log("\n=== Example 24: Variadic Math.max (4 args) ===");

const nativeMax4 = Native.compile({
    args: ['float', 'float', 'float', 'float'],
    returns: 'float'
}, (a, b, c, d) => Math.max(a, b, c, d));

console.log(`Math.max(5, 3, 8, 1) = ${nativeMax4(5, 3, 8, 1)} (should be 8)`);
console.log(`Math.max(10, 20, 5, 15) = ${nativeMax4(10, 20, 5, 15)} (should be 20)`);

// Example 25: Variadic Math.min (6 args - leaves room for intermediates)
console.log("\n=== Example 25: Variadic Math.min (6 args) ===");

const nativeMin6 = Native.compile({
    args: ['float', 'float', 'float', 'float', 'float', 'float'],
    returns: 'float'
}, (a, b, c, d, e, f) => Math.min(a, b, c, d, e, f));

console.log(`Math.min(10, 5, 20, 3, 15, 8) = ${nativeMin6(10, 5, 20, 3, 15, 8)} (should be 3)`);

// Example 26: Variadic Math.max (6 args - leaves room for intermediates)
console.log("\n=== Example 26: Variadic Math.max (6 args) ===");

const nativeMax6 = Native.compile({
    args: ['float', 'float', 'float', 'float', 'float', 'float'],
    returns: 'float'
}, (a, b, c, d, e, f) => Math.max(a, b, c, d, e, f));

console.log(`Math.max(10, 5, 20, 3, 15, 8) = ${nativeMax6(10, 5, 20, 3, 15, 8)} (should be 20)`);

// Math.clamp(value, min, max) - Using inline R5900 MIN.S/MAX.S
console.log("\n--- Math.clamp Test ---");

const nativeClamp = Native.compile({
    args: ['float', 'float', 'float'],
    returns: 'float'
}, (value, min, max) => Math.clamp(value, min, max));

console.log(`Math.clamp(5, 0, 10) = ${nativeClamp(5, 0, 10)} (should be 5 - within bounds)`);
console.log(`Math.clamp(-5, 0, 10) = ${nativeClamp(-5, 0, 10)} (should be 0 - below min)`);
console.log(`Math.clamp(15, 0, 10) = ${nativeClamp(15, 0, 10)} (should be 10 - above max)`);
console.log(`Math.clamp(0, 0, 0) = ${nativeClamp(0, 0, 0)} (should be 0 - all equal)`);
console.log(`Math.clamp(7.5, 2.5, 12.5) = ${nativeClamp(7.5, 2.5, 12.5)} (should be 7.5)`);

// Math.sign(x) - Using inline FPU comparisons
console.log("\n--- Math.sign Test ---");

const nativeSign = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.sign(x));

console.log(`Math.sign(5) = ${nativeSign(5)} (should be 1 - positive)`);
console.log(`Math.sign(-5) = ${nativeSign(-5)} (should be -1 - negative)`);
console.log(`Math.sign(0) = ${nativeSign(0)} (should be 0 - zero)`);
console.log(`Math.sign(0.5) = ${nativeSign(0.5)} (should be 1 - positive float)`);
console.log(`Math.sign(-0.5) = ${nativeSign(-0.5)} (should be -1 - negative float)`);

// Math.fround(x) - Round to float32 (noop on PS2)
console.log("\n--- Math.fround Test ---");

const nativeFround = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.fround(x));

console.log(`Math.fround(1.5) = ${nativeFround(1.5)} (should be 1.5)`);
console.log(`Math.fround(1.337) = ${nativeFround(1.337)} (should be ~1.337)`);

// Math.imul(a, b) - 32-bit integer multiply
console.log("\n--- Math.imul Test ---");

const nativeImul = Native.compile({
    args: ['int', 'int'],
    returns: 'int'
}, (a, b) => Math.imul(a, b));

console.log(`Math.imul(2, 4) = ${nativeImul(2, 4)} (should be 8)`);
console.log(`Math.imul(-5, 3) = ${nativeImul(-5, 3)} (should be -15)`);
console.log(`Math.imul(0xFFFFFFFF, 5) = ${nativeImul(0xFFFFFFFF, 5)} (should be -5)`);

// Math.fma(a, b, c) - Fused multiply-add: a*b+c
console.log("\n--- Math.fma Test ---");

const nativeFma = Native.compile({
    args: ['float', 'float', 'float'],
    returns: 'float'
}, (a, b, c) => Math.fma(a, b, c));

console.log(`Math.fma(2, 3, 4) = ${nativeFma(2, 3, 4)} (should be 10 - 2*3+4)`);
console.log(`Math.fma(1.5, 2.0, 0.5) = ${nativeFma(1.5, 2.0, 0.5)} (should be 3.5 - 1.5*2+0.5)`);
console.log(`Math.fma(-1, 5, 3) = ${nativeFma(-1, 5, 3)} (should be -2 - -1*5+3)`);

// Math.lerp(a, b, t) - Linear interpolation
console.log("\n--- Math.lerp Test ---");

const nativeLerp = Native.compile({
    args: ['float', 'float', 'float'],
    returns: 'float'
}, (a, b, t) => Math.lerp(a, b, t));

console.log(`Math.lerp(0, 10, 0) = ${nativeLerp(0, 10, 0)} (should be 0)`);
console.log(`Math.lerp(0, 10, 0.5) = ${nativeLerp(0, 10, 0.5)} (should be 5)`);
console.log(`Math.lerp(0, 10, 1) = ${nativeLerp(0, 10, 1)} (should be 10)`);
console.log(`Math.lerp(-5, 5, 0.5) = ${nativeLerp(-5, 5, 0.5)} (should be 0)`);

// Math.saturate(x) - Clamp to [0, 1]
console.log("\n--- Math.saturate Test ---");

const nativeSaturate = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.saturate(x));

console.log(`Math.saturate(-0.5) = ${nativeSaturate(-0.5)} (should be 0)`);
console.log(`Math.saturate(0.5) = ${nativeSaturate(0.5)} (should be 0.5)`);
console.log(`Math.saturate(1.5) = ${nativeSaturate(1.5)} (should be 1)`);

// Math.step(edge, x) - Step function
console.log("\n--- Math.step Test ---");

const nativeStep = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (edge, x) => Math.step(edge, x));

console.log(`Math.step(0.5, 0.3) = ${nativeStep(0.5, 0.3)} (should be 0 - x < edge)`);
console.log(`Math.step(0.5, 0.5) = ${nativeStep(0.5, 0.5)} (should be 1 - x >= edge)`);
console.log(`Math.step(0.5, 0.7) = ${nativeStep(0.5, 0.7)} (should be 1 - x >= edge)`);

// Math.smoothstep(e0, e1, x) - Smooth Hermite interpolation
console.log("\n--- Math.smoothstep Test ---");

const nativeSmoothstep = Native.compile({
    args: ['float', 'float', 'float'],
    returns: 'float'
}, (e0, e1, x) => Math.smoothstep(e0, e1, x));

console.log(`Math.smoothstep(0, 1, 0) = ${nativeSmoothstep(0, 1, 0)} (should be 0)`);
console.log(`Math.smoothstep(0, 1, 0.5) = ${nativeSmoothstep(0, 1, 0.5)} (should be 0.5)`);
console.log(`Math.smoothstep(0, 1, 1) = ${nativeSmoothstep(0, 1, 1)} (should be 1)`);
console.log(`Math.smoothstep(0, 10, 5) = ${nativeSmoothstep(0, 10, 5)} (should be 0.5)`);

// Math.rsqrt(x) - Reciprocal square root: 1/sqrt(x)
console.log("\n--- Math.rsqrt Test ---");

const nativeRsqrt = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.rsqrt(x));

console.log(`Math.rsqrt(1) = ${nativeRsqrt(1)} (should be 1)`);
console.log(`Math.rsqrt(4) = ${nativeRsqrt(4)} (should be 0.5)`);
console.log(`Math.rsqrt(0.25) = ${nativeRsqrt(0.25)} (should be 2)`);

// Native.disassemble() test
console.log("\n--- Native.disassemble Test ---");

const simpleAdd = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (a, b) => a + b);

console.log("Disassembly of (a, b) => a + b:");
console.log(Native.disassemble(simpleAdd));

// Int64 Tests
console.log("\n=== Int64 Tests ===");
console.log("Note: Int64 support includes DADD, DSUB, shifts (DSLL/DSRL/DSRA)");
console.log("DMULT/DDIV not available on R5900 - would need software emulation\n");

// Test 1: Int64 addition (using int32 values for now)
console.log("--- Test 1: Int64 Addition ---");
const int64Add = Native.compile({
    args: ['int64', 'int64'],
    returns: 'int64'
}, (a, b) => a + b);

// Using values that fit in int32 for initial testing
console.log(`int64Add(100, 200) = ${int64Add(100, 200)} (should be 300)`);
console.log(`int64Add(-500, 300) = ${int64Add(-500, 300)} (should be -200)`);

// Test 2: Int64 subtraction
console.log("\n--- Test 2: Int64 Subtraction ---");
const int64Sub = Native.compile({
    args: ['int64', 'int64'],
    returns: 'int64'
}, (a, b) => a - b);

console.log(`int64Sub(1000, 300) = ${int64Sub(1000, 300)} (should be 700)`);
console.log(`int64Sub(100, 500) = ${int64Sub(100, 500)} (should be -400)`);

// Test 3: Int64 negation
console.log("\n--- Test 3: Int64 Negation ---");
const int64Neg = Native.compile({
    args: ['int64'],
    returns: 'int64'
}, (a) => -a);

console.log(`int64Neg(42) = ${int64Neg(42)} (should be -42)`);
console.log(`int64Neg(-123) = ${int64Neg(-123)} (should be 123)`);

// Test 4: Int64 left shift
console.log("\n--- Test 4: Int64 Left Shift ---");
const int64Shl = Native.compile({
    args: ['int64', 'int'],
    returns: 'int64'
}, (a, b) => a << b);

console.log(`int64Shl(1, 10) = ${int64Shl(1, 10)} (should be 1024)`);
console.log(`int64Shl(5, 4) = ${int64Shl(5, 4)} (should be 80)`);

// Test 5: I32 to I64 conversion
console.log("\n--- Test 5: Int32 to Int64 Conversion ---");
const i32ToI64 = Native.compile({
    args: ['int'],
    returns: 'int64'
}, (a) => a);

console.log(`i32ToI64(42) = ${i32ToI64(42)} (should be 42)`);
console.log(`i32ToI64(-100) = ${i32ToI64(-100)} (should be -100)`);

// Test 6: I64 to I32 truncation
console.log("\n--- Test 6: Int64 to Int32 Truncation ---");
const i64ToI32 = Native.compile({
    args: ['int64'],
    returns: 'int'
}, (a) => a);

console.log(`i64ToI32(12345) = ${i64ToI32(12345)} (should be 12345)`);

console.log("\nInt64 tests completed!");
console.log("Hardware note: R5900 has native DADD/DSUB but no DMULT/DDIV");

// Native.struct Tests
console.log("\n=== Native.struct Tests ===");

// Test 1: Basic struct definition
console.log("\n--- Test 1: Basic Struct Definition ---");
const Vec3 = Native.struct({
    x: 'float',
    y: 'float',
    z: 'float'
});
console.log(`Vec3.size = ${Vec3.size} (should be 12)`);

// Test 2: Create instance and access fields
console.log("\n--- Test 2: Field Access ---");
const pos = new Vec3();
pos.x = 1.5;
pos.y = 2.5;
pos.z = 3.5;
console.log(`pos.x = ${pos.x} (should be 1.5)`);
console.log(`pos.y = ${pos.y} (should be 2.5)`);
console.log(`pos.z = ${pos.z} (should be 3.5)`);

// Test 3: Different field types
console.log("\n--- Test 3: Mixed Types ---");
const Entity = Native.struct({
    id: 'int',
    health: 'float',
    flags: 'uint'
});
console.log(`Entity.size = ${Entity.size} (should be 12)`);

const player = new Entity();
player.id = 42;
player.health = 100.5;
player.flags = 0xFF00FF00;
console.log(`player.id = ${player.id} (should be 42)`);
console.log(`player.health = ${player.health} (should be ~100.5)`);
console.log(`player.flags = ${player.flags} (should be 4278255360)`);

// Test 4: Array fields (NEW: Element access)
console.log("\n--- Test 4: Array Fields ---");
const Matrix4 = Native.struct({
    m: 'float[16]'
});
console.log(`Matrix4.size = ${Matrix4.size} (should be 64)`);

// Create instance and test array access
const mat = new Matrix4();

// Write some values
mat.m[0] = 1.0;   // Identity diagonal
mat.m[5] = 1.0;
mat.m[10] = 1.0;
mat.m[15] = 1.0;
mat.m[12] = 10.5; // Translation X
mat.m[13] = 20.5; // Translation Y
mat.m[14] = 30.5; // Translation Z

// Read back and verify
console.log(`mat.m[0] = ${mat.m[0]} (should be 1.0)`);
console.log(`mat.m[5] = ${mat.m[5]} (should be 1.0)`);
console.log(`mat.m[12] = ${mat.m[12]} (should be 10.5)`);
console.log(`mat.m[13] = ${mat.m[13]} (should be 20.5)`);
console.log(`mat.m.length = ${mat.m.length} (should be 16)`);

// Test with int array
const IntArray = Native.struct({
    data: 'int[4]'
});
const ints = new IntArray();
ints.data[0] = 100;
ints.data[1] = 200;
ints.data[2] = 300;
ints.data[3] = 400;
console.log(`ints.data[2] = ${ints.data[2]} (should be 300)`);

// Test array access inside native compiled code
console.log("\n--- Test 4b: Arrays in Native Code ---");

// Sum all elements of a float array
const sumArray = Native.compile({
    args: [Matrix4],
    returns: 'float'
}, (mat) => {
    let sum = 0.0;
    sum = sum + mat.m[0];
    sum = sum + mat.m[1];
    sum = sum + mat.m[2];
    sum = sum + mat.m[3];
    return sum;
});
Native.disassemble(sumArray);

// Set mat values
mat.m[0] = 1.0;
mat.m[1] = 2.0;
mat.m[2] = 3.0;
mat.m[3] = 4.0;
const arraySum = sumArray(mat);
console.log(`sumArray(mat) = ${arraySum} (should be 10.0)`);

// Write to array inside native
const clearMatrix = Native.compile({
    args: [Matrix4],
    returns: 'void'
}, (mat) => {
    mat.m[0] = 0.0;
    mat.m[1] = 0.0;
    mat.m[2] = 0.0;
    mat.m[3] = 0.0;
});
Native.disassemble(clearMatrix);

clearMatrix(mat);
console.log(`After clearMatrix: mat.m[0] = ${mat.m[0]} (should be 0)`);

console.log("\nNative.struct tests completed!");

// Test 5: Struct in Native.compile
console.log("\n--- Test 5: Struct in Native Functions ---");

// Create struct for vector with FLOAT fields
const Vec2 = Native.struct({
    x: 'float',
    y: 'float'
});

// Full test with field access and operations
const addVec2 = Native.compile({
    args: [Vec2, Vec2, Vec2],
    returns: 'void'
}, (a, b, out) => {
    out.x = a.x + b.x;
    out.y = a.y + b.y;
});

console.log("addVec2 compiled");
Native.disassemble(addVec2);

// Create struct instances with actual float values
const v1 = new Vec2();
const v2 = new Vec2();
const vout = new Vec2();

v1.x = 1.5; v1.y = 2.5;
v2.x = 3.5; v2.y = 4.5;
vout.x = 0; vout.y = 0;

console.log("Calling addVec2...");
addVec2(v1, v2, vout);

console.log(`vout.x = ${vout.x} (should be 5.0)`);
console.log(`vout.y = ${vout.y} (should be 7.0)`);

console.log("\nNative compiler test completed!");

// === Test 6: Struct Methods ===
console.log("\n--- Test 6: Native Struct Methods ---");

// Define Vec3 with NATIVE compiled methods
const Vec3Native = Native.struct({
    x: 'float',
    y: 'float',
    z: 'float'
}, {
    // Native compiled method - 'self' is the struct type (resolved at binding)
    scale: Native.compile({
        args: ['self', 'float'],  // 'self' = struct ptr, deferred compilation
        returns: 'void'
    }, function (self, factor) {
        self.x = self.x * factor;
        self.y = self.y * factor;
        self.z = self.z * factor;
    }),

    // Another native method - only 'self' arg
    zero: Native.compile({
        args: ['self'],  // only self (struct ptr)
        returns: 'void'
    }, function (self) {
        self.x = 0.0;
        self.y = 0.0;
        self.z = 0.0;
    }),

    // JS fallback method (for complex logic)
    toString: function () {
        return `(${this.x}, ${this.y}, ${this.z})`;
    }
});

// Test 6a: scale() native method
const v3 = new Vec3Native();
v3.x = 1.0; v3.y = 2.0; v3.z = 3.0;
console.log(`Before scale: v3 = ${v3.toString()}`);
v3.scale(2.0);
console.log(`After scale(2): v3.x = ${v3.x} (should be 2.0)`);
console.log(`After scale(2): v3.y = ${v3.y} (should be 4.0)`);
console.log(`After scale(2): v3.z = ${v3.z} (should be 6.0)`);

// Test 6b: zero() native method
const v3z = new Vec3Native();
v3z.x = 10.0; v3z.y = 20.0; v3z.z = 30.0;
console.log(`Before zero: v3z = ${v3z.toString()}`);
v3z.zero();
console.log(`After zero: v3z.x = ${v3z.x} (should be 0.0)`);
console.log(`After zero: v3z.y = ${v3z.y} (should be 0.0)`);
console.log(`After zero: v3z.z = ${v3z.z} (should be 0.0)`);

// Test 6c: JS fallback method (toString)
const v3s = new Vec3Native();
v3s.x = 1.5; v3s.y = 2.5; v3s.z = 3.5;
console.log(`toString() = ${v3s.toString()} (should be (1.5, 2.5, 3.5))`);


console.log("\nNative struct methods test completed!");

// ============================================
// Phase 1: Int64 Emulation Tests
// ============================================
console.log("\n=== Phase 1: Int64 Emulation Tests ===");

// Test 1: Int64 Multiplication (Emulated)
console.log("\n--- Test 1: Int64 Multiply (Emulated) ---");

const int64Mult = Native.compile({
    args: ['int64', 'int64'],
    returns: 'int64'
}, (a, b) => a * b);

console.log(`int64: 1000 * 2000 = ${int64Mult(1000, 2000)} (should be 2000000)`);
console.log(`int64: -500 * 500 = ${int64Mult(-500, 500)} (should be -250000)`);
console.log(`int64: 123456 * 789 = ${int64Mult(123456, 789)} (should be 97406784)`);

// Test 2: Int64 Division (Emulated)
console.log("\n--- Test 2: Int64 Divide (Emulated) ---");

const int64Div = Native.compile({
    args: ['int64', 'int64'],
    returns: 'int64'
}, (a, b) => a / b);

console.log(`int64: 1000000 / 1000 = ${int64Div(1000000, 1000)} (should be 1000)`);
console.log(`int64: -10000 / 100 = ${int64Div(-10000, 100)} (should be -100)`);
console.log(`int64: 999999 / 7 = ${int64Div(999999, 7)} (should be 142857)`);

// Test 3: Int64 Modulo (Emulated)
console.log("\n--- Test 3: Int64 Modulo (Emulated) ---");

const int64Mod = Native.compile({
    args: ['int64', 'int64'],
    returns: 'int64'
}, (a, b) => a % b);

console.log(`int64: 1000 % 7 = ${int64Mod(1000, 7)} (should be 6)`);
console.log(`int64: 999999 % 100 = ${int64Mod(999999, 100)} (should be 99)`);
console.log(`int64: -25 % 7 = ${int64Mod(-25, 7)} (should be -4)`);

// Test 4: Mixed Int32/Int64 Operations
console.log("\n--- Test 4: Mixed Int Types ---");

const mixedIntOps = Native.compile({
    args: ['int', 'int64'],
    returns: 'int64'
}, (a, b) => {
    // a is int32, b is int64
    // Should auto-promote a to int64 for operations
    return a + b;
});

console.log(`Mixed: 100 + 200L = ${mixedIntOps(100, 200)} (should be 300)`);

// ============================================
// Phase 1: String Operations Tests
// ============================================
console.log("\n=== Phase 1: String Operations Tests ===");

// Test: String concatenation
const strConcat = Native.compile({
    args: ['string', 'string'],
    returns: 'string'
}, (a, b) => a + b);

const str1 = "Hello, ";
const str2 = "World!";
console.log(`Concat: "${strConcat(str1, str2)}" (should be "Hello, World!")`);

// Test: String + String (multiple)
const str3 = "Athena";
const str4 = "Env";
console.log(`Concat2: "${strConcat(str3, str4)}" (should be "AthenaEnv")`);

// Test: Empty string concatenation
console.log(`Empty + "test": "${strConcat("", "test")}" (should be "test")`);
console.log(`"test" + "": "${strConcat("test", "")}" (should be "test")`);

console.log("\n=== Phase 1 Tests Completed ===");
console.log("Implemented: Int64 emulation (mult/div/mod), String concatenation");
console.log("Runtime: native_string_concat(), __dmult_i64(), __ddiv_i64(), __dmod_i64()");

// ============================================
// Phase 2: Extended String Operations
// ============================================
console.log("\n=== Phase 2: Extended String Operations ===");

// String literals inside compiled code
console.log("\n--- String Literals ---");
const strLiteral = Native.compile({
    args: [],
    returns: 'string'
}, () => "Hello" + " " + "Native");

console.log(`Literal concat: "${strLiteral()}" (should be "Hello Native")`);

const strEmptyLiteral = Native.compile({
    args: [],
    returns: 'string'
}, () => "" + "empty");

console.log(`Empty literal: "${strEmptyLiteral()}" (should be "empty")`);

// String.length
console.log("\n--- String.length ---");
const strLength = Native.compile({
    args: ['string'],
    returns: 'int'
}, (s) => s.length);

console.log(`length("Hello") = ${strLength("Hello")} (should be 5)`);
console.log(`length("") = ${strLength("")} (should be 0)`);
console.log(`length("AthenaEnv") = ${strLength("AthenaEnv")} (should be 9)`);

// String.slice
console.log("\n--- String.slice ---");
const strSlice = Native.compile({
    args: ['string', 'int', 'int'],
    returns: 'string'
}, (s, start, end) => s.slice(start, end));

console.log(`slice("Hello", 1, 4) = "${strSlice("Hello", 1, 4)}" (should be "ell")`);
console.log(`slice("AthenaEnv", 0, 6) = "${strSlice("AthenaEnv", 0, 6)}" (should be "Athena")`);

// String.indexOf
console.log("\n--- String.indexOf ---");
const strIndexOf = Native.compile({
    args: ['string', 'string'],
    returns: 'int'
}, (s, needle) => s.indexOf(needle));

console.log(`indexOf("Hello World", "World") = ${strIndexOf("Hello World", "World")} (should be 6)`);
console.log(`indexOf("abcabc", "bc") = ${strIndexOf("abcabc", "bc")} (should be 1)`);
console.log(`indexOf("abc", "z") = ${strIndexOf("abc", "z")} (should be -1)`);

// String.toUpperCase / toLowerCase
console.log("\n--- String Case Conversion ---");
const strUpper = Native.compile({
    args: ['string'],
    returns: 'string'
}, (s) => s.toUpperCase());

const strLower = Native.compile({
    args: ['string'],
    returns: 'string'
}, (s) => s.toLowerCase());

console.log(`toUpperCase("Hello") = "${strUpper("Hello")}" (should be "HELLO")`);
console.log(`toLowerCase("AthenaEnv") = "${strLower("AthenaEnv")}" (should be "athenaenv")`);

// String.trim
console.log("\n--- String.trim ---");
const strTrim = Native.compile({
    args: ['string'],
    returns: 'string'
}, (s) => s.trim());

console.log(`trim("  hi  ") = "${strTrim("  hi  ")}" (should be "hi")`);

// String.replace
console.log("\n--- String.replace ---");
const strReplace = Native.compile({
    args: ['string', 'string', 'string'],
    returns: 'string'
}, (s, find, repl) => s.replace(find, repl));

console.log(`replace("foo-bar-baz", "-", "_") = "${strReplace("foo-bar-baz", "-", "_")}" (should be "foo_bar-baz")`);

// String equality / inequality
console.log("\n--- String == / != ---");
const strEquals = Native.compile({
    args: ['string', 'string'],
    returns: 'bool'
}, (a, b) => a == b);

const strNotEquals = Native.compile({
    args: ['string', 'string'],
    returns: 'bool'
}, (a, b) => a != b);

console.log(`"abc" == "abc" = ${strEquals("abc", "abc")} (should be true)`);
console.log(`"abc" == "def" = ${strEquals("abc", "def")} (should be false)`);
console.log(`"abc" != "def" = ${strNotEquals("abc", "def")} (should be true)`);
console.log(`"abc" != "abc" = ${strNotEquals("abc", "abc")} (should be false)`);

// ============================================
// Phase 3: Comparison Correctness
// ============================================
console.log("\n=== Phase 3: Comparison Correctness ===");

// Float comparisons (negative values — previously broken with integer SLT)
console.log("\n--- Float Comparisons ---");
const floatCompare = Native.compile({
    args: ['float', 'float'],
    returns: 'bool'
}, (a, b) => a < b);

const floatGreater = Native.compile({
    args: ['float', 'float'],
    returns: 'bool'
}, (a, b) => a > b);

const floatEquals = Native.compile({
    args: ['float', 'float'],
    returns: 'bool'
}, (a, b) => a == b);

console.log(`-2.0 < -1.0 = ${floatCompare(-2.0, -1.0)} (should be true)`);
console.log(`-1.0 < -2.0 = ${floatCompare(-1.0, -2.0)} (should be false)`);
console.log(`3.14 > 2.71 = ${floatGreater(3.14, 2.71)} (should be true)`);
console.log(`1.0 == 1.0 = ${floatEquals(1.0, 1.0)} (should be true)`);

// Unsigned comparisons (values above 0x7FFFFFFF)
console.log("\n--- Unsigned Comparisons ---");
const uintCompare = Native.compile({
    args: ['uint', 'uint'],
    returns: 'bool'
}, (a, b) => a < b);

const uintGreater = Native.compile({
    args: ['uint', 'uint'],
    returns: 'bool'
}, (a, b) => a > b);

const bigA = 0x80000001;  // 2147483649
const bigB = 0x80000000;  // 2147483648
const bigC = 0x80000002;  // 2147483650

console.log(`0x80000001 < 0x80000002 = ${uintCompare(bigA, bigC)} (should be true)`);
console.log(`0x80000001 > 0x80000000 = ${uintGreater(bigA, bigB)} (should be true)`);
console.log(`0x80000000 < 0x80000001 = ${uintCompare(bigB, bigA)} (should be true)`);

// Int64 comparisons
console.log("\n--- Int64 Comparisons ---");
const int64Less = Native.compile({
    args: ['int64', 'int64'],
    returns: 'bool'
}, (a, b) => a < b);

const int64Equals = Native.compile({
    args: ['int64', 'int64'],
    returns: 'bool'
}, (a, b) => a == b);

const int64Greater = Native.compile({
    args: ['int64', 'int64'],
    returns: 'bool'
}, (a, b) => a > b);

console.log(`100 < 200 = ${int64Less(100, 200)} (should be true)`);
console.log(`500 == 500 = ${int64Equals(500, 500)} (should be true)`);
console.log(`1000 > 999 = ${int64Greater(1000, 999)} (should be true)`);
console.log(`-100 < 50 = ${int64Less(-100, 50)} (should be true)`);

// Bool return type
console.log("\n--- Bool Return Type ---");
const isEven = Native.compile({
    args: ['int'],
    returns: 'bool'
}, (n) => (n % 2) == 0);

console.log(`isEven(4) = ${isEven(4)} (should be true)`);
console.log(`isEven(7) = ${isEven(7)} (should be false)`);

// ============================================
// Phase 4: Nested Native Calls
// ============================================
console.log("\n=== Phase 4: Nested Native Calls ===");

const nativeDouble = Native.compile({
    args: ['int'],
    returns: 'int'
}, (x) => x * 2);

const nativeQuadruple = Native.compile({
    args: ['int'],
    returns: 'int'
}, (x) => nativeDouble(nativeDouble(x)));

const nativeAddTen = Native.compile({
    args: ['int'],
    returns: 'int'
}, (x) => x + 10);

const nativeDoublePlusTen = Native.compile({
    args: ['int'],
    returns: 'int'
}, (x) => nativeAddTen(nativeDouble(x)));

console.log(`quadruple(5) = ${nativeQuadruple(5)} (should be 20)`);
console.log(`quadruple(0) = ${nativeQuadruple(0)} (should be 0)`);
console.log(`doublePlusTen(3) = ${nativeDoublePlusTen(3)} (should be 16)`);

// Nested float helper
const nativeSqrtHalf = Native.compile({
    args: ['float'],
    returns: 'float'
}, (x) => Math.sqrt(x) * 0.5);

const nativeHypotHalf = Native.compile({
    args: ['float', 'float'],
    returns: 'float'
}, (a, b) => nativeSqrtHalf(a * a + b * b));

console.log(`hypotHalf(3, 4) = ${nativeHypotHalf(3.0, 4.0).toFixed(4)} (should be 2.5)`);

// ============================================
// Phase 5: Dynamic Array API (pointer arg)
// ============================================
console.log("\n=== Phase 5: Dynamic Array API ===");
console.log("Dynamic arrays use DynamicInt32Array/Uint32Array/Float32Array pointer args.");
console.log("Pass a NativeDynamicArray* as an integer pointer when a JS constructor is available.");

// Compile helpers — run when a valid array pointer is provided
const dynArrayPushAndLen = Native.compile({
    args: ['DynamicInt32Array', 'int'],
    returns: 'int'
}, (arr, val) => {
    arr.push(val);
    return arr.length;
});

const dynArrayGetSet = Native.compile({
    args: ['DynamicInt32Array', 'int', 'int'],
    returns: 'int'
}, (arr, idx, val) => {
    arr[idx] = val;
    return arr[idx];
});

const dynArrayPop = Native.compile({
    args: ['DynamicInt32Array'],
    returns: 'int'
}, (arr) => {
    return arr.pop();
});

const dynFloatPush = Native.compile({
    args: ['DynamicFloat32Array', 'float'],
    returns: 'int'
}, (arr, val) => {
    arr.push(val);
    return arr.length;
});

console.log("Compiled: dynArrayPushAndLen, dynArrayGetSet, dynArrayPop, dynFloatPush");

// Runtime test using the host-provided dynamic array API
const intArr = Native.createDynamicArray('int', 8);
console.log(`push 10 -> length ${dynArrayPushAndLen(intArr, 10)} (should be 1)`);
console.log(`push 20 -> length ${dynArrayPushAndLen(intArr, 20)} (should be 2)`);
console.log(`push 30 -> length ${dynArrayPushAndLen(intArr, 30)} (should be 3)`);
console.log(`getSet [1]=99 -> ${dynArrayGetSet(intArr, 1, 99)} (should be 99)`);
console.log(`element [1] readback = ${Native.dynArrayGet(intArr, 1)} (should be 99)`);
console.log(`pop -> ${dynArrayPop(intArr)} (should be 30)`);
console.log(`length after pop = ${Native.dynArrayLength(intArr)} (should be 2)`);
Native.dynArrayFree(intArr);

const floatArr = Native.createDynamicArray('float', 4);
console.log(`float push 1.5 -> length ${dynFloatPush(floatArr, 1.5)} (should be 1)`);
console.log(`float push 2.5 -> length ${dynFloatPush(floatArr, 2.5)} (should be 2)`);
console.log(`float element [0] = ${Native.dynArrayGet(floatArr, 0)} (should be 1.5)`);
Native.dynArrayFree(floatArr);

// ============================================
// Summary
// ============================================
console.log("\n=== All Native Compiler Tests Completed ===");
console.log("Phase 2: String literals, methods, ==/!=");
console.log("Phase 3: Float/uint/int64 comparisons, bool");
console.log("Phase 4: Nested Native.compile() calls");
console.log("Phase 5: Dynamic array API (compile-only until JS constructor exists)");

System.sleep(9999999999);