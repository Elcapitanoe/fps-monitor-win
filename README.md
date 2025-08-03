

# FPS Monitor for Windows

A lightweight, real-time overlay tool for monitoring frames per second (FPS) in games and applications on Windows.

## Core Features

- **Accurate FPS Tracking**: Get precise, up-to-the-second frame rate stats.  
- **Low Resource Usage**: Barely touches your CPU or memory.  
- **Wide API Support**: Works with DirectX 9, DirectX 11, and OpenGL.  
- **Overlay Customization**: Tweak position, colors, and font sizes to suit your setup.  
- **Auto-Detection**: Spots running games and apps without manual setup.  

## Quick Setup

1. Head to the [Releases page](https://github.com/Elcapitanoe/fps-monitor-win/releases) and download the latest version.  
2. Launch `FPSOverlay.exe`.  
3. See the FPS overlay pop up on your screen.  
4. To exit, press `Ctrl+C` in the console or close the window.  

## Troubleshooting Tips

### FPS Readings Look Off (e.g., 10 FPS on a 100 Hz Monitor)  
Older versions sampled FPS every 500 ms, which threw off results. **Fixed in v1.1+**:  
- Now refreshes every 16 ms for spot-on accuracy.  
- Smoother algorithm handles high refresh rates (75 Hz, 100 Hz, 144 Hz, etc.) flawlessly.  

### Windows Defender Flags It  
A **false positive**, common for tools hooking into graphics APIs:  
- **Reason**: It interacts with graphics processes, triggering suspicion.  
- **Safety**: Open-source and fully auditable.  
- **Fixes**:  
  - Add `FPSOverlay.exe` to Defender’s exclusions.  
  - Stick to official GitHub downloads.  
  - Build from source for peace of mind.  

### Missing DLLs (e.g., VCRUNTIME140.dll)  
You need the **Microsoft Visual C++ Redistributable**:  
- Grab it from [Microsoft’s site](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist).  
- Or install the "VC Redist All in One" package.  
- **Bonus Option**: Use the statically linked build, zero dependencies.  

## Customization

Fine-tune the overlay with `config.ini`. Example:

```ini
[Display]  
Position=TOP_RIGHT  
FontSize=18  
TextColor=255,255,0,255  
UpdateInterval=16  

[Advanced]  
EnableHooks=true  
MemoryLimit=25MB  
```

- **Position**: Set overlay location (e.g., TOP_RIGHT, BOTTOM_LEFT).  
- **FontSize**: Adjust text size for visibility.  
- **TextColor**: Define RGBA values (red, green, blue, alpha).  
- **UpdateInterval**: Refresh rate in milliseconds.  
- **EnableHooks**: Toggle API hooking for advanced tracking.  
- **MemoryLimit**: Restrict memory usage.  

## Build It Yourself

### Prerequisites  
- Windows 10 or 11  
- Visual Studio 2019 (or later) or CMake 3.10+  
- Windows SDK  

### Build Instructions  
1. Clone the repo:  
   ```bash  
   git clone https://github.com/Elcapitanoe/fps-monitor-win.git  
   ```  
2. Navigate to the directory:  
   ```bash  
   cd fps-monitor-win  
   ```  
3. Create a build folder:  
   ```bash  
   mkdir build && cd build  
   ```  
4. Generate build files:  
   ```bash  
   cmake .. -G "Visual Studio 17 2022" -A x64  
   ```  
5. Compile:  
   ```bash  
   cmake --build . --config Release  
   ```  

## Command Line Basics

Launch `FPSOverlay.exe` with these options:  

- `--help`, `-h`, `/?`: Display usage info.
- `--menu`, `-m`: Launch the interactive control panel menu.

## Interactive Control Panel

The FPS Monitor includes a comprehensive terminal-based control panel that provides easy access to all features and settings.

### Launching the Menu

```bash
FPSOverlay.exe --menu
# or
FPSOverlay.exe -m
# or simply run without arguments (default behavior)
FPSOverlay.exe
```

### Command Line Options

- `FPSOverlay.exe` - Launch interactive control panel (default)
- `FPSOverlay.exe --menu` or `FPSOverlay.exe -m` - Launch interactive control panel
- `FPSOverlay.exe --help` or `FPSOverlay.exe -h` - Show help information
- `FPSOverlay.exe --version` or `FPSOverlay.exe -v` - Show version information

### Menu Features

The control panel is organized into logical categories:

#### Configuration
- **Change Overlay Position**: Set overlay position (Top-Left, Top-Right, Bottom-Left, Bottom-Right)
- **Change Font Size**: Adjust the font size of the overlay text
- **Change Text Color**: Modify the color of the overlay text
- **Change Background Color**: Modify the background color of the overlay
- **Toggle Overlay**: Enable or disable the FPS overlay
- **Toggle Background**: Show or hide the overlay background
- **Change Update Interval**: Set the update frequency of the overlay
- **Reset to Defaults**: Restore all settings to default values
- **Save Configuration**: Save current settings to config file
- **Load Configuration**: Load settings from config file

#### System
- **System Information**: Display detailed system information
- **System Compatibility**: Check system compatibility requirements
- **Graphics APIs**: Show available graphics APIs
- **Memory Usage**: Display current memory usage
- **Process Information**: Show current process details
- **Window Information**: Display active window details

#### Performance
- **Start Performance Test**: Begin performance benchmarking
- **Stop Performance Test**: End performance benchmarking
- **Performance Statistics**: Show performance statistics
- **Optimize Settings**: Automatically optimize settings for best performance

#### Utilities
- **Clear Console**: Clear the console screen
- **Show Logs**: Display application logs
- **Backup Configuration**: Create a backup of current configuration
- **Restore Configuration**: Restore configuration from backup
- **Validate Configuration**: Check configuration file integrity

#### Advanced
- **Show Help**: Display help information
- **About**: Show application information
- **Exit Application**: Exit the FPS Monitor application

### Navigation

- Type the number of the option you want to select
- Press 'h' for help
- Press 'q' to quit
- Type 'clear' to clear the console

### System Information Display

The menu header shows real-time system information:
- Current user and computer name
- Windows version and build
- Time zone information
- Available graphics APIs  
- `--version`, `-v`: Show the tool’s version.  
- `--config <file>`: Load a custom config file.  
- `--exit`: Terminate any running instance.  

## Tech Specs

- **Polling Rate**: 16 ms (~60 FPS) for fluid updates.  
- **Memory Use**: Typically under 25 MB.  
- **CPU Load**: Less than 1% on modern rigs.  
- **OS Support**: Windows 7+ (64-bit only).  

## License

Released under the MIT License. See [LICENSE](LICENSE) for details.  

## Contributing

1. Fork the repository.  
2. Create a branch for your changes.  
3. Implement your feature or fix.  
4. Add tests if relevant.  
5. Submit a pull request.  

## Troubleshooting

### Common Issues

1. **Application Won't Launch**: 
   - Make sure you're running the executable directly: `FPSOverlay.exe`
   - Try running with help flag: `FPSOverlay.exe --help`
   - Check if antivirus is blocking the application

2. **No Menu Appears**: 
   - The application now defaults to showing the interactive menu when run without arguments
   - If no interface appears, try: `FPSOverlay.exe --menu`

3. **Antivirus False Positives**: The application may trigger antivirus warnings due to its frame detection capabilities. This is a false positive - the source code is available on GitHub.

4. **Windows Version**: Requires Windows 7 or later.

5. **Graphics APIs**: The application supports DirectX 9, DirectX 11, and OpenGL applications.

### Fixed in v1.2+:

- **Default Menu Launch**: Application now launches the interactive control panel by default
- **Better Command Line Interface**: Added help and version commands
- **Improved Error Handling**: Better error messages and logging
- **Enhanced Menu System**: Categorized options with better organization
- **Fixed Compilation Issues**: Resolved all compilation errors and deprecation warnings

## Get Support

- **Report Bugs**: Use [GitHub Issues](https://github.com/Elcapitanoe/fps-monitor-win/issues).
