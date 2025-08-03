# Building FPS Overlay v1.3.0

This guide provides comprehensive instructions for building the FPS Overlay application on Windows systems.

## Prerequisites

### Required Software

1. **Operating System**: Windows 7 SP1 or later
2. **Compiler**: One of the following:
   - Visual Studio 2019 or later (recommended)
   - Visual Studio Build Tools 2019+
   - MinGW-w64 8.0 or later
3. **Build System**: CMake 3.10 or later
4. **Git**: For source code management (optional)

### Visual Studio Setup (Recommended)

**Option 1: Visual Studio Community (Free)**
1. Download from: https://visualstudio.microsoft.com/vs/community/
2. During installation, select:
   - "Desktop development with C++" workload
   - Windows 10/11 SDK (latest version)
   - CMake tools for C++

**Option 2: Build Tools Only**
1. Download Visual Studio Build Tools
2. Install C++ build tools and Windows SDK

### MinGW-w64 Setup (Alternative)

**Option 1: MSYS2 (Recommended for MinGW)**
1. Download from: https://www.msys2.org/
2. Install and update: `pacman -Syu`
3. Install tools: `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake`

**Option 2: Standalone MinGW-w64**
1. Download from: https://winlibs.com/
2. Extract to C:\mingw64
3. Add C:\mingw64\bin to PATH

### CMake Installation

1. Download from: https://cmake.org/download/
2. During installation, select "Add CMake to system PATH"
3. Verify installation: `cmake --version`

## Quick Build

### Using the Build Script (Easiest)

```batch
# Clone or extract source code
cd FPSOverlay

# Run the automated build script
build.bat

# The executable will be in build\Release\ or build\ directory
```

### Manual Build with Visual Studio

```batch
# Create build directory
mkdir build
cd build

# Configure with CMake (64-bit)
cmake -G "Visual Studio 16 2019" -A x64 ..

# Build release version
cmake --build . --config Release

# Or build debug version
cmake --build . --config Debug
```

### Manual Build with MinGW

```batch
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .
# Or: mingw32-make
```

## Advanced Build Options

### CMake Configuration Options

```batch
# 32-bit build (Visual Studio)
cmake -G "Visual Studio 16 2019" -A Win32 ..

# Specify Windows SDK version
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_SYSTEM_VERSION=10.0.19041.0 ..

# Debug build with MinGW
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..

# Enable verbose output
cmake --build . --config Release --verbose
```

### Compiler-Specific Flags

The CMakeLists.txt automatically sets optimized flags:

**MSVC:**
- `/W3` - Warning level 3
- `/O2` - Maximum optimization
- `/MT` - Static runtime linking

**MinGW/GCC:**
- `-Wall` - All warnings
- `-O2` - Optimization level 2
- `-static-libgcc -static-libstdc++` - Static linking

### Target Architecture

**64-bit (x64) - Recommended:**
```batch
cmake -G "Visual Studio 16 2019" -A x64 ..
```

**32-bit (x86) - For compatibility:**
```batch
cmake -G "Visual Studio 16 2019" -A Win32 ..
```

## Build Targets

### Primary Target
- `FPSOverlay.exe` - Main executable

### Files Generated
- `FPSOverlay.exe` - The main application
- `config.ini` - Configuration file (copied automatically)

## Troubleshooting

### Common Build Issues

**1. CMake not found**
```
Solution: Add CMake to PATH or specify full path
cmake --version
```

**2. Compiler not found**
```
# For Visual Studio
"C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

# For MinGW
set PATH=C:\mingw64\bin;%PATH%
```

**3. Windows SDK missing**
```
Solution: Install Windows 10/11 SDK via Visual Studio Installer
```

**4. D3D/DirectX headers missing**
```
Solution: The project includes basic DirectX headers.
For advanced features, install DirectX SDK or use Windows SDK.
```

**5. Link errors with GDI32/User32**
```
Solution: These are automatically linked via CMakeLists.txt
Check Windows SDK installation if issues persist.
```

### Debug Build Issues

**Enable debug symbols:**
```batch
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Verbose build output:**
```batch
cmake --build . --config Release --verbose
```

### Memory Issues

The application is designed to use <25MB RAM. If you encounter memory issues:

1. Check for memory leaks in debug builds
2. Verify RAII patterns are followed
3. Ensure all resources are properly released

## Build Verification

### Testing the Build

```batch
# Check executable exists
dir build\Release\FPSOverlay.exe

# Test basic functionality
FPSOverlay.exe --version

# Test configuration loading
FPSOverlay.exe --help
```

### Performance Verification

```batch
# Run and monitor memory usage
FPSOverlay.exe
# Check Task Manager for memory usage (<25MB expected)
```

## Deployment

### Standalone Deployment

The application is designed to be standalone:
- Single executable file
- No external DLL dependencies (when built with static linking)
- Configuration file in same directory

### Distribution Package

Recommended distribution structure:
```
FPSOverlay-v1.3.0/
├── FPSOverlay.exe
├── config.ini
├── README.md
└── LICENSE
```

### System Requirements for Built Application

**Minimum:**
- Windows 7 SP1 (64-bit) or Windows 10 (32-bit)
- 50MB free disk space
- 25MB available RAM
- DirectX 9 or OpenGL support

**Recommended:**
- Windows 10/11 (64-bit)
- DirectX 11 support
- 1GB available RAM

## Cross-Compilation

### From Linux (Advanced)

```bash
# Install MinGW cross-compiler
sudo apt-get install mingw-w64

# Configure for Windows
cmake -DCMAKE_TOOLCHAIN_FILE=mingw64.cmake ..

# Build
make
```

Note: Cross-compilation requires additional setup and may have limited testing.

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build Windows
on: [push, pull_request]
jobs:
  build:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v2
    - name: Build
      run: |
        mkdir build
        cd build
        cmake -G "Visual Studio 16 2019" -A x64 ..
        cmake --build . --config Release
```

## Getting Help

If you encounter build issues:

1. Check this guide for common solutions
2. Verify all prerequisites are installed
3. Try a clean build: `rm -rf build && mkdir build`
4. Check CMake and compiler versions
5. Review error messages carefully

For development questions, refer to the source code comments and documentation.