#include "utils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <locale>
#include <codecvt>

// Static variables for application utilities
static HANDLE g_appMutex = nullptr;

namespace Utils {

// String conversion utilities
std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), 
                                         nullptr, 0, nullptr, nullptr);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), 
                       &result[0], size_needed, nullptr, nullptr);
    return result;
}

std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), 
                                         nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), 
                       &result[0], size_needed);
    return result;
}

std::wstring ToLower(const std::wstring& str) {
    std::wstring result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

std::wstring Trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L' ');
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L' ');
    return str.substr(first, (last - first + 1));
}

// File and path utilities
std::wstring GetExecutablePath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::wstring(path);
}

std::wstring GetExecutableDirectory() {
    std::wstring path = GetExecutablePath();
    size_t lastSlash = path.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        return path.substr(0, lastSlash);
    }
    return path;
}

bool FileExists(const std::wstring& filePath) {
    DWORD dwAttrib = GetFileAttributesW(filePath.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool CreateDirectoryRecursive(const std::wstring& dirPath) {
    if (dirPath.empty()) return false;
    
    DWORD dwAttrib = GetFileAttributesW(dirPath.c_str());
    if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        return true; // Directory already exists
    }
    
    // Find parent directory
    size_t lastSlash = dirPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        std::wstring parentDir = dirPath.substr(0, lastSlash);
        if (!CreateDirectoryRecursive(parentDir)) {
            return false;
        }
    }
    
    return CreateDirectoryW(dirPath.c_str(), nullptr) != 0;
}

// System information
bool IsWindows7OrLater() {
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW osvi = { 0 };
            osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            
            if (NT_SUCCESS(RtlGetVersion(&osvi))) {
                return (osvi.dwMajorVersion > 6) || 
                       (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 1);
            }
        }
    }
    return false;
}

bool IsWindows10OrLater() {
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW osvi = { 0 };
            osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            
            if (NT_SUCCESS(RtlGetVersion(&osvi))) {
                return osvi.dwMajorVersion >= 10;
            }
        }
    }
    return false;
}

std::wstring GetWindowsVersion() {
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (RtlGetVersion) {
            RTL_OSVERSIONINFOW osvi = { 0 };
            osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
            
            if (NT_SUCCESS(RtlGetVersion(&osvi))) {
                std::wostringstream oss;
                oss << L"Windows " << osvi.dwMajorVersion << L"." << osvi.dwMinorVersion;
                oss << L" Build " << osvi.dwBuildNumber;
                return oss.str();
            }
        }
    }
    return L"Unknown Windows Version";
}

bool IsProcessElevated() {
    BOOL isElevated = FALSE;
    HANDLE hToken = nullptr;
    
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD dwSize;
        
        if (GetTokenInformation(hToken, TokenElevation, &elevation, 
                               sizeof(elevation), &dwSize)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return isElevated != FALSE;
}

size_t GetProcessMemoryUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), 
                            (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

// Graphics API detection
bool IsDirectX9Available() {
    HMODULE hD3D9 = LoadLibraryW(L"d3d9.dll");
    if (hD3D9) {
        FreeLibrary(hD3D9);
        return true;
    }
    return false;
}

bool IsDirectX11Available() {
    HMODULE hD3D11 = LoadLibraryW(L"d3d11.dll");
    if (hD3D11) {
        FreeLibrary(hD3D11);
        return true;
    }
    return false;
}

bool IsOpenGLAvailable() {
    HMODULE hOpenGL32 = LoadLibraryW(L"opengl32.dll");
    if (hOpenGL32) {
        FreeLibrary(hOpenGL32);
        return true;
    }
    return false;
}

std::vector<GraphicsAPI> GetAvailableGraphicsAPIs() {
    std::vector<GraphicsAPI> apis;
    
    if (IsDirectX9Available()) {
        apis.push_back(GraphicsAPI::D3D9);
    }
    if (IsDirectX11Available()) {
        apis.push_back(GraphicsAPI::D3D11);
    }
    if (IsOpenGLAvailable()) {
        apis.push_back(GraphicsAPI::OPENGL);
    }
    
    return apis;
}

// Window utilities
HWND GetForegroundGameWindow() {
    return GetForegroundWindow();
}

std::wstring GetWindowClassName(HWND hwnd) {
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256)) {
        return std::wstring(className);
    }
    return L"";
}

std::wstring GetWindowTitle(HWND hwnd) {
    wchar_t title[256];
    if (GetWindowTextW(hwnd, title, 256)) {
        return std::wstring(title);
    }
    return L"";
}

DWORD GetWindowProcessId(HWND hwnd) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId;
}

bool IsFullscreenWindow(HWND hwnd) {
    RECT windowRect, screenRect;
    GetWindowRect(hwnd, &windowRect);
    GetWindowRect(GetDesktopWindow(), &screenRect);
    
    return (windowRect.left <= screenRect.left &&
            windowRect.top <= screenRect.top &&
            windowRect.right >= screenRect.right &&
            windowRect.bottom >= screenRect.bottom);
}

