/**
 * AthenaEnv v2 Type Definitions for VS Code IntelliSense (Pure JavaScript)
 */

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
