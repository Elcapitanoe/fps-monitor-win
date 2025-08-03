@echo off
echo Testing FPS Monitor Menu System
echo ================================
echo.

echo Building the application...
call build.bat

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Testing menu system...
echo Launching FPS Monitor with menu mode...
echo.

FPSOverlay.exe --menu

echo.
echo Menu test completed.
pause 