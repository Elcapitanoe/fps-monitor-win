@echo off
echo Building FPS Overlay v1.3.0
echo ================================

:: Check if Visual Studio is available
where cl >nul 2>nul
if %ERRORLEVEL% == 0 (
    echo Using Visual Studio compiler
    goto :build_msvc
)

:: Check if MinGW is available
where g++ >nul 2>nul
if %ERRORLEVEL% == 0 (
    echo Using MinGW compiler
    goto :build_mingw
)

echo Error: No suitable compiler found!
echo Please install Visual Studio or MinGW-w64
pause
exit /b 1

:build_msvc
echo Building with MSVC...
mkdir build 2>nul
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)
goto :build_success

:build_mingw
echo Building with MinGW...
mkdir build 2>nul
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)
cmake --build .
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)
goto :build_success

:build_success
echo.
echo Build completed successfully!
echo Executable location: build/Release/FPSOverlay.exe (MSVC) or build/FPSOverlay.exe (MinGW)
echo.
echo Copying configuration file...
copy ..\config.ini . >nul 2>nul
echo.
echo Ready to run! Execute FPSOverlay.exe to start the overlay.
echo Use FPSOverlay.exe --help for command line options.
echo.
pause