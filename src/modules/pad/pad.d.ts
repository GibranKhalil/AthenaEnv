declare interface Pad {
    /** Controller port index (0 or 1) */
    readonly port: number;

    /** Controller slot index within the port (0-3, multitap) */
    readonly slot: number;

    /** True if the controller is currently connected and ready */
    readonly connected: boolean;

    /** Controller type (e.g. TYPE_DIGITAL, TYPE_DUALSHOCK) as of the last update() */
    readonly type: number;

    /** Current button bitmask pressed */
    readonly btns: number;

    /** Previous frame button bitmask */
    readonly old_btns: number;

    /** Left analog stick X axis, normalized (-1.0 to 1.0) */
    readonly lx: number;

    /** Left analog stick Y axis, normalized (-1.0 to 1.0) */
    readonly ly: number;

    /** Right analog stick X axis, normalized (-1.0 to 1.0) */
    readonly rx: number;

    /** Right analog stick Y axis, normalized (-1.0 to 1.0) */
    readonly ry: number;

    /** Previous frame left analog X, normalized */
    readonly old_lx: number;

    /** Previous frame left analog Y, normalized */
    readonly old_ly: number;

    /** Previous frame right analog X, normalized */
    readonly old_rx: number;

    /** Previous frame right analog Y, normalized */
    readonly old_ry: number;

    /**
     * Polls hardware and refreshes state for just this pad.
     * Optional convenience — Pads.update() refreshes every pad in one call
     * and is the recommended way to update once per frame.
     */
    update(): void;

    /** Checks if a specific button is currently held down */
    pressed(button: number): boolean;

    /** Checks if a button was pressed down in this exact frame (rising edge) */
    justPressed(button: number): boolean;

    /** Checks if a button was released in this exact frame (falling edge) */
    justReleased(button: number): boolean;

    /**
     * Analog pressure of a button (0-255), when the controller is in
     * DualShock press mode. SELECT, START, L3 and R3 have no pressure
     * sensor on the DS2 and always return 0.
     */
    pressure(button: number): number;

    /**
     * Triggers vibration on this pad.
     * The DS2 small motor only supports on/off (largeIntensity is the only
     * motor with variable strength, 0-255).
     */
    rumble(smallOn: boolean, largeIntensity: number): void;
}

declare namespace Pads {
    /** Initialize controller subsystems */
    function init(): void;

    /** Polls hardware and refreshes state for every pad. Call once per frame. */
    function update(): void;

    /** Closes all controller ports. Existing Pad instances become disconnected/zeroed. */
    function shutdown(): void;

    /** Get the singleton Pad instance for the given port and slot (multitap) */
    function get(port?: number, slot?: number): Pad;

    /** Returns controller type (e.g. TYPE_DIGITAL, TYPE_DUALSHOCK) */
    function getType(port?: number, slot?: number): number;

    /** Returns controller connection state (e.g. STATE_STABLE, STATE_DISCONN) */
    function getState(port?: number, slot?: number): number;

    /** Returns true if controller is connected and ready to send data */
    function isActive(port?: number, slot?: number): boolean;

    /** Returns the connected {port, slot} pairs, including multitap slots */
    function getConnected(): { port: number; slot: number }[];

    /** Returns number of connected controllers, across all ports/slots */
    function getConnectedCount(): number;

    /** Returns the number of physical controller ports (usually 2) */
    function getPortCount(): number;

    /** Returns the number of slots available on a port (>1 means multitap) */
    function getSlotCount(port: number): number;

    /** Triggers vibration on a specific pad. Port and slot are always required. */
    function rumble(port: number, slot: number, smallOn: boolean, largeIntensity: number): void;

    /* Buttons */
    const SELECT: number;
    const START: number;
    const UP: number;
    const RIGHT: number;
    const DOWN: number;
    const LEFT: number;
    const TRIANGLE: number;
    const CIRCLE: number;
    const CROSS: number;
    const SQUARE: number;
    const L1: number;
    const R1: number;
    const L2: number;
    const R2: number;
    const L3: number;
    const R3: number;

    /* Pad Types */
    const TYPE_DIGITAL: number;
    const TYPE_ANALOG: number;
    const TYPE_DUALSHOCK: number;

    /* States */
    const STATE_DISCONN: number;
    const STATE_FINDCTP1: number;
    const STATE_STABLE: number;
    const STATE_ERROR: number;
}