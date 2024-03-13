cl  main.c hidapi/x64/hidapi.lib /o dist/k400p-fn-lock.exe /I hidapi/include
cl  main.c hidapi/x64/hidapi.lib /o dist/k400p-fn-unlock.exe /D FNUNLOCK /I hidapi/include