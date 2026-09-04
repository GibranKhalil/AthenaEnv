// Initialize controller
const pad = Pads.get(0);

console.log("[Athena Game Loop] Started. Running with Pads & System modules...");
console.log(`[Athena] Connected pads: ${Pads.getConnected().length}`);

let frameCount = 0;
let lastTime = System.getMilliseconds();

while (frameCount < 10) {
    let now = System.getMilliseconds();
    let dt = now - lastTime;
    lastTime = now;

    frameCount++;

    pad.update();

    if (pad.justPressed(Pads.CROSS)) {
        console.log("[Pad] CROSS was pressed!");
    }

    console.log(
        `[Frame ${frameCount}] dt: ${dt.toFixed(2)}ms | RAM Free: ${(System.getFreeMemory() / 1024 / 1024).toFixed(2)}MB`
    );

    System.sleep(50);
}

console.log("[Athena Game Loop] Test loop completed.");