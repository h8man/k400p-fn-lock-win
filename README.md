# k400p-fn-lock-for-windows
 Lock Fn keys on K400+ (for windows)

## Goal
This program locks the fn key for the Logitech K400+ bluetooth keyboard on Windows. 
Having the F1-F12 without pressing the Fn key is nice for developers.

I wanted a simple way to do the same as the Logitech Options/Options+ softwares WITHOUT their continuously running processes.

## Download
Precompiled binaries available here :
https://github.com/dheygere/k400p-fn-lock-for-windows/releases/tag/v1.0.0

## How to use
Running `k400p-fn-lock.exe` will lock Fn key till next reboot.  
Revert with `k400p-fn-unlocklock.exe` to unlock and restore media keys.

## How to build

- build with gcc:   
    `gcc main.c -o dist/k400p-fn-lock.exe -I hidapi/include -L hidapi/x86 -lhidapi`  
    `gcc main.c -D setMediaKeys -o dist/k400p-fn-unlock.exe -I hidapi/include -L hidapi/x86 -lhidapi`
- build with msvc:
    `build.bat`

# Inspiration
code from : https://github.com/dheygere/k380-fn-lock-for-windows
values from : https://github.com/sginne/fn_key_k400_for_logitech
