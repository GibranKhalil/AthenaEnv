declare interface Pad {
    /** Controller port index (0 or 1) */
    readonly port: number;

    /** Current button bitmask pressed */
    readonly btns: number;

    /** Left analog stick X axis (-127 to 127) */
    readonly lx: number;

    /** Left analog stick Y axis (-127 to 127) */
    readonly ly: number;

    /** Right analog stick X axis (-127 to 127) */
    readonly rx: number;

    /** Right analog stick Y axis (-127 to 127) */
    readonly ry: number;

    /** Previous frame button bitmask */
    readonly old_btns: number;

    /** Previous frame left analog X */
    readonly old_lx: number;

    /** Previous frame left analog Y */
    readonly old_ly: number;

    /** Previous frame right analog X */
    readonly old_rx: number;

    /** Previous frame right analog Y */
    readonly old_ry: number;

    /** Polls hardware and updates internal button and stick states for the current frame */
    update(): void;

    /** Checks if a specific button is currently held down */
    pressed(button: number): boolean;

    /** Checks if a button was pressed down in this exact frame (rising edge) */
    justPressed(button: number): boolean;
}

declare namespace Pads {
    /** Initialize controller subsystems */
    function init(): void;

    /** Get the Pad controller instance for the given port (0 for Port 1, 1 for Port 2) */
    function get(port?: number): Pad;

    /** Returns controller type (e.g. TYPE_DIGITAL, TYPE_DUALSHOCK) */
    function getType(port?: number): number;

    /** Returns controller connection state (e.g. STATE_STABLE, STATE_DISCONN) */
    function getState(port?: number): number;

    /** Returns true if controller is connected and ready to send data */
    function isActive(port?: number): boolean;

    /** Returns an array of connected port numbers (e.g. [0, 1]) */
    function getConnected(): number[];

    /** Returns number of connected controllers */
    function getConnectedCount(): number[];

    /** Trigger vibration motors (rumble) */
    function rumble(port: number, smallMotor: number, largeMotor: number): void;
    function rumble(smallMotor: number, largeMotor: number): void;

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
    const STATE_STABLE: number;
    const STATE_ERROR: number;
}
