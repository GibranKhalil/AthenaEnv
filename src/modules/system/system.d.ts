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
