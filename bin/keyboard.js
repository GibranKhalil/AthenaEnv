// {"name": "Keyboard demo", "author": "Daniel Santos", "version": "04102023", "file": "keyboard.js"}

IOP.loadModule("ps2kbd");

Keyboard.init();

let cur_char = 0;

while(true) {
    cur_char = Keyboard.get();
    if (cur_char != 0) {
        console.log(`pressed ${String.fromCharCode(cur_char)}`);
    }
    
}