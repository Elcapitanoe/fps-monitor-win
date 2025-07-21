# FPS Monitor for Windows

A lightweight, real-time FPS (Frames Per Second) monitoring overlay for Windows applications and games.

## Features

- **Real-time FPS monitoring** with high accuracy
- **Low system impact** - minimal CPU and memory usage  
- **Multiple graphics API support** - DirectX 9/11, OpenGL
- **Customizable overlay** - position, colors, fonts
- **Automatic game detection** - works with most applications
- **Console and GUI support** - flexible usage options

## Quick Start

1. Download the latest release from [Releases](https://github.com/Elcapitanoe/fps-monitor-win/releases)
2. Run `FPSOverlay.exe`
3. The FPS overlay will appear on your screen
4. Use `Ctrl+C` or close the console to stop

## Troubleshooting

### FPS Shows Wrong Values (e.g., 10fps on 100hz monitor)
This was a bug in earlier versions where FPS was calculated every 500ms instead of per frame. **Fixed in v1.0.0+**:
- Now uses 16ms update intervals for proper frame detection
- Improved FPS calculation algorithm with smoothing
- Accurate readings for high refresh rate monitors (100hz, 144hz, etc.)

### Windows Defender Virus Detection
This is a **false positive** common with frame detection software:
- **Why it happens**: The software monitors graphics APIs and processes
- **It's safe**: Source code is open and available for review
- **Solutions**:
  - Add exception in Windows Defender for `FPSOverlay.exe`
  - Download from official GitHub releases only
  - Compile from source if concerned

### Missing DLL Errors (VCRUNTIME140.dll, etc.)
Install **Microsoft Visual C++ Redistributable**:
- Download from [Microsoft's official site](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)
- Or use "VC Redist All in One" package
- **Alternative**: Use the statically linked build (larger file, no dependencies)

## Configuration

Edit `config.ini` to customize:
```ini
[Display]
Position=TOP_LEFT
FontSize=16
TextColor=0,255,0,255
UpdateInterval=16

[Advanced]
EnableHooks=true
MemoryLimit=25MB
```

## Build from Source

### Requirements
- Windows 10/11
- Visual Studio 2019+ or CMake 3.10+
- Windows SDK

### Build Steps
```bash
git clone https://github.com/Elcapitanoe/fps-monitor-win.git
cd fps-monitor-win
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Command Line Options

```bash
FPSOverlay.exe [options]

Options:
  --help, -h, /?     Show help message
  --version, -v      Show version information  
  --config <file>    Use custom config file
  --exit             Stop any running instance
```

## Technical Details

- **Update Frequency**: 16ms (≈60fps polling) for accurate detection
- **Memory Usage**: <25MB typical
- **CPU Impact**: <1% on modern systems
- **Compatibility**: Windows 7+ (64-bit)

## License

MIT License - see [LICENSE](LICENSE) for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Support

- **Issues**: [GitHub Issues](https://github.com/Elcapitanoe/fps-monitor-win/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Elcapitanoe/fps-monitor-win/discussions)
- **Documentation**: See [docs/](docs/) folder