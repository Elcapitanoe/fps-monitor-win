@echo off
echo FPS Overlay - Windows Defender Exclusion Helper
echo ================================================
echo.
echo This script will add FPS Overlay to Windows Defender exclusions
echo to prevent false positive virus detections.
echo.
echo NOTE: This requires Administrator privileges
echo.
pause

echo Adding exclusion for current directory...
powershell -Command "Add-MpPreference -ExclusionPath '%~dp0'"

echo Adding exclusion for FPSOverlay.exe...
powershell -Command "Add-MpPreference -ExclusionProcess 'FPSOverlay.exe'"

echo.
echo Exclusions added successfully!
echo You can now run FPSOverlay.exe without antivirus warnings.
echo.
pause