@echo off
set APP_PATH="%~dp0k400p-fn-tray.exe"
set STARTUP_FOLDER="%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup"

echo Creating startup shortcut...
powershell -command "$s=(New-Object -COM WScript.Shell).CreateShortcut('%STARTUP_FOLDER%'+'\k400p-fn-tray.lnk'); $s.TargetPath='%APP_PATH%'; $s.Save()"
pause
