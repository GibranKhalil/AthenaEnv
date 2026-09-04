let frameCount = 0;
let lastTime = System.getMilliseconds();

while (frameCount < 10) {
    let now = System.getMilliseconds();
    let dt = now - lastTime;
    lastTime = now;

    frameCount++;

    console.log(
        `[Frame ${frameCount}] dt: ${dt.toFixed(2)}ms`
    );

    System.sleep(100);
}