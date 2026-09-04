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
