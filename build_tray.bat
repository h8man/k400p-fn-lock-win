rc res/app.rc
cl /EHsc res/app.res tray.cpp user32.lib shell32.lib gdi32.lib hidapi/x64/hidapi.lib /o dist/k400p-fn-tray.exe /DUNICODE /D_UNICODE /I hidapi/include /link /MACHINE:X64 