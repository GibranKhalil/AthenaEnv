Pads.init();

function stateName(state) {
    switch (state) {
        case Pads.STATE_DISCONN:   return "DISCONN";
        case Pads.STATE_FINDCTP1:  return "FINDCTP1";
        case Pads.STATE_STABLE:    return "STABLE";
        case Pads.STATE_ERROR:     return "ERROR";
        default:                   return `UNKNOWN(${state})`;
    }
}

console.log("[Demo 5] Portas físicas:", Pads.getPortCount());
console.log("[Demo 5] Imprimindo estado bruto de todos os slots a cada ~60 frames...");

let frameCount = 0;
let running = true;

while (running) {
    frameCount++;
    Pads.update();

    if (frameCount % 60 === 0) {
        console.log(`--- Frame ${frameCount} ---`);
        for (let port = 0; port < Pads.getPortCount(); port++) {
            for (let slot = 0; slot < 4; slot++) {
                const state = Pads.getState(port, slot);
                const type = Pads.getType(port, slot);
                console.log(`  [${port}:${slot}] state=${stateName(state)} (${state}) type=${type}`);
            }
        }
    }

    for (let port = 0; port < Pads.getPortCount() && running; port++) {
        const pad = Pads.get(port, 0);
        if (pad.connected && pad.justPressed(Pads.START)) {
            console.log(`[Demo 5] START em ${port}:0, encerrando.`);
            running = false;
        }
    }

    System.sleep(16);
}

console.log("[Demo 5] Finalizado.");