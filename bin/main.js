// AthenaEnv v2 - Minimal Core Game Loop Example

console.log("========================================");
console.log("   AthenaEnv v2 Core - Game Loop Test   ");
console.log("========================================");
console.log("[JS] Boot Path: " + System.bootPath);
console.log("[JS] Free Memory: " + (System.getFreeMemory() / (1024 * 1024)).toFixed(2) + " MB");
console.log("[JS] Used Memory: " + (System.getUsedMemory() / 1024).toFixed(2) + " KB");

let frameCount = 0;
let lastTime = System.getMilliseconds();

// Game loop demonstration
console.log("[JS] Starting game loop timer (10 frames demo)...");

let timer = setInterval(() => {
    let now = System.getMilliseconds();
    let dt = now - lastTime;
    lastTime = now;
    frameCount++;

    console.log(`[Frame ${frameCount}] dt: ${dt.toFixed(2)}ms | Free RAM: ${(System.getFreeMemory() / (1024 * 1024)).toFixed(2)} MB`);

    if (frameCount >= 10) {
        clearInterval(timer);
        console.log("========================================");
        console.log("   Game loop test completed safely!     ");
        console.log("========================================");
    }
}, 100);