// Performance Timer implementation
PerformanceTimer::PerformanceTimer() : m_running(false) {}

void PerformanceTimer::Start() {
    m_startTime = std::chrono::high_resolution_clock::now();
    m_running = true;
}

void PerformanceTimer::Stop() {
    m_endTime = std::chrono::high_resolution_clock::now();
    m_running = false;
}

double PerformanceTimer::GetElapsedSeconds() const {
    auto endTime = m_running ? std::chrono::high_resolution_clock::now() : m_endTime;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime);
    return duration.count() / 1000000.0;
}

double PerformanceTimer::GetElapsedMilliseconds() const {
    return GetElapsedSeconds() * 1000.0;
}

// Memory Monitor implementation
MemoryMonitor::MemoryMonitor(size_t maxMemoryMB) : m_maxMemoryMB(maxMemoryMB) {}

bool MemoryMonitor::CheckMemoryUsage() {
    size_t currentUsageMB = GetProcessMemoryUsage() / (1024 * 1024);
    return currentUsageMB <= m_maxMemoryMB;
}

size_t MemoryMonitor::GetCurrentUsageMB() const {
    return GetProcessMemoryUsage() / (1024 * 1024);
}

// Error handling
std::wstring GetLastErrorString() {
    DWORD errorCode = GetLastError();
    if (errorCode == 0) return L"No error";
    
    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPWSTR)&messageBuffer, 0, nullptr);
    
    std::wstring message(messageBuffer, size);
    LocalFree(messageBuffer);
    
    return message;
}

void LogError(const std::wstring& message) {
    std::string utf8Msg = WideToUtf8(L"[ERROR] " + message);
    OutputDebugStringA(utf8Msg.c_str());
}

void LogInfo(const std::wstring& message) {
    std::string utf8Msg = WideToUtf8(L"[INFO] " + message);
    OutputDebugStringA(utf8Msg.c_str());
}

void LogWarning(const std::wstring& message) {
    std::string utf8Msg = WideToUtf8(L"[WARNING] " + message);
    OutputDebugStringA(utf8Msg.c_str());
}

// Application utilities
bool IsApplicationAlreadyRunning() {
    HANDLE hMutex = CreateMutexW(nullptr, FALSE, L"Global\\FPSOverlayMutex");
    if (hMutex == nullptr) return true;
    
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return true;
    }
    
    CloseHandle(hMutex);
    return false;
}

bool CreateMutex(const std::wstring& mutexName) {
    g_appMutex = CreateMutexW(nullptr, TRUE, mutexName.c_str());
    return (g_appMutex != nullptr && GetLastError() != ERROR_ALREADY_EXISTS);
}

void ReleaseMutex() {
    if (g_appMutex) {
        CloseHandle(g_appMutex);
        g_appMutex = nullptr;
    }
}

// Font utilities
bool IsFontInstalled(const std::wstring& fontName) {
    HDC hdc = GetDC(nullptr);
    if (!hdc) return false;
    
    LOGFONTW logFont;
    ZeroMemory(&logFont, sizeof(LOGFONTW));
    wcscpy_s(logFont.lfFaceName, fontName.c_str());
    logFont.lfCharSet = DEFAULT_CHARSET;
    
    bool found = false;
    EnumFontFamiliesExW(hdc, &logFont, [](const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM lParam) -> int {
        bool* pFound = reinterpret_cast<bool*>(lParam);
        *pFound = true;
        return 0; // Stop enumeration
    }, reinterpret_cast<LPARAM>(&found), 0);
    
    ReleaseDC(nullptr, hdc);
    return found;
}

std::vector<std::wstring> GetAvailableSystemFonts() {
    std::vector<std::wstring> fonts;
    HDC hdc = GetDC(nullptr);
    if (!hdc) return fonts;
    
    LOGFONTW logFont;
    ZeroMemory(&logFont, sizeof(LOGFONTW));
    logFont.lfCharSet = DEFAULT_CHARSET;
    
    EnumFontFamiliesExW(hdc, &logFont, [](const LOGFONTW* lpelfe, const TEXTMETRICW*, DWORD, LPARAM lParam) -> int {
        std::vector<std::wstring>* pFonts = reinterpret_cast<std::vector<std::wstring>*>(lParam);
        pFonts->push_back(lpelfe->lfFaceName);
        return 1; // Continue enumeration
    }, reinterpret_cast<LPARAM>(&fonts), 0);
    
    ReleaseDC(nullptr, hdc);
    return fonts;
}

std::wstring GetBestAvailableFont(const std::vector<std::wstring>& preferredFonts) {
    for (const auto& font : preferredFonts) {
        if (IsFontInstalled(font)) {
            return font;
        }
    }
    
    // Fallback fonts that should be available on all Windows systems
    std::vector<std::wstring> fallbackFonts = {
        L"Consolas", L"Courier New", L"Arial", L"Tahoma", L"MS Sans Serif"
    };
    
    for (const auto& font : fallbackFonts) {
        if (IsFontInstalled(font)) {
            return font;
        }
    }
    
    return L"System"; // Final fallback
}

} // namespace Utils