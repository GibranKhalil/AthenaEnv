/**
 * AthenaEnv v2 Type Definitions for VS Code IntelliSense (Pure JavaScript)
 * Generated based on selected modules.
 */

declare namespace console {
    function log(...args: any[]): void;
    function warn(...args: any[]): void;
    function error(...args: any[]): void;
}

declare function setTimeout(handler: (...args: any[]) => void, timeout?: number, ...args: any[]): any;
declare function setInterval(handler: (...args: any[]) => void, timeout?: number, ...args: any[]): any;
declare function clearTimeout(handle?: any): void;
declare function clearInterval(handle?: any): void;
declare function setImmediate(handler: (...args: any[]) => void, ...args: any[]): any;
declare function clearImmediate(handle?: any): void;


/* === Module: Gamepad Controller (Pads) (pad) === */
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


/* === Module: System Core (system) === */
declare namespace System {
    /** The path from which the application booted (e.g. "mass0:/", "cdfs:/") */
    const bootPath: string;

    /** Returns raw CPU clock ticks */
    function getTicks(): number;

    /** Returns high-resolution elapsed time in milliseconds */
    function getMilliseconds(): number;

    /** Sleep the EE thread for the specified number of milliseconds */
    function sleep(ms: number): void;

    /** Returns currently used RAM in bytes */
    function getUsedMemory(): number;

    /** Returns remaining available RAM in bytes */
    function getFreeMemory(): number;

    /** Force QuickJS garbage collection */
    function gc(): void;

    /** Exit application to the PS2 browser/OSDSYS */
    function exit(): void;
}
