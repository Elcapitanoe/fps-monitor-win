#pragma once

#include "common.h"

namespace Utils {
    // String conversion utilities
    std::string WideToUtf8(const std::wstring& wide);
    std::wstring Utf8ToWide(const std::string& utf8);
    std::wstring ToLower(const std::wstring& str);
    std::wstring Trim(const std::wstring& str);
    
    // File and path utilities
    std::wstring GetExecutablePath();
    std::wstring GetExecutableDirectory();
    bool FileExists(const std::wstring& filePath);
    bool CreateDirectoryRecursive(const std::wstring& dirPath);
    
    // System information
    bool IsWindows7OrLater();
    bool IsWindows10OrLater();
    std::wstring GetWindowsVersion();
    bool IsProcessElevated();
    size_t GetProcessMemoryUsage();
    
    // Graphics API detection
    bool IsDirectX9Available();
    bool IsDirectX11Available();
    bool IsOpenGLAvailable();
    std::vector<GraphicsAPI> GetAvailableGraphicsAPIs();
    
    // Window utilities
    HWND GetForegroundGameWindow();
    std::wstring GetWindowClassName(HWND hwnd);
    std::wstring GetWindowTitle(HWND hwnd);
    DWORD GetWindowProcessId(HWND hwnd);
    bool IsFullscreenWindow(HWND hwnd);
    
    // Performance utilities
    class PerformanceTimer {
    public:
        PerformanceTimer();
        void Start();
        void Stop();
        double GetElapsedSeconds() const;
        double GetElapsedMilliseconds() const;
        
    private:
        std::chrono::high_resolution_clock::time_point m_startTime;
        std::chrono::high_resolution_clock::time_point m_endTime;
        bool m_running;
    };
    
    // Memory utilities
    class MemoryMonitor {
    public:
        MemoryMonitor(size_t maxMemoryMB = 25);
        bool CheckMemoryUsage();
        size_t GetCurrentUsageMB() const;
        size_t GetMaxUsageMB() const { return m_maxMemoryMB; }
        
    private:
        size_t m_maxMemoryMB;
    };
    
    // Configuration file utilities
    bool BackupConfigFile(const std::wstring& configPath);
    bool RestoreConfigFile(const std::wstring& configPath);
    bool ValidateConfigFile(const std::wstring& configPath);
    
    // Error handling
    std::wstring GetLastErrorString();
    void LogError(const std::wstring& message);
    void LogInfo(const std::wstring& message);
    void LogWarning(const std::wstring& message);
    
    // Application utilities
    bool IsApplicationAlreadyRunning();
    bool CreateMutex(const std::wstring& mutexName);
    void ReleaseMutex();
    
    // Font utilities
    bool IsFontInstalled(const std::wstring& fontName);
    std::vector<std::wstring> GetAvailableSystemFonts();
    std::wstring GetBestAvailableFont(const std::vector<std::wstring>& preferredFonts);
}