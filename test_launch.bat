@echo off
echo Testing FPS Monitor Launch
echo ===========================
echo.

echo Building the application...
call build.bat

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Testing application launch...
echo.

echo 1. Testing help command...
FPSOverlay.exe --help

echo.
echo 2. Testing version command...
FPSOverlay.exe --version

echo.
echo 3. Testing default launch (should show menu)...
echo Press any key to continue to menu test...
pause

FPSOverlay.exe

echo.
echo Launch test completed.
pause 