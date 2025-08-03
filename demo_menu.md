# FPS Monitor Menu System Demo

This document demonstrates the terminal-based CLI-style GUI menu system that has been added to the FPS Monitor application.

## Menu Interface Preview

When you run `FPSOverlay.exe --menu`, you'll see a display similar to this:

```
================================================================================
                                FPS MONITOR - CONTROL PANEL
================================================================================
 USER: Administrator
 COMPUTERNAME: WIN-FVRP3BFDJQJ
 CURRENT OS: Windows 11 Pro 24H2   26100.1742   64bit
 Time Zone: Pacific Standard Time
================================================================================
        FOLLOW US: HTTPS://GITHUB.COM/ELCAPITANOE/FPS-MONITOR-WIN
================================================================================

CONFIGURATION
-------------
[1] Change Overlay Position | Set overlay position (Top-Left, Top-Right, Bottom-Left, Bottom-Right)
[2] Change Font Size | Adjust the font size of the overlay text
[3] Change Text Color | Modify the color of the overlay text
[4] Change Background Color | Modify the background color of the overlay
[5] Toggle Overlay | Enable or disable the FPS overlay
[6] Toggle Background | Show or hide the overlay background
[7] Change Update Interval | Set the update frequency of the overlay
[8] Reset to Defaults | Restore all settings to default values
[9] Save Configuration | Save current settings to config file
[10] Load Configuration | Load settings from config file

SYSTEM
------
[11] System Information | Display detailed system information
[12] System Compatibility | Check system compatibility requirements
[13] Graphics APIs | Show available graphics APIs
[14] Memory Usage | Display current memory usage
[15] Process Information | Show current process details
[16] Window Information | Display active window details

PERFORMANCE
-----------
[21] Start Performance Test | Begin performance benchmarking
[22] Stop Performance Test | End performance benchmarking
[23] Performance Statistics | Show performance statistics
[24] Optimize Settings | Automatically optimize settings for best performance

UTILITIES
---------
[31] Clear Console | Clear the console screen
[32] Show Logs | Display application logs
[33] Backup Configuration | Create a backup of current configuration
[34] Restore Configuration | Restore configuration from backup
[35] Validate Configuration | Check configuration file integrity

ADVANCED
--------
[41] Show Help | Display help information
[42] About | Show application information
[99] Exit Application | Exit the FPS Monitor application

================================================================================
NOTE: Type option number to select. Press 'h' for help, 'q' to quit.
Recommended: Use option 1-10 to configure overlay settings.
================================================================================
Type option:
```

## Key Features

### 1. **System Information Header**
- Displays current user, computer name, OS version, and timezone
- Shows real-time system information
- Includes GitHub link for support

### 2. **Categorized Menu Options**
- **Configuration**: Overlay settings and preferences
- **System**: System information and diagnostics  
- **Performance**: Performance monitoring and optimization
- **Utilities**: Utility functions and tools
- **Advanced**: Advanced options and debugging

### 3. **Interactive Navigation**
- Type option numbers to select
- Press 'h' for help
- Press 'q' to quit
- Type 'clear' to clear console

### 4. **Real-time System Data**
- Memory usage monitoring
- Graphics API detection
- Process information
- Window detection

## Usage Examples

### Launch Menu Mode
```bash
FPSOverlay.exe --menu
# or
FPSOverlay.exe -m
```

### Typical Workflow
1. Launch with `--menu` flag
2. View system information at the top
3. Select configuration options (1-10) to customize overlay
4. Use system options (11-16) to check compatibility
5. Monitor performance with options (21-24)
6. Use utilities (31-35) for maintenance
7. Exit with option 99 or 'q'

## Technical Implementation

The menu system is built using:
- **C++17** with modern features
- **Windows Console API** for display
- **Lambda functions** for menu actions
- **STL containers** for option management
- **Exception handling** for robustness

## Benefits

1. **User-Friendly**: No need to edit config files manually
2. **Comprehensive**: Access to all application features
3. **Informative**: Real-time system data display
4. **Extensible**: Easy to add new options and categories
5. **Cross-Platform Ready**: Terminal-based interface works on any Windows system

## Future Enhancements

- Color-coded menu options
- Keyboard shortcuts for common actions
- Configuration import/export
- Performance benchmarking tools
- Log viewer integration
- Real-time FPS display in menu 